#ifndef MAINWINDOW_H
# define MAINWINDOW_H
# include <QMainWindow>
# include <stdexcept>

# include "TOTPGenerator.hpp"
# include "FileHandler.hpp"
# include "qrencode.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void    on_btnGenerate_clicked();
    int     generate_TOTP(std::string inputKeyStr);
    void    generate_QRCode(std::string inputKeyStr);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
