#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QMainWindow>

#include "Cmd.h"

namespace Ui {
class EditorWindow;
}

class EditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = 0, Fuzzer *f = 0, QString fileName = "", QString c = "", QString a = "");
    ~EditorWindow();
    void setContent(QString);

private slots:
    void on_saveText_clicked();

private:
    Ui::EditorWindow *ui;
    Fuzzer *fuzzer;
    QString fileName;
    QString context;
    QString absolutePath;
};

#endif // EDITORWINDOW_H
