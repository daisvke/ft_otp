#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "TOTPGenerator.hpp"
#include "FileHandler.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnGenerate_clicked()
{

}

