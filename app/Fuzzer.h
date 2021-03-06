#ifndef FUZZER_H
#define FUZZER_H

#include <QObject>
#include <QNetworkReply>
#include <QProgressBar>
#include <QFile>

class Fuzzer : public QObject
{
    Q_OBJECT
public:
    explicit Fuzzer(QObject *parent = 0, QString h = "Nes", QString u = "http://localhost/hack/perl.php");
    void sendRequest(QString, bool = true);
    void downloadFile(QString, QString);
    QString uploadFile(QString, QString, QString);
    QString getResponse();
    QString getRequest();
    QString getAbsolutePath();

signals:
    void finishRequest();

private:
    QString normalizeRequest(QString);
    QByteArray buildUploadString(QString, QString);

    QString header;
    QString url;
    QString response;
    QString request;
    QNetworkAccessManager *gManager;
    bool inHeader;
    bool isDownloadRequest;
    QString absolutePath;
    QString downloadDir;
    QFile *file;

private slots:
    void handleReturn();
    void onReadyRead();
};

#endif // FUZZER_H
