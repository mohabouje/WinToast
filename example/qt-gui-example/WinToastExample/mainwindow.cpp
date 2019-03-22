#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include "../../../src/wintoastlib.h"
using namespace WinToastLib;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->audioMode->addItem("Default", WinToastTemplate::AudioOption::Default);
    ui->audioMode->addItem("Loop", WinToastTemplate::AudioOption::Loop);
    ui->audioMode->addItem("Silence", WinToastTemplate::AudioOption::Silent);

    ui->audioSystemFile->addItem("Default", WinToastTemplate::AudioSystemFile::DefaultSound);
    ui->audioSystemFile->addItem("Mail", WinToastTemplate::AudioSystemFile::Mail);
    ui->audioSystemFile->addItem("SMS", WinToastTemplate::AudioSystemFile::SMS);
    ui->audioSystemFile->addItem("Alarm", WinToastTemplate::AudioSystemFile::Alarm);

    WinToast::instance()->setAppName(L"WinToastExample");
    WinToast::instance()->setAppUserModelId(
                WinToast::configureAUMI(L"mohabouje", L"wintoast", L"wintoastexample", L"20161006"));
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

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const {
        std::wcout << L"The user clicked in this toast" << std::endl;
    }

    void toastActivated(int actionIndex) const {
        std::wcout << L"The user clicked on button #" << actionIndex << L" in this toast" << std::endl;
    }

    void toastFailed() const {
        std::wcout << L"Error showing current toast" << std::endl;
    }
    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
        case UserCanceled:
            std::wcout << L"The user dismissed this toast" << std::endl;
            break;
        case ApplicationHidden:
            std::wcout <<  L"The application hid the toast using ToastNotifier.hide()" << std::endl;
            break;
        case TimedOut:
            std::wcout << L"The toast has timed out" << std::endl;
            break;
        default:
            std::wcout << L"Toast not activated" << std::endl;
            break;
        }
    }
};

void MainWindow::on_showToast_clicked()
{
    WinToastTemplate templ = WinToastTemplate(WinToastTemplate::ImageAndText04);
    templ.setImagePath(ui->imagePath->text().toStdWString());
    templ.setTextField(ui->firstLine->text().toStdWString(), WinToastTemplate::FirstLine);
    templ.setTextField(ui->secondLine->text().toStdWString(), WinToastTemplate::SecondLine);
    templ.setTextField(ui->secondLine->text().toStdWString(), WinToastTemplate::ThirdLine);
    templ.setExpiration(ui->spinBox->value() * 1000);
    templ.setAudioPath(static_cast<WinToastTemplate::AudioSystemFile>(ui->audioSystemFile->currentData().toInt()));
    templ.setAudioOption(static_cast<WinToastTemplate::AudioOption>(ui->audioMode->currentData().toInt()));
    if (ui->addYes->isChecked()) templ.addAction(L"Yes");
    if (ui->addNo->isChecked()) templ.addAction(L"No");


    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        QMessageBox::warning(this, "Error", "Could not launch your toast notification!");
    }
}
