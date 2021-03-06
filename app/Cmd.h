#ifndef CMD_H
#define CMD_H

#include "Fuzzer.h"
#include <QObject>

class Cmd  : public QObject
{
    Q_OBJECT
private:
    Fuzzer *fuzzer;
    QString context;
    QString executer;
    bool isWindows;

    QString normalizeRequest(QString = "", QString = "r", bool = true);

public slots:
    void ls_handle();
    void cat_handle();

signals:
    void ls_done(std::vector<std::string>);
    void cat_done(QString);

public:
    explicit Cmd(QObject *parent = 0, Fuzzer *f = 0, QString = "shell_exec", bool = false);

    void listDir(QString);
    void getFileContent(QString);


    void changeDir(QString);
    QString vim(QString,QString);
    QString raw(QString);

    //GETTERS
    QString getContext();

    //SETTERS
    void setContext(QString);
};

#endif // CMD_H
