// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0
#include <utils/futuresynchronizer.h>
    std::array<QString, SideCount> text{};
    DiffFileInfoArray fileInfo{};
        if (reloadInfo.text[LeftSide] == reloadInfo.text[RightSide])
                        differ.diff(reloadInfo.text[LeftSide], reloadInfo.text[RightSide]));
        fileData.fileInfo = reloadInfo.fileInfo;
        m_futureWatcher.setFuture({});
        reloadInput.text = {leftText, rightText};
        reloadInput.fileInfo = {DiffFileInfo(m_fileName, tr("Saved")),
                                DiffFileInfo(m_fileName, tr("Modified"))};
        reloadInput.fileInfo[RightSide].patchBehaviour = DiffFileInfo::PatchEditor;
            reloadInput.text = {leftText, rightText};
            reloadInput.fileInfo = {DiffFileInfo(fileName, tr("Saved")),
                                    DiffFileInfo(fileName, tr("Modified"))};
            reloadInput.fileInfo[RightSide].patchBehaviour = DiffFileInfo::PatchEditor;
            reloadInput.text = {leftText, rightText};
            reloadInput.fileInfo = {DiffFileInfo(fileName, tr("Saved")),
                                    DiffFileInfo(fileName, tr("Modified"))};
            reloadInput.fileInfo[RightSide].patchBehaviour = DiffFileInfo::PatchEditor;
    reloadInput.text = {leftText, rightText};
    reloadInput.fileInfo[LeftSide].fileName = m_leftFileName;
    reloadInput.fileInfo[RightSide].fileName = m_rightFileName;
    DiffEditorFactory m_editorFactory;
    DiffEditorServiceImpl m_service;
    FutureSynchronizer m_futureSynchronizer;
    m_futureSynchronizer.setCancelOnWait(true);
static DiffEditorPlugin *s_instance = nullptr;

DiffEditorPlugin::DiffEditorPlugin()
{
    s_instance = this;
}

    s_instance = nullptr;
void DiffEditorPlugin::addFuture(const QFuture<void> &future)
{
    QTC_ASSERT(s_instance, return);
    s_instance->d->m_futureSynchronizer.addFuture(future);
}


        QCOMPARE(resultFileData.fileInfo[LeftSide].fileName, fileName);
        QCOMPARE(resultFileData.fileInfo[RightSide].fileName, fileName);
            QCOMPARE(resultChunkData.startingLineNumber[LeftSide], sourceChunk.startingLineNumber[LeftSide]);
            QCOMPARE(resultChunkData.startingLineNumber[RightSide], sourceChunk.startingLineNumber[RightSide]);
                QCOMPARE(resultRowData.line[LeftSide].text, sourceRowData.line[LeftSide].text);
                QCOMPARE(resultRowData.line[LeftSide].textLineType, sourceRowData.line[LeftSide].textLineType);
                QCOMPARE(resultRowData.line[RightSide].text, sourceRowData.line[RightSide].text);
                QCOMPARE(resultRowData.line[RightSide].textLineType, sourceRowData.line[RightSide].textLineType);
    QTest::addColumn<QList<FileData>>("fileDataList");
    fileData1.fileInfo = {DiffFileInfo("src/plugins/diffeditor/diffeditor.cpp", "eab9e9b"),
                          DiffFileInfo("src/plugins/diffeditor/diffeditor.cpp", "082c135")};
    chunkData1.startingLineNumber = {186, 186};
    fileData2.fileInfo = {DiffFileInfo(_("src/plugins/diffeditor/diffutils.cpp"), _("2f641c9")),
                          DiffFileInfo(_("src/plugins/diffeditor/diffutils.cpp"), _("f8ff795"))};
    chunkData2.startingLineNumber = {463, 463};
    fileData3.fileInfo = {DiffFileInfo("new", "0000000"), DiffFileInfo("new", "257cc56")};
    chunkData3.startingLineNumber = {-1, 0};
    fileData4.fileInfo = {DiffFileInfo("deleted", "257cc56"), DiffFileInfo("deleted", "0000000")};
    chunkData4.startingLineNumber = {0, -1};
    fileData5.fileInfo = {DiffFileInfo("empty", "0000000"), DiffFileInfo("empty", "e69de29")};
    fileData6.fileInfo = {DiffFileInfo("empty", "e69de29"), DiffFileInfo("empty", "0000000")};
    fileData7.fileInfo = {DiffFileInfo("file a.txt", "1234567"),
                          DiffFileInfo("file b.txt", "9876543")};
    chunkData7.startingLineNumber = {19, 19};
    fileData8.fileInfo = {DiffFileInfo("file a.txt"), DiffFileInfo("file b.txt")};
    fileData9.fileInfo = {DiffFileInfo("file.txt", "1234567"), DiffFileInfo("file.txt", "9876543")};
    fileData1.fileInfo = {DiffFileInfo("file foo.txt", "1234567"),
                          DiffFileInfo("file foo.txt", "9876543")};
    chunkData1.startingLineNumber = {49, 49};
    fileData1.fileInfo = {DiffFileInfo("file foo.txt", "1234567"),
                          DiffFileInfo("file foo.txt", "9876543")};
    chunkData1.startingLineNumber = {0, 0};
    fileData1.fileInfo = {DiffFileInfo("difftest.txt", "1234567"),
                          DiffFileInfo("difftest.txt", "9876543")};
    chunkData1.startingLineNumber = {1, 1};
    chunkData2.startingLineNumber = {8, 8};
    fileData1.fileInfo = {DiffFileInfo("file foo.txt", "1234567"),
                          DiffFileInfo("file foo.txt", "9876543")};
    chunkData1.startingLineNumber = {0, 0};
    fileData1 = {};
    fileData1.fileInfo = {DiffFileInfo("src/plugins/texteditor/basetextdocument.h"),
                          DiffFileInfo("src/plugins/texteditor/textdocument.h")};
    fileData2 = {};
    fileData2.fileInfo = {DiffFileInfo("src/plugins/texteditor/basetextdocumentlayout.cpp", "0121933"),
                          DiffFileInfo("src/plugins/texteditor/textdocumentlayout.cpp", "01cc3a0")};
    chunkData2.startingLineNumber = {1, 1};
    fileData1 = {};
    fileData1.fileInfo = {DiffFileInfo("src/shared/qbs"), DiffFileInfo("src/shared/qbs")};
    chunkData1.startingLineNumber = {0, 0};
    fileData1 = {};
    fileData1.fileInfo = {DiffFileInfo("demos/arthurplugin/arthurplugin.pro", "0000000"),
                          DiffFileInfo("demos/arthurplugin/arthurplugin.pro", "c5132b4")};
    chunkData1 = {};
    chunkData1.startingLineNumber = {-1, 0};
    fileData2 = {};
    fileData2.fileInfo = {DiffFileInfo("demos/arthurplugin/bg1.jpg", "0000000"),
                          DiffFileInfo("demos/arthurplugin/bg1.jpg", "dfc7cee")};
    fileData3 = {};
    fileData3.fileInfo = {DiffFileInfo("demos/arthurplugin/flower.jpg", "0000000"),
                          DiffFileInfo("demos/arthurplugin/flower.jpg", "f8e022c")};
    fileData1 = {};
    fileData1.fileInfo = {DiffFileInfo("script.sh"), DiffFileInfo("script.sh")};
    fileData1 = {};
    fileData1.fileInfo = {DiffFileInfo("old.sh"), DiffFileInfo("new.sh")};
    fileData1 = {};
    fileData1.fileInfo = {DiffFileInfo("src/plugins/subversion/subversioneditor.cpp"),
                          DiffFileInfo("src/plugins/subversion/subversioneditor.cpp")};
    chunkData1 = {};
    chunkData1.startingLineNumber = {-1, 124};
    fileData1 = {};
    fileData1.fileInfo = {DiffFileInfo("src/plugins/subversion/subversioneditor.cpp"),
                          DiffFileInfo("src/plugins/subversion/subversioneditor.cpp")};
    chunkData1 = {};
    chunkData1.startingLineNumber = {0, -1};
    fileData1 = {};
    fileData1.fileInfo = {DiffFileInfo("src/plugins/subversion/subversioneditor.cpp"),
                          DiffFileInfo("src/plugins/subversion/subversioneditor.cpp")};
    chunkData1 = {};
    chunkData1.startingLineNumber = {119, 119};
        QCOMPARE(resultFileData.fileInfo[LeftSide].fileName, origFileData.fileInfo[LeftSide].fileName);
        QCOMPARE(resultFileData.fileInfo[LeftSide].typeInfo, origFileData.fileInfo[LeftSide].typeInfo);
        QCOMPARE(resultFileData.fileInfo[RightSide].fileName, origFileData.fileInfo[RightSide].fileName);
        QCOMPARE(resultFileData.fileInfo[RightSide].typeInfo, origFileData.fileInfo[RightSide].typeInfo);
            QCOMPARE(resultChunkData.startingLineNumber[LeftSide], origChunkData.startingLineNumber[LeftSide]);
            QCOMPARE(resultChunkData.startingLineNumber[RightSide], origChunkData.startingLineNumber[RightSide]);
                QCOMPARE(resultRowData.line[LeftSide].text, origRowData.line[LeftSide].text);
                QCOMPARE(resultRowData.line[LeftSide].textLineType, origRowData.line[LeftSide].textLineType);
                QCOMPARE(resultRowData.line[RightSide].text, origRowData.line[RightSide].text);
                QCOMPARE(resultRowData.line[RightSide].textLineType, origRowData.line[RightSide].textLineType);
    QTest::addColumn<PatchAction>("patchAction");
    auto createChunk = [] {
        chunk.startingLineNumber = {49, 49};
        row.line[LeftSide].text = left;
        row.line[LeftSide].textLineType = left.isEmpty() ? TextLineData::Separator : TextLineData::TextLine;
        row.line[RightSide].text = right;
        row.line[RightSide].textLineType = right.isEmpty() ? TextLineData::Separator : TextLineData::TextLine;
    QTest::newRow("one added") << chunk << rows << ChunkSelection() << PatchAction::Apply;
    QTest::newRow("one removed") << chunk << rows << ChunkSelection() << PatchAction::Apply;
    QTest::newRow("stage selected added") << chunk << rows << ChunkSelection({2, 3}, {2, 3})
                                          << PatchAction::Apply;
    QTest::newRow("stage selected added keep changed") << chunk << rows << ChunkSelection({1}, {1})
                                                       << PatchAction::Apply;
    QTest::newRow("stage selected removed") << chunk << rows << ChunkSelection({2, 3}, {2, 3})
                                            << PatchAction::Apply;
    QTest::newRow("stage selected added/removed") << chunk << rows << ChunkSelection({2, 3}, {2, 3})
                                                  << PatchAction::Apply;
    QTest::newRow("stage modified row") << chunk << rows << ChunkSelection({1}, {1})
                                        << PatchAction::Apply;
    QTest::newRow("stage modified and unmodified rows") << chunk << rows
                  << ChunkSelection({0, 1, 2}, {0, 1, 2}) << PatchAction::Apply;
    QTest::newRow("stage unmodified left rows") << chunk << rows << ChunkSelection({0, 1, 2}, {1})
                                                << PatchAction::Apply;
    QTest::newRow("stage unmodified right rows") << chunk << rows << ChunkSelection({1}, {0, 1, 2})
                                                 << PatchAction::Apply;
    QTest::newRow("stage left only") << chunk << rows << ChunkSelection({1}, {})
                                     << PatchAction::Apply;
    QTest::newRow("stage right only") << chunk << rows << ChunkSelection({}, {1})
                                      << PatchAction::Apply;
    QTest::newRow("stage modified row and revert") << chunk << rows << ChunkSelection({1}, {1})
                                                   << PatchAction::Revert;
    QTest::newRow("stage left only and revert") << chunk << rows << ChunkSelection({1}, {})
                                                << PatchAction::Revert;
    QTest::newRow("stage right only and revert") << chunk << rows << ChunkSelection({}, {1})
                                                 << PatchAction::Revert;
    QFETCH(PatchAction, patchAction);
    const ChunkData result = DiffEditorDocument::filterChunk(chunk, selection, patchAction);
        QCOMPARE(result.rows.at(i).line[LeftSide].text, rows.at(i).first);
        QCOMPARE(result.rows.at(i).line[RightSide].text, rows.at(i).second);