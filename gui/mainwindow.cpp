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
    ui->keyErrorLabel->setAlignment(Qt::AlignCenter);

    ui->TOTPErrorLabel->setStyleSheet("color: red;");
    ui->TOTPErrorLabel->setText(""); // Ensure it's empty at the start
    ui->TOTPErrorLabel->setAlignment(Qt::AlignCenter);

    ui->QRLabel->setAlignment(Qt::AlignCenter);
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::generate_TOTP(std::string inputKeyStr)
{
    TOTPGenerator   TOTPGen(false);
    uint8_t         keyFormat = TOTPGen.isValidHexOrBase32(inputKeyStr);
    QString         TOTP;

    // Show an error message at the bottom of the key is not Hex or Base32
    if (keyFormat == 0) {
        ui->keyErrorLabel->setText("Error: Please enter a valid key.");
        return 1;
    }
    else ui->keyErrorLabel->setText("");

    try {
        TOTP = QString::fromStdString(TOTPGen.generateTOTPHmacSha1(inputKeyStr, 30, 6));
        ui->lineTOTP->setText(TOTP);
        if (TOTP.isEmpty()) throw std::exception();
    } catch (const std::exception &e) {
        ui->TOTPErrorLabel->setText("Error:  TOTP generation failed.");
        return 1;
    }
    return 0;
}

void MainWindow::generate_QRCode(std::string inputKeyStr)
{
    try
    {
        QRcode  *qr = generateQRCodeFromURI(inputKeyStr, false);
        if (qr) {
            // Create a QImage from the QR code data
            int size = qr->width;
            QImage image(size, size, QImage::Format_RGB888);

            // Set the QR code pixels in the QImage
            for (int y = 0; y < size; ++y) {
                for (int x = 0; x < size; ++x) {
                    int color = qr->data[y * size + x] & 0x01 ? 0x000000 : 0xFFFFFF;
                    image.setPixel(x, y, color);
                }
            }

            // Convert the QImage to QPixmap for display
            QPixmap pixmap = QPixmap::fromImage(image);
            if (!pixmap) throw QRCodeGenerationException();

            // Set the QPixmap on the corresponding QLabel
            ui->QRLabel->setPixmap(
                pixmap.scaled(ui->QRLabel->size(), Qt::KeepAspectRatio)
                );

            // Free the QR code memory
            QRcode_free(qr);
        } else throw QRCodeGenerationException();
    }
    catch (const std::exception &e)
    {
        ui->QRErrorLabel->setText("Error: QR code generation failed.");
    }
}

void MainWindow::on_btnGenerate_clicked()
{
    // Retrieve the key entered by the user
    QString     inputKeyQStr = ui->lineKey->text();
    std::string inputKeyStr = inputKeyQStr.toStdString();

    ui->lineTOTP->setText("");  // Clear the previous TOTP
    ui->QRLabel->clear();       // Clear the previous QR code

    // Show an error message at the bottom of the key field if it is empty
    if (inputKeyQStr.isEmpty()) {
        ui->keyErrorLabel->setText("Error: Please enter a key.");
        return;
    }

    if (generate_TOTP(inputKeyStr)) return; // Return if error (1) is returned
    generate_QRCode(inputKeyStr);
}
