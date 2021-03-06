#include <QMessageBox>

#include <QFile>
#include <QProcess>

#include "Cmd.h"

Cmd::Cmd(QObject *parent, Fuzzer *f, QString executer, bool isWindows) : QObject(parent)
{
    this->fuzzer = f;
    this->executer = executer;
    this->isWindows = isWindows;
}

QString Cmd::normalizeRequest(QString request, QString r, bool system)
{
    QString contexter, newRequest;

    if(!this->context.isEmpty()) {
        contexter = "cd " + this->context + " && ";
    }

    if(system) {
        //system()
        if(this->executer == "system") {
            newRequest = "ob_start();";
            newRequest += this->executer + "('" + contexter + request + "');";
            newRequest += "$" + r.toLatin1() + '=';
            newRequest += "ob_get_contents();ob_end_clean();";
        }

        //shell_exec()
        if(this->executer == "shell_exec") {
            newRequest = "$" + r.toLatin1();
            newRequest += '=';
            newRequest += this->executer + "('" + contexter + request + "');";
        }
    }

    return newRequest;
}

void Cmd::listDir(QString dir)
{
    QString ls, lsContext, lsFolder, lsFile;

    if(!dir.isEmpty()) {
       lsContext = "cd " + dir + " && ";
    }

    lsFolder = lsContext + "ls -d */";
    lsFile = lsContext + "find . -maxdepth 1 -type f"; //ls -p | grep -v /

    lsFolder = this->normalizeRequest(lsFolder, "o");
    lsFile = this->normalizeRequest(lsFile, "f");

    ls = lsFolder + lsFile + "$r=$o . \"|||\" . $f;";

    this->fuzzer->sendRequest(ls);
    connect(this->fuzzer, SIGNAL(finishRequest()), this, SLOT(ls_handle()), Qt::UniqueConnection);
}

void Cmd::ls_handle()
{
    QString response = this->fuzzer->getResponse();

    std::vector<std::string> arr(2);
    if(!response.isEmpty()) {
        QStringList list = response.split("|||");
        arr[0] = list.at(0).toStdString();

        if(list.size() > 1) {
            arr[1] = list.at(1).toStdString();
        }
    }

    emit ls_done(arr);
    disconnect(this->fuzzer, SIGNAL(finishRequest()), this, SLOT(ls_handle()));
}

void Cmd::getFileContent(QString file)
{
    QString cat;

    if (file.isEmpty()) {
        return;
    }

    cat = "cat " + file;
    cat = this->normalizeRequest(cat);

    this->fuzzer->sendRequest(cat, false);
    connect(this->fuzzer, SIGNAL(finishRequest()), this, SLOT(cat_handle()));
}

void Cmd::cat_handle()
{
    QString response = this->fuzzer->getResponse();
    response.replace("+", " ");

    emit cat_done(response);
    disconnect(this->fuzzer, SIGNAL(finishRequest()), this, SLOT(cat_handle()));
}

QString Cmd::vim(QString fileName, QString fileContent)
{
    QString request, response;

    request = "file_put_contents('"+fileName+"', '" + fileContent + "');$r='Success!';";

    request = this->normalizeRequest(request, false);

    return response;
}
