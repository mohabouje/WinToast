#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <qfiledialog.h>
#include <qmessagebox.h>

wchar_t*  converToWChar_t(const QString& text){
    wchar_t* c_Text = new wchar_t[text.length() + 1];
    text.toWCharArray(c_Text);
    c_Text[text.length()] = 0; //Add this line should work as you expected
    return c_Text;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    WinToast::instance()->setAppName(L"WinToastExample");
    WinToast::instance()->setAppUserModelId(L"WinToastExample_AUMI");
    if (!WinToast::instance()->initialize()) {
        qDebug() << "Error, your system in not compatible!";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_imagePathSelector_clicked()
{
    const QString fileName = QFileDialog::getOpenFileName(this, "Select an image", QDir::currentPath(), "*.png");
    if (fileName.isEmpty())
        return;
    ui->imagePath->setText(QDir::toNativeSeparators(fileName));

}

void MainWindow::on_showToast_clicked()
{
    WinToastTemplate templ = WinToastTemplate(2, !ui->imagePath->text().isEmpty());
    templ.setImagePath(converToWChar_t(ui->imagePath->text()));
    templ.setTextField(converToWChar_t(ui->firstLine->text()), 0);
    templ.setTextField(converToWChar_t(ui->secondLine->text()), 1);
    if (!WinToast::instance()->showToast(templ)) {
        QMessageBox::warning(this, "Error", "Could not launch your toast notification!");
    }
}
