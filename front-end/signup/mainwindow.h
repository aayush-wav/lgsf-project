#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QProcess>
#include <QDebug>
#include "dbhandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_signupBtn_clicked();
    void on_labelAlreadyHaveAccount_linkActivated(const QString &link);

private:
    Ui::MainWindow *ui;
    void setupUI();
    bool validateInputs();
    QString hashPassword(const QString &password);
    bool checkUsernameExists(const QString &username);
    void openLoginPage();
    QProcess *loginProcess = nullptr;
};

#endif // MAINWINDOW_H
