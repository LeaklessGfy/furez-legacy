#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editorwindow.h"
#include "showwindow.h"
#include "filecreatebox.h"

#include <QMessageBox>

#include "Fuzzer.h"
#include "Cmd.h"
#include "Logger.h"

#include <QStandardItemModel>
#include <QStandardItem>

#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(1, false);
    this->firstTime = true;

    QSettings settings(QString("config.ini"), QSettings::IniFormat);
    QString defaultUrl = settings.value("config/url", "").toString();

    this->ui->url->setText(defaultUrl);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::progressUpdate(qint64 readBytes, qint64 totalBytes) {
    this->ui->progressBar->setMaximum(totalBytes);
    this->ui->progressBar->setValue(readBytes);
}

QString MainWindow::getUrl()
{
    QString url = this->ui->url->text();

    return url;
}

QString MainWindow::getHeader()
{
    QString header = this->ui->header->text();

    if(header.isEmpty()) {
        header = "Nes";
    }

    return header;
}

QAbstractButton* MainWindow::getExecuter()
{
    QButtonGroup * executeCommand = new QButtonGroup(this);
    executeCommand->addButton(this->ui->shellExec);
    executeCommand->addButton(this->ui->exec);
    executeCommand->addButton(this->ui->passthru);

    QAbstractButton * radio = executeCommand->checkedButton();

    return radio;
}

QString MainWindow::getRootPath()
{
    QString rPath = this->ui->rootPath->text();

    if(rPath.isEmpty()) {
        rPath = "./";
    }

    return rPath;
}

QString MainWindow::getCacheDir()
{
    QString rCache = this->ui->cacheDir->text();

    if(rCache.isEmpty()) {
        rCache = "cache/";
    }

    return rCache;
}

bool MainWindow::checkGlobals()
{
    if(gUrl.isEmpty()) {
        QMessageBox alert;
        alert.setIcon(alert.Critical);
        alert.setText("You must fill in the url attribute");
        alert.exec();

        return false;
    }

    return true;
}

void MainWindow::on_save_clicked()
{
    gUrl = this->getUrl();
    gHeader = this->getHeader();
    gRootPath = this->getRootPath();
    gCacheDir = this->getCacheDir();
    //QAbstractButton * radio = executeCommand->checkedButton();

    if(this->checkGlobals() == false) {
        return;
    }

    if(firstTime) {
        fuzzer = new Fuzzer(this, gHeader, gUrl);
        cmd = new Cmd(this, fuzzer);
        uiService = new UiService(this);
        logger = new Logger(this, fuzzer, this->ui->loggerZone);

        this->firstTime = false;
    } else {
        qDebug() << "Update";
    }

    cmd->listDir(gRootPath);
    connect(cmd, SIGNAL(ls_done(std::vector<std::string>)), this, SLOT(save_finish(std::vector<std::string>)), Qt::UniqueConnection);
}

void MainWindow::save_finish(std::vector<std::string> response)
{
    if(response[0] == "ERROR") {
        this->progressUpdate(100, 100);
        return;
    }

    this->setFixedSize(1000,900);

    QStandardItemModel *model = uiService->handleTreeView(response);
    this->ui->treeView->setModel(model);
    this->ui->treeView->setColumnWidth(0, 400);

    this->ui->tabWidget->setTabEnabled(1, true);
    this->ui->tabWidget->setCurrentIndex(1);

    this->progressUpdate(100, 100);
    disconnect(cmd, SIGNAL(ls_done(std::vector<std::string>)), this, SLOT(save_finish(std::vector<std::string>)));
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QModelIndex newIndex = index;
    QStandardItemModel *rootPathModel = uiService->getRootPathModel();

    currentItem = rootPathModel->itemFromIndex(index);

    QString type = index.sibling(index.row(), 1).data().toString();

    if(currentItem->text() == "Folder" || currentItem->text() == "File") {
        newIndex = index.sibling(index.row(), 0);
        currentItem = rootPathModel->itemFromIndex(newIndex);
    }

    gContext = gRootPath + uiService->getContext(currentItem);
    gFileName = currentItem->text();

    if(!currentItem->hasChildren()) {
        if(type == "Folder") {
            cmd->listDir(gContext + gFileName);
            connect(cmd, SIGNAL(ls_done(std::vector<std::string>)), this, SLOT(treeView_finish(std::vector<std::string>)), Qt::UniqueConnection);
        }
    }

    if(type == "Folder") {
        this->ui->show->setEnabled(false);
        this->ui->edit->setEnabled(false);
        this->ui->download->setEnabled(false);
    } else {
        this->ui->show->setEnabled(true);
        this->ui->edit->setEnabled(true);
        this->ui->download->setEnabled(true);
    }

    if(!this->ui->treeView->isExpanded(newIndex)) {
        this->ui->treeView->expand(newIndex);
    } else {
        this->ui->treeView->collapse(newIndex);
    }
}

void MainWindow::treeView_finish(std::vector<std::string> responseArr)
{
    uiService->prepareBlock(responseArr, currentItem);
    this->progressUpdate(100, 100);
}

void MainWindow::on_show_clicked()
{
    cmd->getFileContent(gContext + gFileName);
    connect(cmd, SIGNAL(cat_done(QString)), this, SLOT(show_finish(QString)), Qt::UniqueConnection);
}

void MainWindow::show_finish(QString response)
{
    disconnect(cmd, SIGNAL(cat_done(QString)), this, SLOT(show_finish(QString)));

    ShowWindow *showWindow = new ShowWindow(this, currentItem->text(), response);
    showWindow->show();

    this->progressUpdate(100, 100);
}

void MainWindow::on_edit_clicked()
{
    //EditorWindow *editWindow = new EditorWindow(this, fuzzer, gFileName, gContext, gCacheDir);

    //downloader = new Downloader(this, gHeader, gUrl, gCacheDir, editWindow);
    //downloader->downloadFile(gContext, gFileName, true);
}

void MainWindow::on_generateVirus_clicked()
{
    FileCreateBox *fileCreateBox = new FileCreateBox(this);
    fileCreateBox->show();
}

void MainWindow::on_download_clicked()
{
    fuzzer->downloadFile(gContext, gFileName);
    connect(fuzzer, SIGNAL(finishRequest()), this, SLOT(download_finish()));
}

void MainWindow::download_finish()
{
    disconnect(fuzzer, SIGNAL(finishRequest()), this, SLOT(download_finish()));
    this->progressUpdate(100, 100);

    QString absolutePath = this->fuzzer->getAbsolutePath();
    QMessageBox msg;
    msg.setText("File download in: " + absolutePath);
    msg.exec();
}
