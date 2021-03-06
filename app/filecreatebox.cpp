#include "filecreatebox.h"
#include "ui_filecreatebox.h"

#include "QFileSystemModel"

FileCreateBox::FileCreateBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileCreateBox)
{
    ui->setupUi(this);

    dirModel = new QFileSystemModel(this);
    dirModel->setRootPath("C:/");

    this->ui->treeView->setModel(dirModel);
    this->ui->treeView->setRootIndex(dirModel->setRootPath("/"));
}

FileCreateBox::~FileCreateBox()
{
    delete ui;
}

void FileCreateBox::on_buttonBox_accepted()
{
    QModelIndexList indexes = this->ui->treeView->selectionModel()->selection().indexes();

    if(indexes.count() > 0) {
    }

    QString absolute = dirModel->filePath(indexes.at(0));

    QFile *fileOpen = new QFile;
    fileOpen->setFileName(absolute + "furez.php");
    fileOpen->open(QIODevice::WriteOnly);

    if(fileOpen->isOpen()) {
        QString fullText = ""; //Put here infected code

        fileOpen->write(fullText.toLatin1());
        fileOpen->close();
        fileOpen->deleteLater();
    }
}
