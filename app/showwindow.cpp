#include "showwindow.h"
#include "ui_showwindow.h"

ShowWindow::ShowWindow(QWidget *parent, QString title, QString content) :
    QMainWindow(parent),
    ui(new Ui::ShowWindow)
{
    ui->setupUi(this);

    this->ui->fileBrowser->setText(content);
    this->setWindowTitle(title);
}

ShowWindow::~ShowWindow()
{
    delete ui;
}
