#ifndef UISERVICE_H
#define UISERVICE_H

#include <QObject>
#include <QTreeWidget>
#include <QStandardItem>

#include "Parser.h"

class UiService : public QObject
{
    Q_OBJECT
public:
    explicit UiService(QObject *parent = 0);
    QString getContext(QStandardItem *);
    QStandardItemModel* getRootPathModel();

    QStandardItemModel* handleTreeView(std::vector<std::string>);
    void prepareBlock(std::vector<std::string>, QStandardItem *);
    void createBlock(std::vector<std::string>, int, QString, QStandardItem *);
    QList<QStandardItem *> constructRowItem(std::string, QString);

private:
    QStandardItemModel *rootPathModel;
    Parser *parser;

signals:

public slots:
};

#endif // UISERVICE_H
