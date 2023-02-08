// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "cpplocatorfilter_test.h"

#include "cppcurrentdocumentfilter.h"
#include "cpplocatorfilter.h"
#include "cppmodelmanager.h"
#include "cpptoolstestcase.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/locator/locatorfiltertest.h>
#include <coreplugin/testdatadir.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/environment.h>
#include <utils/fileutils.h>

#include <QDebug>
#include <QFileInfo>
#include <QtTest>

using namespace Core;
using namespace Core::Tests;
using namespace ExtensionSystem;
using namespace Utils;

namespace CppEditor::Internal {
namespace {

const bool debug = qtcEnvironmentVariable("QTC_DEBUG_CPPLOCATORFILTERTESTCASE") == "1";

QTC_DECLARE_MYTESTDATADIR("../../../tests/cpplocators/")

class CppLocatorFilterTestCase
    : public BasicLocatorFilterTest
    , public CppEditor::Tests::TestCase
{
public:
    CppLocatorFilterTestCase(ILocatorFilter *filter,
                             const QString &fileName,
                             const QString &searchText,
                             const ResultDataList &expectedResults)
        : BasicLocatorFilterTest(filter)
        , m_fileName(fileName)
    {
        QVERIFY(succeededSoFar());
        QVERIFY(!m_fileName.isEmpty());
        QVERIFY(garbageCollectGlobalSnapshot());

        ResultDataList results = ResultData::fromFilterEntryList(matchesFor(searchText));
        if (debug) {
            ResultData::printFilterEntries(expectedResults, "Expected:");
            ResultData::printFilterEntries(results, "Results:");
        }
        QVERIFY(!results.isEmpty());
        QCOMPARE(results, expectedResults);
    }

private:
    void doBeforeLocatorRun() override { QVERIFY(parseFiles(m_fileName)); }
    void doAfterLocatorRun() override { QVERIFY(garbageCollectGlobalSnapshot()); }

private:
    const QString m_fileName;
};

class CppCurrentDocumentFilterTestCase
    : public BasicLocatorFilterTest
    , public CppEditor::Tests::TestCase
{
public:
    CppCurrentDocumentFilterTestCase(const FilePath &filePath,
                                     const ResultDataList &expectedResults,
                                     const QString &searchText = QString())
        : BasicLocatorFilterTest(CppModelManager::instance()->currentDocumentFilter())
        , m_filePath(filePath)
    {
        QVERIFY(succeededSoFar());
        QVERIFY(!m_filePath.isEmpty());

        ResultDataList results = ResultData::fromFilterEntryList(matchesFor(searchText));
        if (debug) {
            ResultData::printFilterEntries(expectedResults, "Expected:");
            ResultData::printFilterEntries(results, "Results:");
        }
        QVERIFY(!results.isEmpty());
        QCOMPARE(results, expectedResults);
    }

private:
    void doBeforeLocatorRun() override
    {
        QVERIFY(DocumentModel::openedDocuments().isEmpty());
        QVERIFY(garbageCollectGlobalSnapshot());

        m_editor = EditorManager::openEditor(m_filePath);
        QVERIFY(m_editor);

        QVERIFY(waitForFileInGlobalSnapshot(m_filePath));
    }

    void doAfterLocatorRun() override
    {
        QVERIFY(closeEditorWithoutGarbageCollectorInvocation(m_editor));
        QCoreApplication::processEvents();
        QVERIFY(DocumentModel::openedDocuments().isEmpty());
        QVERIFY(garbageCollectGlobalSnapshot());
    }

private:
    IEditor *m_editor = nullptr;
    const FilePath m_filePath;
};

} // anonymous namespace

void LocatorFilterTest::testLocatorFilter()
{
    QFETCH(QString, testFile);
    QFETCH(ILocatorFilter *, filter);
    QFETCH(QString, searchText);
    QFETCH(ResultDataList, expectedResults);

    Tests::VerifyCleanCppModelManager verify;

    CppLocatorFilterTestCase(filter, testFile, searchText, expectedResults);
}

void LocatorFilterTest::testLocatorFilter_data()
{
    QTest::addColumn<QString>("testFile");
    QTest::addColumn<ILocatorFilter *>("filter");
    QTest::addColumn<QString>("searchText");
    QTest::addColumn<ResultDataList>("expectedResults");

    CppModelManager *cppModelManager = CppModelManager::instance();

    ILocatorFilter *cppFunctionsFilter = cppModelManager->functionsFilter();
    ILocatorFilter *cppClassesFilter = cppModelManager->classesFilter();
    ILocatorFilter *cppLocatorFilter = cppModelManager->locatorFilter();

    MyTestDataDir testDirectory("testdata_basic");
    QString testFile = testDirectory.file("file1.cpp");
    testFile[0] = testFile[0].toLower(); // Ensure Windows path sorts after scope names.
    const QString objTestFile = testDirectory.file("file1.mm");
    const QString testFileShort = FilePath::fromString(testFile).shortNativePath();
    const QString objTestFileShort = FilePath::fromString(objTestFile).shortNativePath();

    QTest::newRow("CppFunctionsFilter")
        << testFile
        << cppFunctionsFilter
        << "function"
        << ResultDataList{
               ResultData("functionDefinedInClass(bool, int)",
                          "<anonymous namespace>::MyClass (file1.cpp)"),
               ResultData("functionDefinedInClass(bool, int)", "MyClass (file1.cpp)"),
               ResultData("functionDefinedInClass(bool, int)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClass(char)",
                          "<anonymous namespace>::MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClass(char)", "MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClass(char)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClassAndNamespace(float)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("myFunction(bool, int)", "<anonymous namespace> (file1.cpp)"),
               ResultData("myFunction(bool, int)", "MyNamespace (file1.cpp)"),
               ResultData("myFunction(bool, int)", testFileShort)
           };

    QTest::newRow("CppFunctionsFilter-Sorting")
        << testFile
        << cppFunctionsFilter
        << "pos"
        << ResultDataList{
               ResultData("positiveNumber()", testFileShort),
               ResultData("somePositionWithin()", testFileShort),
               ResultData("pointOfService()", testFileShort),
               ResultData("matchArgument(Pos)", testFileShort)
           };

    QTest::newRow("CppFunctionsFilter-arguments")
        << testFile
        << cppFunctionsFilter
        << "function*bool"
        << ResultDataList{
               ResultData("functionDefinedInClass(bool, int)",
                          "<anonymous namespace>::MyClass (file1.cpp)"),
               ResultData("functionDefinedInClass(bool, int)",
                          "MyClass (file1.cpp)"),
               ResultData("functionDefinedInClass(bool, int)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("myFunction(bool, int)", "<anonymous namespace> (file1.cpp)"),
               ResultData("myFunction(bool, int)", "MyNamespace (file1.cpp)"),
               ResultData("myFunction(bool, int)", testFileShort)
           };

    QTest::newRow("CppFunctionsFilter-WithNamespacePrefix")
        << testFile
        << cppFunctionsFilter
        << "mynamespace::"
        << ResultDataList{
               ResultData("MyClass()", "MyNamespace::MyClass (file1.cpp)"),
               ResultData("functionDefinedInClass(bool, int)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClass(char)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClassAndNamespace(float)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("myFunction(bool, int)", "MyNamespace (file1.cpp)"),
           };

    QTest::newRow("CppFunctionsFilter-WithClassPrefix")
        << testFile
        << cppFunctionsFilter
        << "MyClass::func"
        << ResultDataList{
               ResultData("functionDefinedInClass(bool, int)",
                          "<anonymous namespace>::MyClass (file1.cpp)"),
               ResultData("functionDefinedInClass(bool, int)",
                          "MyClass (file1.cpp)"),
               ResultData("functionDefinedInClass(bool, int)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClass(char)",
                          "<anonymous namespace>::MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClass(char)",
                          "MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClass(char)",
                          "MyNamespace::MyClass (file1.cpp)"),
               ResultData("functionDefinedOutSideClassAndNamespace(float)",
                          "MyNamespace::MyClass (file1.cpp)"),
           };

    QTest::newRow("CppClassesFilter")
        << testFile
        << cppClassesFilter
        << "myclass"
        << ResultDataList{
               ResultData("MyClass", "<anonymous namespace>"),
               ResultData("MyClass", "MyNamespace"),
               ResultData("MyClass", testFileShort)
           };

    QTest::newRow("CppClassesFilter-WithNamespacePrefix")
        << testFile
        << cppClassesFilter
        << "mynamespace::"
        << ResultDataList{
               ResultData("MyClass", "MyNamespace")
           };

    // all symbols in the left column are expected to be fully qualified.
    QTest::newRow("CppLocatorFilter-filtered")
        << testFile
        << cppLocatorFilter
        << "my"
        << ResultDataList{
               ResultData("MyClass", testFileShort),
               ResultData("MyClass::MyClass", "()"),
               ResultData("MyClass::functionDefinedOutSideClass", "(char)"),
               ResultData("MyEnum", testFileShort),
               ResultData("MyNamespace::MyClass", testFileShort),
               ResultData("MyNamespace::MyClass::MyClass", "()"),
               ResultData("MyNamespace::MyClass::functionDefinedOutSideClass",
                          "(char)"),
               ResultData("MyNamespace::MyClass::functionDefinedOutSideClassAndNamespace",
                          "(float)"),
               ResultData("MyNamespace::MyEnum", testFileShort),
               ResultData("MyNamespace::myFunction", "(bool, int)"),
               ResultData("myFunction", "(bool, int)"),
               ResultData("<anonymous namespace>::MyClass", testFileShort),
               ResultData("<anonymous namespace>::MyClass::MyClass", "()"),
               ResultData("<anonymous namespace>::MyClass::functionDefinedOutSideClass",
                          "(char)"),
               ResultData("<anonymous namespace>::MyEnum", testFileShort),
               ResultData("<anonymous namespace>::myFunction", "(bool, int)")
           };

    QTest::newRow("CppClassesFilter-ObjC")
        << objTestFile
        << cppClassesFilter
        << "M"
        << ResultDataList{
               ResultData("MyClass", objTestFileShort),
               ResultData("MyClass", objTestFileShort),
               ResultData("MyClass", objTestFileShort),
               ResultData("MyProtocol", objTestFileShort),
           };

    QTest::newRow("CppFunctionsFilter-ObjC")
        << objTestFile
        << cppFunctionsFilter
        << "M"
        << ResultDataList{
               ResultData("anotherMethod", "MyClass (file1.mm)"),
               ResultData("anotherMethod:", "MyClass (file1.mm)"),
               ResultData("someMethod", "MyClass (file1.mm)")
           };
}

void LocatorFilterTest::testCurrentDocumentFilter()
{
    MyTestDataDir testDirectory("testdata_basic");
    const FilePath testFile = testDirectory.filePath("file1.cpp");

    auto expectedResults = ResultDataList{
        ResultData("int myVariable", ""),
        ResultData("myFunction(bool, int)", ""),
        ResultData("Pos", ""),
        ResultData("somePositionWithin()", ""),
        ResultData("pointOfService()", ""),
        ResultData("matchArgument(Pos)", ""),
        ResultData("positiveNumber()", ""),
        ResultData("MyEnum", ""),
        ResultData("int V1", "MyEnum"),
        ResultData("int V2", "MyEnum"),
        ResultData("MyClass", ""),
        ResultData("MyClass()", "MyClass"),
        ResultData("functionDeclaredOnly()", "MyClass"),
        ResultData("functionDefinedInClass(bool, int)", "MyClass"),
        ResultData("functionDefinedOutSideClass(char)", "MyClass"),
        ResultData("int myVariable", "MyNamespace"),
        ResultData("myFunction(bool, int)", "MyNamespace"),
        ResultData("MyEnum", "MyNamespace"),
        ResultData("int V1", "MyNamespace::MyEnum"),
        ResultData("int V2", "MyNamespace::MyEnum"),
        ResultData("MyClass", "MyNamespace"),
        ResultData("MyClass()", "MyNamespace::MyClass"),
        ResultData("functionDeclaredOnly()", "MyNamespace::MyClass"),
        ResultData("functionDefinedInClass(bool, int)", "MyNamespace::MyClass"),
        ResultData("functionDefinedOutSideClass(char)", "MyNamespace::MyClass"),
        ResultData("functionDefinedOutSideClassAndNamespace(float)",
                   "MyNamespace::MyClass"),
        ResultData("int myVariable", "<anonymous namespace>"),
        ResultData("myFunction(bool, int)", "<anonymous namespace>"),
        ResultData("MyEnum", "<anonymous namespace>"),
        ResultData("int V1", "<anonymous namespace>::MyEnum"),
        ResultData("int V2", "<anonymous namespace>::MyEnum"),
        ResultData("MyClass", "<anonymous namespace>"),
        ResultData("MyClass()", "<anonymous namespace>::MyClass"),
        ResultData("functionDeclaredOnly()", "<anonymous namespace>::MyClass"),
        ResultData("functionDefinedInClass(bool, int)", "<anonymous namespace>::MyClass"),
        ResultData("functionDefinedOutSideClass(char)", "<anonymous namespace>::MyClass"),
        ResultData("main()", ""),
    };

    CppCurrentDocumentFilterTestCase(testFile, expectedResults);
}

void LocatorFilterTest::testCurrentDocumentHighlighting()
{
    MyTestDataDir testDirectory("testdata_basic");
    const FilePath testFile = testDirectory.filePath("file1.cpp");

    const QString searchText = "pos";
    const ResultDataList expectedResults{
        ResultData("Pos", "",
                   "~~~"),
        ResultData("pointOfService()", "",
                   "~    ~ ~        "),
        ResultData("positiveNumber()", "",
                   "~~~             "),
        ResultData("somePositionWithin()", "",
                   "    ~~~             "),
        ResultData("matchArgument(Pos)", "",
                   "              ~~~ ")
       };

    Tests::VerifyCleanCppModelManager verify;

    CppCurrentDocumentFilterTestCase(testFile, expectedResults, searchText);
}

void LocatorFilterTest::testFunctionsFilterHighlighting()
{
    MyTestDataDir testDirectory("testdata_basic");
    const QString testFile = testDirectory.file("file1.cpp");
    const QString testFileShort = FilePath::fromString(testFile).shortNativePath();

    const QString searchText = "pos";
    const ResultDataList expectedResults{
        ResultData("positiveNumber()", testFileShort,
                   "~~~             "),
        ResultData("somePositionWithin()", testFileShort,
                   "    ~~~             "),
        ResultData("pointOfService()", testFileShort,
                   "~    ~ ~        "),
        ResultData("matchArgument(Pos)", testFileShort,
                   "              ~~~ ")
       };

    CppModelManager *cppModelManager = CppModelManager::instance();
    ILocatorFilter *filter = cppModelManager->functionsFilter();

    Tests::VerifyCleanCppModelManager verify;

    CppLocatorFilterTestCase(filter, testFile, searchText, expectedResults);
}

} // namespace CppEditor::Internal
