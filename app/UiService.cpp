#include "UiService.h"

#include <QTreeWidget>
#include <QStandardItem>

UiService::UiService(QObject *parent) : QObject(parent)
{
    this->parser = new Parser();
}

QString UiService::getContext(QStandardItem *itemPath)
{
    QStandardItem *sourcePath = itemPath->parent();
    QString context = "";

    while(sourcePath != 0) {
        context = sourcePath->text() + context;
        sourcePath = sourcePath->parent();
    }

    return context;
}

QStandardItemModel* UiService::getRootPathModel()
{
    return rootPathModel;
}

QStandardItemModel* UiService::handleTreeView(std::vector<std::string> response)
{
    rootPathModel = new QStandardItemModel();
    QStandardItem *pathItem = rootPathModel->invisibleRootItem();

    this->prepareBlock(response, pathItem);

    rootPathModel->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    rootPathModel->setHorizontalHeaderItem(1, new QStandardItem("Type"));

    return rootPathModel;
}

void UiService::prepareBlock(std::vector<std::string> response, QStandardItem *pathItem)
{
    QString folderName, fileName;

    this->createBlock(response, 0, "Folder", pathItem);

    if(1 < response.size()) {
        this->createBlock(response, 1, "File", pathItem);
    }
}

void UiService::createBlock(std::vector<std::string> response, int pos, QString type, QStandardItem *pathItem)
{
    bool cut = false;

    if(type == "File") {
        cut = true;
    }

    std::vector<std::string> elem = parser->parseSpace(response[pos], cut);

    for(auto& s: elem) {
        QList<QStandardItem *> rowItem = this->constructRowItem(s, type);
        pathItem->appendRow(rowItem);
    }
}

QList<QStandardItem *> UiService::constructRowItem(std::string elemName, QString type)
{
    QList<QStandardItem *> rowItem;
    QStandardItem *item = new QStandardItem(elemName.c_str());
    QStandardItem *typeItem = new QStandardItem(type);

    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
    typeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

    rowItem << item;
    rowItem << typeItem;

    return rowItem;
}
