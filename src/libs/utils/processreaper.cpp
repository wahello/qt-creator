/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "processreaper.h"
#include "qtcassert.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QHash>
#include <QProcess>
#include <QTimer>
#include <QWaitCondition>

using namespace Utils;

namespace Utils {
namespace Internal {

/*

Observations on how QProcess::terminate() behaves on different platforms when called for
never ending running process:

1. Windows:

   The issue in QTCREATORBUG-27118 with adb.exe is most probably a special
   Windows only case described in docs of QProcess::terminate():

   "Console applications on Windows that do not run an event loop, or whose event loop does
   not handle the WM_CLOSE message, can only be terminated by calling kill()."

   It looks like when you call terminate() for the adb.exe, it won't stop, never, even after
   default 30 seconds timeout. The same happens for blocking processes tested in
   tst_QtcProcess::killBlockingProcess(). It's hard to say whether any process on Windows can
   be finished by a call to terminate(). Until now, no such a process has been found.

   Further call to kill() (after a call to terminate()) finishes the process quickly.

2. Linux:

   It looks like a call to terminate() finishes the running process after a long wait
   (between 10-15 seconds). After calling terminate(), further calls to kill() doesn't
   make the process to finish soon (are no-op). On the other hand, when we start reaping the
   running process from a call to kill() without a prior call to terminate(), the process
   finishes quickly.

3. Mac:

   It looks like the process finishes quickly after a call to terminate().

*/

static const int s_timeoutThreshold = 10000; // 10 seconds

static QString execWithArguments(QProcess *process)
{
    QStringList commandLine;
    commandLine.append(process->program());
    commandLine.append(process->arguments());
    return commandLine.join(QChar::Space);
}

struct ReaperSetup
{
    QProcess *m_process = nullptr;
    int m_timeoutMs;
};

class Reaper : public QObject
{
    Q_OBJECT

public:
    Reaper(const ReaperSetup &reaperSetup) : m_reaperSetup(reaperSetup) {}

    void reap()
    {
        m_timer.start();

        connect(m_reaperSetup.m_process,
                QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &Reaper::handleFinished);

        if (emitFinished())
            return;

        terminate();
    }

signals:
    void finished();

private:
    void terminate()
    {
        // TODO: do a custom terminate here for ctrlCStub
        m_reaperSetup.m_process->terminate();
        QTimer::singleShot(m_reaperSetup.m_timeoutMs, this, &Reaper::handleTerminateTimeout);
    }

    void kill() { m_reaperSetup.m_process->kill(); }

    bool emitFinished()
    {
        if (m_reaperSetup.m_process->state() != QProcess::NotRunning)
            return false;

        if (!m_finished) {
            const int timeout = m_timer.elapsed();
            if (timeout > s_timeoutThreshold) {
                qWarning() << "Finished parallel reaping of" << execWithArguments(m_reaperSetup.m_process)
                           << "in" << (timeout / 1000.0) << "seconds.";
            }

            m_finished = true;
            emit finished();
        }
        return true;
    }

    void handleFinished()
    {
        // In case the process is still running - wait until it has finished
        QTC_ASSERT(emitFinished(), QTimer::singleShot(m_reaperSetup.m_timeoutMs,
                                                      this, &Reaper::handleFinished));
    }

    void handleTerminateTimeout()
    {
        if (emitFinished())
            return;

        kill();
    }

    bool m_finished = false;
    QElapsedTimer m_timer;
    const ReaperSetup m_reaperSetup;
};

class ProcessReaperPrivate : public QObject
{
    Q_OBJECT

public:
    // Called from non-reaper's thread
    void scheduleReap(const ReaperSetup &reaperSetup)
    {
        QTC_CHECK(QThread::currentThread() != thread());
        QMutexLocker locker(&m_mutex);
        m_reaperSetupList.append(reaperSetup);
        QMetaObject::invokeMethod(this, &ProcessReaperPrivate::flush);
    }

    // Called from non-reaper's thread
    void waitForFinished()
    {
        QTC_CHECK(QThread::currentThread() != thread());
        QMetaObject::invokeMethod(this, &ProcessReaperPrivate::flush,
                                  Qt::BlockingQueuedConnection);
        QMutexLocker locker(&m_mutex);
        if (m_reaperList.isEmpty())
            return;

        m_waitCondition.wait(&m_mutex);
    }

private:
    // All the private methods are called from the reaper's thread
    QList<ReaperSetup> takeReaperSetupList()
    {
        QMutexLocker locker(&m_mutex);
        const QList<ReaperSetup> reaperSetupList = m_reaperSetupList;
        m_reaperSetupList.clear();
        return reaperSetupList;
    }

    void flush()
    {
        while (true) {
            const QList<ReaperSetup> reaperSetupList = takeReaperSetupList();
            if (reaperSetupList.isEmpty())
                return;
            for (const ReaperSetup &reaperSetup : reaperSetupList)
                reap(reaperSetup);
        }
    }

    void reap(const ReaperSetup &reaperSetup)
    {
        Reaper *reaper = new Reaper(reaperSetup);
        connect(reaper, &Reaper::finished, this, [this, reaper, process = reaperSetup.m_process] {
            QMutexLocker locker(&m_mutex);
            QTC_CHECK(m_reaperList.removeOne(reaper));
            delete reaper;
            delete process;
            if (m_reaperList.isEmpty())
                m_waitCondition.wakeOne();
        }, Qt::QueuedConnection);

        {
            QMutexLocker locker(&m_mutex);
            m_reaperList.append(reaper);
        }

        reaper->reap();
    }

    QMutex m_mutex;
    QWaitCondition m_waitCondition;
    QList<ReaperSetup> m_reaperSetupList;
    QList<Reaper *> m_reaperList;
};

} // namespace Internal

using namespace Utils::Internal;

static QMutex s_instanceMutex;

ProcessReaper::ProcessReaper()
    : m_private(new ProcessReaperPrivate())
{
    m_private->moveToThread(&m_thread);
    QObject::connect(&m_thread, &QThread::finished, m_private, &QObject::deleteLater);
    m_thread.start();
    m_thread.moveToThread(qApp->thread());
}

ProcessReaper::~ProcessReaper()
{
    QTC_CHECK(QThread::currentThread() == qApp->thread());
    QMutexLocker locker(&s_instanceMutex);
    instance()->m_private->waitForFinished();
    m_thread.quit();
    m_thread.wait();
}

void ProcessReaper::reap(QProcess *process, int timeoutMs)
{
    if (!process)
        return;

    QTC_ASSERT(QThread::currentThread() == process->thread(), return);

    process->disconnect();
    if (process->state() == QProcess::NotRunning) {
        process->deleteLater();
        return;
    }

    // Neither can move object with a parent into a different thread
    // nor reaping the process with a parent makes any sense.
    process->setParent(nullptr);

    QMutexLocker locker(&s_instanceMutex);
    ProcessReaperPrivate *priv = instance()->m_private;

    process->moveToThread(priv->thread());
    ReaperSetup reaperSetup {process, timeoutMs};
    priv->scheduleReap(reaperSetup);
}

} // namespace Utils

#include "processreaper.moc"
