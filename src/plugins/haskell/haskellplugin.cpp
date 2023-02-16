// Copyright (c) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "haskellplugin.h"

#include "haskellbuildconfiguration.h"
#include "haskellconstants.h"
#include "haskelleditorfactory.h"
#include "haskellmanager.h"
#include "haskellproject.h"
#include "haskellrunconfiguration.h"
#include "haskelltr.h"
#include "optionspage.h"
#include "stackbuildstep.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/jsonwizard/jsonwizardfactory.h>
#include <texteditor/snippets/snippetprovider.h>

#include <QAction>

namespace Haskell {
namespace Internal {

class HaskellPluginPrivate
{
public:
    HaskellEditorFactory editorFactory;
    OptionsPage optionsPage;
    HaskellBuildConfigurationFactory buildConfigFactory;
    StackBuildStepFactory stackBuildStepFactory;
    HaskellRunConfigurationFactory runConfigFactory;
    ProjectExplorer::SimpleTargetRunnerFactory runWorkerFactory{{Constants::C_HASKELL_RUNCONFIG_ID}};
};

HaskellPlugin::~HaskellPlugin()
{
    delete d;
}

static void registerGhciAction()
{
    QAction *action = new QAction(Tr::tr("Run GHCi"), HaskellManager::instance());
    Core::ActionManager::registerAction(action, Constants::A_RUN_GHCI);
    QObject::connect(action, &QAction::triggered, HaskellManager::instance(), [] {
        if (Core::IDocument *doc = Core::EditorManager::currentDocument())
            HaskellManager::openGhci(doc->filePath());
    });
}

bool HaskellPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    d = new HaskellPluginPrivate;

    ProjectExplorer::ProjectManager::registerProjectType<HaskellProject>(
        Constants::C_HASKELL_PROJECT_MIMETYPE);
    TextEditor::SnippetProvider::registerGroup(Constants::C_HASKELLSNIPPETSGROUP_ID,
                                               Tr::tr("Haskell", "SnippetProvider"));

    connect(Core::ICore::instance(), &Core::ICore::saveSettingsRequested, this, [] {
        HaskellManager::writeSettings(Core::ICore::settings());
    });

    registerGhciAction();

    HaskellManager::readSettings(Core::ICore::settings());

    ProjectExplorer::JsonWizardFactory::addWizardPath(":/haskell/share/wizards/");
    return true;
}

} // namespace Internal
} // namespace Haskell
