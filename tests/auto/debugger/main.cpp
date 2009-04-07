
#include "gdbmi.h"

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtTest/QtTest>
//#include <QtTest/qtest_gui.h>

using namespace Debugger;
using namespace Debugger::Internal;

static const char test1[] =
    "[frame={level=\"0\",addr=\"0x00000000004061ca\","
    "func=\"main\",file=\"test1.cpp\","
    "fullname=\"/home/apoenitz/work/test1/test1.cpp\",line=\"209\"}]";

static const char test2[] =
    "[frame={level=\"0\",addr=\"0x00002ac058675840\","
    "func=\"QApplication\",file=\"/home/apoenitz/dev/qt/src/gui/kernel/qapplication.cpp\","
    "fullname=\"/home/apoenitz/dev/qt/src/gui/kernel/qapplication.cpp\",line=\"592\"},"
    "frame={level=\"1\",addr=\"0x00000000004061e0\",func=\"main\",file=\"test1.cpp\","
    "fullname=\"/home/apoenitz/work/test1/test1.cpp\",line=\"209\"}]";

static const char test3[] =
    "[stack={frame={level=\"0\",addr=\"0x00000000004061ca\","
    "func=\"main\",file=\"test1.cpp\","
    "fullname=\"/home/apoenitz/work/test1/test1.cpp\",line=\"209\"}}]";

static const char test4[] =
    "&\"source /home/apoenitz/dev/ide/main/bin/gdb/qt4macros\\n\""
    "4^done\n";

static const char test5[] =
    "[reason=\"breakpoint-hit\",bkptno=\"1\",thread-id=\"1\","
    "frame={addr=\"0x0000000000405738\",func=\"main\","
    "args=[{name=\"argc\",value=\"1\"},{name=\"argv\",value=\"0x7fff1ac78f28\"}],"
    "file=\"test1.cpp\",fullname=\"/home/apoenitz/work/test1/test1.cpp\","
    "line=\"209\"}]";

static const char test8[] =
    "[data={locals={{name=\"a\"},{name=\"w\"}}}]";

static const char test9[] =
    "[data={locals=[name=\"baz\",name=\"urgs\",name=\"purgs\"]}]";

static const char test10[] =
    "[name=\"urgs\",numchild=\"1\",type=\"Urgs\"]";

static const char test11[] =
    "[{name=\"size\",value=\"1\",type=\"size_t\",readonly=\"true\"},"
     "{name=\"0\",value=\"one\",type=\"QByteArray\"}]";

static const char test12[] =
    "[{iname=\"local.hallo\",value=\"\\\"\\\"\",type=\"QByteArray\","
     "numchild=\"0\"}]";

class tst_Debugger : public QObject
{
    Q_OBJECT

public:
    tst_Debugger() {}

    void testMi(const char* input)
    {
        QCOMPARE('\n' + QString::fromLatin1(GdbMi(input).toString(false)),
            '\n' + QString(input));
    }

private slots:
    void mi1()  { testMi(test1);  }
    void mi2()  { testMi(test2);  }
    void mi3()  { testMi(test3);  }
    //void mi4()  { testMi(test4);  }
    void mi5()  { testMi(test5);  }
    void mi8()  { testMi(test8);  }
    void mi9()  { testMi(test9);  }
    void mi10() { testMi(test10); }
    void mi11() { testMi(test11); }
    void mi12() { testMi(test12); }
    void runQtc();

public slots:
    void readStandardOutput();
    void readStandardError();

private:
    QProcess m_proc; // the Qt Creator process
};

static QByteArray stripped(QByteArray ba)
{
    for (int i = ba.size(); --i >= 0; ) {
        if (ba.at(i) == '\n' || ba.at(i) == ' ')
            ba.chop(1);
        else
            break;
    }
    return ba;
}

void tst_Debugger::readStandardOutput()
{
    qDebug() << "qtcreator-out: " << stripped(m_proc.readAllStandardOutput());
}

void tst_Debugger::readStandardError()
{
    qDebug() << "qtcreator-err: " << stripped(m_proc.readAllStandardError());
}

void tst_Debugger::runQtc()
{
    QString test = QFileInfo(qApp->arguments().at(0)).absoluteFilePath();
    QString qtc = QFileInfo(test).absolutePath() + "/../../../bin/qtcreator.bin";
    qtc = QFileInfo(qtc).absoluteFilePath();
    QStringList env = QProcess::systemEnvironment();
    env.append("QTC_DEBUGGER_TEST=" + test);
    m_proc.setEnvironment(env);
    connect(&m_proc, SIGNAL(readyReadStandardOutput()),
        this, SLOT(readStandardOutput()));
    connect(&m_proc, SIGNAL(readyReadStandardError()),
        this, SLOT(readStandardError()));
    m_proc.start(qtc);
    m_proc.waitForStarted();
    QCOMPARE(m_proc.state(), QProcess::Running);
    m_proc.waitForFinished();
    QCOMPARE(m_proc.state(), QProcess::NotRunning);
}

void runDebuggee()
{
    qDebug() << "RUNNING DEBUGGEE";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();


    if (args.size() == 2 && args.at(1) == "--run-debuggee") {
        runDebuggee();
        return 0;
    }

    tst_Debugger test;
    return QTest::qExec(&test, argc, argv);
}

#include "main.moc"

