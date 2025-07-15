#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->firstNameLineEdit->setPlaceholderText("First Name");
    ui->lastNameLineEdit->setPlaceholderText("Last Name");
    ui->passwordLineEdit->setPlaceholderText("Password");
    ui->retypePasswordLineEdit->setPlaceholderText("Re-type Password");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_cursorPositionChanged(int arg1, int arg2)
{


}


void MainWindow::on_signupLineEdit_clicked()
{
}

