// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "collectioneditorutils.h"

#include "collectiondatatypemodel.h"
#include "model.h"
#include "nodemetainfo.h"
#include "propertymetainfo.h"

#include <coreplugin/documentmanager.h>
#include <coreplugin/icore.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectmanager.h>
#include <qmljs/qmljsmodelmanagerinterface.h>
#include <utils/qtcassert.h>

#include <variant>

#include <QColor>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QUrl>

using DataType = QmlDesigner::CollectionDetails::DataType;

namespace {

using CollectionDataVariant = std::variant<QString, bool, double, int, QUrl, QColor>;

inline bool operator<(const QColor &a, const QColor &b)
{
    return a.name(QColor::HexArgb) < b.name(QColor::HexArgb);
}

inline CollectionDataVariant valueToVariant(const QVariant &value, DataType type)
{
    switch (type) {
    case DataType::String:
        return value.toString();
    case DataType::Real:
        return value.toDouble();
    case DataType::Integer:
        return value.toInt();
    case DataType::Boolean:
        return value.toBool();
    case DataType::Color:
        return value.value<QColor>();
    case DataType::Image:
    case DataType::Url:
        return value.value<QUrl>();
    default:
        return false;
    }
}

struct LessThanVisitor
{
    template<typename T1, typename T2>
    bool operator()(const T1 &a, const T2 &b) const
    {
        return CollectionDataVariant(a).index() < CollectionDataVariant(b).index();
    }

    template<typename T>
    bool operator()(const T &a, const T &b) const
    {
        return a < b;
    }
};

Utils::FilePath findFile(const Utils::FilePath &path, const QString &fileName)
{
    QDirIterator it(path.toString(), QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QFileInfo file(it.next());
        if (file.isDir())
            continue;

        if (file.fileName() == fileName)
            return Utils::FilePath::fromFileInfo(file);
    }
    return {};
}

Utils::FilePath dataStoreDir()
{
    using Utils::FilePath;
    ProjectExplorer::Project *currentProject = ProjectExplorer::ProjectManager::startupProject();

    if (!currentProject)
        return {};

    FilePath oldImportDirectory = currentProject->projectDirectory().pathAppended(
        "imports/" + currentProject->displayName());
    if (oldImportDirectory.exists())
        return oldImportDirectory;

    return currentProject->projectDirectory().pathAppended(currentProject->displayName());
}

inline Utils::FilePath collectionPath(const QString &filePath)
{
    return dataStoreDir().pathAppended(filePath);
}

inline Utils::FilePath qmlDirFilePath()
{
    return collectionPath("qmldir");
}

} // namespace

namespace QmlDesigner::CollectionEditorUtils {

bool variantIslessThan(const QVariant &a, const QVariant &b, DataType type)
{
    return std::visit(LessThanVisitor{}, valueToVariant(a, type), valueToVariant(b, type));
}

QString getSourceCollectionType(const ModelNode &node)
{
    using namespace QmlDesigner;
    if (node.type() == CollectionEditorConstants::JSONCOLLECTIONMODEL_TYPENAME)
        return "json";

    return {};
}

Utils::FilePath dataStoreJsonFilePath()
{
    return collectionPath("models.json");
}

Utils::FilePath dataStoreQmlFilePath()
{
    return collectionPath("DataStore.qml");
}

bool canAcceptCollectionAsModel(const ModelNode &node)
{
    const NodeMetaInfo nodeMetaInfo = node.metaInfo();
    if (!nodeMetaInfo.isValid())
        return false;

    const PropertyMetaInfo modelProperty = nodeMetaInfo.property("model");
    if (!modelProperty.isValid())
        return false;

    return modelProperty.isWritable() && !modelProperty.isPrivate()
           && modelProperty.propertyType().isVariant();
}

bool hasTextRoleProperty(const ModelNode &node)
{
    const NodeMetaInfo nodeMetaInfo = node.metaInfo();
    if (!nodeMetaInfo.isValid())
        return false;

    const PropertyMetaInfo textRoleProperty = nodeMetaInfo.property("textRole");
    if (!textRoleProperty.isValid())
        return false;

    return textRoleProperty.isWritable() && !textRoleProperty.isPrivate()
           && textRoleProperty.propertyType().isString();
}

QString getSourceCollectionPath(const ModelNode &dataStoreNode)
{
    using Utils::FilePath;
    if (!dataStoreNode.isValid())
        return {};

    const FilePath expectedFile = dataStoreJsonFilePath();

    if (expectedFile.exists())
        return expectedFile.toFSPathString();

    return {};
}

bool isDataStoreNode(const ModelNode &dataStoreNode)
{
    using Utils::FilePath;

    if (!dataStoreNode.isValid())
        return false;

    const FilePath expectedFile = dataStoreQmlFilePath();

    if (!expectedFile.exists())
        return false;

    FilePath modelPath = FilePath::fromUserInput(dataStoreNode.model()->fileUrl().toLocalFile());

    return modelPath.isSameFile(expectedFile);
}

bool ensureDataStoreExists(bool &justCreated)
{
    using Utils::FilePath;
    using Utils::FileReader;
    using Utils::FileSaver;

    FilePath qmlDestinationPath = dataStoreQmlFilePath();
    justCreated = false;

    auto extractDependency = [&justCreated](const FilePath &filePath) -> bool {
        if (filePath.exists())
            return true;

        const QString templateFileName = filePath.fileName() + u".tpl";
        const FilePath templatePath = findFile(Core::ICore::resourcePath(), templateFileName);
        if (!templatePath.exists()) {
            qWarning() << Q_FUNC_INFO << __LINE__ << templateFileName << "does not exist";
            return false;
        }

        if (!filePath.parentDir().ensureWritableDir()) {
            qWarning() << Q_FUNC_INFO << __LINE__ << "Cannot create directory"
                       << filePath.parentDir();
            return false;
        }

        if (templatePath.copyFile(filePath)) {
            justCreated = true;
            return true;
        }

        qWarning() << Q_FUNC_INFO << __LINE__ << "Cannot copy" << templateFileName << "to" << filePath;
        return false;
    };

    if (!extractDependency(dataStoreJsonFilePath()))
        return false;

    if (!extractDependency(collectionPath("data.json")))
        return false;

    if (!extractDependency(collectionPath("JsonData.qml")))
        return false;

    if (!qmlDestinationPath.exists()) {
        if (qmlDestinationPath.ensureExistingFile()) {
            justCreated = true;
        } else {
            qWarning() << Q_FUNC_INFO << __LINE__ << "Can't create DataStore Qml File";
            return false;
        }
    }

    FilePath qmlDirPath = qmlDirFilePath();
    qmlDirPath.ensureExistingFile();

    FileReader qmlDirReader;
    if (!qmlDirReader.fetch(qmlDirPath)) {
        qWarning() << Q_FUNC_INFO << __LINE__ << "Can't read the content of the qmldir";
        return false;
    }

    QByteArray qmlDirContent = qmlDirReader.data();
    const QList<QByteArray> qmlDirLines = qmlDirContent.split('\n');
    for (const QByteArray &line : qmlDirLines) {
        if (line.startsWith("singleton DataStore "))
            return true;
    }

    if (!qmlDirContent.isEmpty() && qmlDirContent.back() != '\n')
        qmlDirContent.append("\n");
    qmlDirContent.append("singleton DataStore 1.0 DataStore.qml\n");

    FileSaver qmlDirSaver(qmlDirPath);
    qmlDirSaver.write(qmlDirContent);

    if (qmlDirSaver.finalize()) {
        justCreated = true;
        return true;
    }

    qWarning() << Q_FUNC_INFO << __LINE__ << "Can't write to the qmldir file";
    return false;
}

QJsonObject defaultCollection()
{
    QJsonObject collectionObject;

    QJsonArray columns;
    QJsonObject defaultColumn;
    defaultColumn.insert("name", "Column 1");
    defaultColumn.insert("type", CollectionDataTypeModel::dataTypeToString(DataType::String));
    columns.append(defaultColumn);

    QJsonArray collectionData;
    QJsonArray cellData;
    cellData.append(QString{});
    collectionData.append(cellData);

    collectionObject.insert("columns", columns);
    collectionObject.insert("data", collectionData);

    return collectionObject;
}

QJsonObject defaultColorCollection()
{
    using Utils::FilePath;
    using Utils::FileReader;
    const FilePath templatePath = findFile(Core::ICore::resourcePath(), "Colors.json.tpl");

    FileReader fileReader;
    if (!fileReader.fetch(templatePath)) {
        qWarning() << Q_FUNC_INFO << __LINE__ << "Can't read the content of the file" << templatePath;
        return {};
    }

    QJsonParseError parseError;
    const CollectionDetails collection = CollectionDetails::fromImportedJson(fileReader.data(),
                                                                             &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << Q_FUNC_INFO << __LINE__ << "Error in template file" << parseError.errorString();
        return {};
    }

    return collection.toLocalJson();
}

bool writeToJsonDocument(const Utils::FilePath &path, const QJsonDocument &document, QString *errorString)
{
    Core::FileChangeBlocker fileBlocker(path);
    Utils::FileSaver jsonFile(path);
    if (jsonFile.write(document.toJson()))
        jsonFile.finalize();
    if (errorString)
        *errorString = jsonFile.errorString();

    return !jsonFile.hasError();
}

} // namespace QmlDesigner::CollectionEditorUtils
