// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0+ OR GPL-3.0 WITH Qt-GPL-exception-1.0
static QList<RowData> readLines(QStringView patch, bool lastChunk, bool *lastChunkAtTheEndOfFile, bool *ok)
    const QVector<QStringView> lines = patch.split(newLine);
        QStringView line = lines.at(i);
static QStringView readLine(QStringView text, QStringView *remainingText, bool *hasNewLine)
            *remainingText = QStringView();
static bool detectChunkData(QStringView chunkDiff, ChunkData *chunkData, QStringView *remainingPatch)
    const QStringView chunkLine = readLine(chunkDiff, remainingPatch, &hasNewLine);
    QStringView leftPos = chunkLine.mid(leftPosStart, leftPosLength);
    QStringView rightPos = chunkLine.mid(rightPosStart, rightPosLength);
    const QStringView optionalHint = chunkLine.mid(optionalHintStart, optionalHintLength);
static QList<ChunkData> readChunks(QStringView patch, bool *lastChunkAtTheEndOfFile, bool *ok)
        const QStringView chunkDiff = patch.mid(chunkStart, chunkEnd - chunkStart);
        QStringView lines;
static FileData readDiffHeaderAndChunks(QStringView headerAndChunks, bool *ok)
    QStringView patch = headerAndChunks;
static QList<FileData> readDiffPatch(QStringView patch, bool *ok, QFutureInterfaceBase *jobController)
                QStringView headerAndChunks = patch.mid(lastPos, pos - lastPos);
            QStringView headerAndChunks = patch.mid(lastPos, patch.size() - lastPos - 1);
static bool detectIndexAndBinary(QStringView patch, FileData *fileData, QStringView *remainingPatch)
    QStringView afterNextLine;
    const QStringView nextLine = readLine(patch, &afterNextLine, &hasNewLine);
        const QStringView indices = nextLine.mid(indexHeader.size());
        *remainingPatch = QStringView();
    QStringView afterMinuses;
    const QStringView minuses = readLine(*remainingPatch, &afterMinuses, &hasNewLine);
    QStringView afterPluses;
    const QStringView pluses = readLine(afterMinuses, &afterPluses, &hasNewLine);
static bool extractCommonFileName(QStringView fileNames, QStringView *fileName)
    const QStringView leftFileName = fileNames.mid(2, fileNameSize - 2);
    const QStringView rightFileName = fileNames.mid(fileNameSize + 3, fileNameSize - 2);
static bool detectFileData(QStringView patch, FileData *fileData, QStringView *remainingPatch)
    QStringView afterDiffGit;
    const QStringView diffGit = readLine(patch, &afterDiffGit, &hasNewLine);
    const QStringView fileNames = diffGit.mid(gitHeader.size());
    QStringView commonFileName;
        QStringView afterSecondLine;
        const QStringView secondLine = readLine(afterDiffGit, &afterSecondLine, &hasNewLine);
            QStringView afterThirdLine;
        QStringView afterModeOrSimilarity;
        QStringView afterSimilarity;
        const QStringView secondLine = readLine(afterDiffGit, &afterModeOrSimilarity, &hasNewLine);
        QStringView afterCopyRenameFrom;
        const QStringView copyRenameFrom = readLine(afterSimilarity, &afterCopyRenameFrom, &hasNewLine);
        QStringView afterCopyRenameTo;
        const QStringView copyRenameTo = readLine(afterCopyRenameFrom, &afterCopyRenameTo, &hasNewLine);
static QList<FileData> readGitPatch(QStringView patch, bool *ok, QFutureInterfaceBase *jobController)
        QStringView patch;
        const QStringView fileDiff = patch.mid(diffStart, diffEnd - diffStart);
        QStringView remainingFileDiff;
    QStringView croppedPatch = QStringView(patch);