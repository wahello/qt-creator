// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "findincurrentfile.h"

#include "textdocument.h"
#include "texteditortr.h"

#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/editormanager/editormanager.h>

#include <QSettings>

using namespace Utils;

namespace TextEditor::Internal {

FindInCurrentFile::FindInCurrentFile()
{
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged,
            this, &FindInCurrentFile::handleFileChange);
    handleFileChange(Core::EditorManager::currentEditor());
}

QString FindInCurrentFile::id() const
{
    return QLatin1String("Current File");
}

QString FindInCurrentFile::displayName() const
{
    return Tr::tr("Current File");
}

FileContainerProvider FindInCurrentFile::fileContainerProvider() const
{
    return [fileName = m_currentDocument->filePath()] {
        const QMap<FilePath, QTextCodec *> encodings = TextDocument::openedTextDocumentEncodings();
        QTextCodec *codec = encodings.value(fileName);
        if (!codec)
            codec = Core::EditorManager::defaultTextCodec();
        return FileListContainer({fileName}, {codec});
    };
}

QString FindInCurrentFile::label() const
{
    return Tr::tr("File \"%1\":").arg(m_currentDocument->filePath().fileName());
}

QString FindInCurrentFile::toolTip() const
{
    // %2 is filled by BaseFileFind::runNewSearch
    return Tr::tr("File path: %1\n%2").arg(m_currentDocument->filePath().toUserOutput());
}

bool FindInCurrentFile::isEnabled() const
{
    return m_currentDocument && !m_currentDocument->filePath().isEmpty();
}

void FindInCurrentFile::handleFileChange(Core::IEditor *editor)
{
    if (!editor) {
        m_currentDocument = nullptr;
        emit enabledChanged(isEnabled());
    } else {
        Core::IDocument *document = editor->document();
        if (document != m_currentDocument) {
            m_currentDocument = document;
            emit enabledChanged(isEnabled());
        }
    }
}


void FindInCurrentFile::writeSettings(QSettings *settings)
{
    settings->beginGroup(QLatin1String("FindInCurrentFile"));
    writeCommonSettings(settings);
    settings->endGroup();
}

void FindInCurrentFile::readSettings(QSettings *settings)
{
    settings->beginGroup(QLatin1String("FindInCurrentFile"));
    readCommonSettings(settings, "*", "");
    settings->endGroup();
}

} // TextEditor::Internal
