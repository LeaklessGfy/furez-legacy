#include "editorwindow.h"
#include "ui_editorwindow.h"

#include <QMessageBox>

EditorWindow::EditorWindow(QWidget *parent, Fuzzer *f, QString fileName, QString c, QString a) :
    QMainWindow(parent),
    ui(new Ui::EditorWindow)
{
    ui->setupUi(this);

    this->fuzzer = f;
    this->fileName = fileName;
    this->context = c;
    this->absolutePath = a;

    this->ui->fileName->setText(fileName);
}

EditorWindow::~EditorWindow()
{
    delete ui;
}

void EditorWindow::setContent(QString fileContent)
{
    this->ui->textEditor->setPlainText(fileContent);
}

void EditorWindow::on_saveText_clicked()
{
    QString fullText;

    QFile *fileOpen = new QFile;
    fileOpen->setFileName(absolutePath);
    fileOpen->open(QIODevice::WriteOnly);

    if(fileOpen->isOpen()) {
        fullText = this->ui->textEditor->toPlainText();

        fileOpen->write(fullText.toLatin1());
        fileOpen->close();
        fileOpen->deleteLater();
    }

    QString t = this->fuzzer->uploadFile(fileName, absolutePath, context);

    //Handle post on server
    //response = this->cmd->vim(this->context, this->ui->textEditor->toPlainText());
}
