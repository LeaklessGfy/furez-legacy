#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QStringList>

#include "editorwindow.h"

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0, QString h = 0, QString u = 0, QString d = 0, EditorWindow *e = 0);
    virtual ~Downloader();
    QString downloadFile(QString, QString, bool);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile *file;
    QString header;
    QString url;
    QString downloadDir;
    QString absolutePath;
    EditorWindow *editorWindow;

    void hydrateContent();

private slots:
    void onFinished(QNetworkReply*);
    void onReadyRead();
    void onReplyFinished();
};

#endif // DOWNLOADER_H
