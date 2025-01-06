#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Set the window title
    this->setWindowTitle("ft_otp");

    // Set initial error labels properties
    ui->keyErrorLabel->setStyleSheet("color: red;");
    ui->keyErrorLabel->setText(""); // Ensure it's empty at the start

    ui->TOTPErrorLabel->setStyleSheet("color: red;");
    ui->TOTPErrorLabel->setText(""); // Ensure it's empty at the start
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnGenerate_clicked()
{
    // Retrieve the key entered by the user
    QString     inputKeyQStr = ui->lineKey->text();
    std::string inputKeyStr = inputKeyQStr.toStdString();

    ui->lineTOTP->setText("");
    // Show an error message at the bottom of the key field if it is empty
    if (inputKeyQStr.isEmpty()) {
        ui->keyErrorLabel->setText("Error: Please enter a key.");
        return;
    }

    TOTPGenerator   TOTPGen(false);
    uint8_t         keyFormat = TOTPGen.isValidHexOrBase32(inputKeyStr);
    QString         TOTP;

    // Show an error message at the bottom of the key is not Hex or Base32
    if (keyFormat == 0) {
        ui->keyErrorLabel->setText("Error: Please enter a valid key.");
        return;
    }
    else ui->keyErrorLabel->setText("");

    try {
        TOTP = QString::fromStdString(TOTPGen.generateTOTPHmacSha1(inputKeyStr, 30, 6));
        ui->lineTOTP->setText(TOTP);
    } catch (const std::exception &e) {
        ui->TOTPErrorLabel->setText("Error: Something went wrong.");
    }
}
