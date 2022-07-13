/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#pragma once

#include "testconfiguration.h"

#include "autotesttr.h"

#include <debugger/debuggerrunconfigurationaspect.h>

#include <projectexplorer/devicesupport/devicemanager.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/runconfiguration.h>

#include <utils/qtcassert.h>

namespace Autotest {
namespace Internal {

class TestRunConfiguration : public ProjectExplorer::RunConfiguration
{
public:
    TestRunConfiguration(ProjectExplorer::Target *parent, TestConfiguration *config)
        : ProjectExplorer::RunConfiguration(parent, "AutoTest.TestRunConfig")
    {
        setDefaultDisplayName(Tr::tr("AutoTest Debug"));

        bool enableQuick = false;
        if (auto debuggable = dynamic_cast<DebuggableTestConfiguration *>(config))
            enableQuick = debuggable->mixedDebugging();

        auto debugAspect = addAspect<Debugger::DebuggerRunConfigurationAspect>(parent);
        debugAspect->setUseQmlDebugger(enableQuick);
        ProjectExplorer::ProjectExplorerPlugin::updateRunActions();
        m_testConfig = config;
    }

    ProjectExplorer::Runnable runnable() const override
    {
        ProjectExplorer::Runnable r;
        QTC_ASSERT(m_testConfig, return r);
        r.command.setExecutable(m_testConfig->executableFilePath());
        r.command.addArgs(m_testConfig->argumentsForTestRunner().join(' '), Utils::CommandLine::Raw);
        r.workingDirectory = m_testConfig->workingDirectory();
        r.environment = m_testConfig->environment();
        return r;
    }

private:
    TestConfiguration *m_testConfig = nullptr;
};

} // namespace Internal
} // namespace Autotest
