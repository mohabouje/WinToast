#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include "wintoastlib.h"

using namespace WinToastLib;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->toastType->addItem("ImageAndText01", WinToastTemplate::ImageAndText01);
    ui->toastType->addItem("ImageAndText02", WinToastTemplate::ImageAndText02);
    ui->toastType->addItem("ImageAndText03", WinToastTemplate::ImageAndText03);
    ui->toastType->addItem("ImageAndText04", WinToastTemplate::ImageAndText04);
    ui->toastType->addItem("Text01", WinToastTemplate::Text01);
    ui->toastType->addItem("Text02", WinToastTemplate::Text02);
    ui->toastType->addItem("Text03", WinToastTemplate::Text03);
    ui->toastType->addItem("Text04", WinToastTemplate::Text04);

    ui->audioMode->addItem("Default", WinToastTemplate::AudioOption::Default);
    ui->audioMode->addItem("Loop", WinToastTemplate::AudioOption::Loop);
    ui->audioMode->addItem("Silence", WinToastTemplate::AudioOption::Silent);

    ui->audioSystemFile->addItem("Default", WinToastTemplate::AudioSystemFile::DefaultSound);
    ui->audioSystemFile->addItem("Mail", WinToastTemplate::AudioSystemFile::Mail);
    ui->audioSystemFile->addItem("SMS", WinToastTemplate::AudioSystemFile::SMS);
    ui->audioSystemFile->addItem("Alarm", WinToastTemplate::AudioSystemFile::Alarm);

    ui->cropHint->addItem("Square", WinToastTemplate::CropHint::Square);
    ui->cropHint->addItem("Circle", WinToastTemplate::CropHint::Circle);

    WinToast::instance()->setAppName(L"WinToastExample");
    WinToast::instance()->setAppUserModelId(WinToast::configureAUMI(L"mohabouje", L"wintoast", L"wintoastexample", L"20161006"));
    if (!WinToast::instance()->initialize()) {
        qDebug() << "Error, your system in not compatible!";
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_imagePathSelector_clicked() {
    const QString fileName = QFileDialog::getOpenFileName(this, "Select an image", QDir::currentPath(), "*.png");
    if (fileName.isEmpty()) {
        return;
    }
    ui->imagePath->setText(QDir::toNativeSeparators(fileName));
}

void MainWindow::on_heroPathSelector_clicked() {
    const QString fileName = QFileDialog::getOpenFileName(this, "Select an image", QDir::currentPath(), "*.png");
    if (fileName.isEmpty()) {
        return;
    }
    ui->heroPath->setText(QDir::toNativeSeparators(fileName));
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
                std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
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

void MainWindow::on_showToast_clicked() {
    auto const type        = static_cast<WinToastTemplate::WinToastTemplateType>(ui->toastType->currentData().toInt());
    WinToastTemplate templ = WinToastTemplate(type);
    templ.setImagePath(ui->imagePath->text().toStdWString(), static_cast<WinToastTemplate::CropHint>(ui->cropHint->currentData().toInt()));
    templ.setHeroImagePath(ui->heroPath->text().toStdWString(), ui->inlineHeroImage->isChecked());
    templ.setTextField(ui->firstLine->text().toStdWString(), WinToastTemplate::FirstLine);
    templ.setTextField(ui->secondLine->text().toStdWString(), WinToastTemplate::SecondLine);
    templ.setTextField(ui->thirdLine->text().toStdWString(), WinToastTemplate::ThirdLine);
    templ.setExpiration(ui->spinBox->value() * 1000);
    templ.setAudioPath(static_cast<WinToastTemplate::AudioSystemFile>(ui->audioSystemFile->currentData().toInt()));
    templ.setAudioOption(static_cast<WinToastTemplate::AudioOption>(ui->audioMode->currentData().toInt()));

    if (ui->addYes->isChecked()) {
        templ.addAction(L"Yes");
    }

    if (ui->addNo->isChecked()) {
        templ.addAction(L"No");
    }

    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        QMessageBox::warning(this, "Error", "Could not launch your toast notification!");
    }
}
