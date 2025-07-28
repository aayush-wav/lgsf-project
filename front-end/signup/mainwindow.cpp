#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDir>
#include <QSysInfo>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUI();

    if (!DBHandler::instance().connectToDB()) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the database.");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    this->showMaximized();

    ui->usernameLineEdit->setPlaceholderText("Username");
    ui->passwordLineEdit->setPlaceholderText("Password");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
    ui->repasswordLineEdit->setPlaceholderText("Re-enter Password");
    ui->repasswordLineEdit->setEchoMode(QLineEdit::Password);

    ui->labelAlreadyHaveAccount->setTextFormat(Qt::RichText);
    ui->labelAlreadyHaveAccount->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->labelAlreadyHaveAccount->setText("<a href=\"#\" style=\"color: white; text-decoration: underline;\">Already have an account?</a>");

    connect(ui->signupBtn, &QPushButton::clicked, this, &MainWindow::on_signupBtn_clicked);
    connect(ui->labelAlreadyHaveAccount, &QLabel::linkActivated, this, &MainWindow::on_labelAlreadyHaveAccount_linkActivated);

    connect(ui->usernameLineEdit, &QLineEdit::returnPressed, [this]() {
        if (ui->usernameLineEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Username is required.");
        } else {
            ui->passwordLineEdit->setFocus();
        }
    });

    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, [this]() {
        if (ui->passwordLineEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Password is required.");
        } else {
            ui->repasswordLineEdit->setFocus();
        }
    });

    connect(ui->repasswordLineEdit, &QLineEdit::returnPressed, this, &MainWindow::on_signupBtn_clicked);
}


void MainWindow::on_signupBtn_clicked()
{
    qDebug() << "Signup button clicked!";

    if (!validateInputs()) {
        return;
    }

    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString passwordHash = hashPassword(password);

    if (DBHandler::instance().createUser(username, passwordHash)) {
        QMessageBox::information(this, "Success", "Signup successful! Redirecting to login...");
        openLoginPage();
    } else {
        QMessageBox::warning(this, "Error", "Signup failed. Username may already exist or database error occurred.");
    }
}

bool MainWindow::validateInputs()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString repassword = ui->repasswordLineEdit->text();

    if (username.isEmpty() || password.isEmpty() || repassword.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "All fields are required.");
        return false;
    }

    if (checkUsernameExists(username)) {
        QMessageBox::warning(this, "Input Error", "Username already exists.");
        return false;
    }

    if (password != repassword) {
        QMessageBox::warning(this, "Input Error", "Passwords do not match.");
        return false;
    }

    return true;
}

QString MainWindow::hashPassword(const QString &password)
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

bool MainWindow::checkUsernameExists(const QString &username)
{
    return DBHandler::instance().usernameExists(username);
}

void MainWindow::openLoginPage()
{
    QProcess *loginProcess = new QProcess(this);
    QString loginPath = QDir::currentPath() + "/login/login" + (QSysInfo::kernelType() == "winnt" ? ".exe" : "");

    if (QFile::exists(loginPath)) {
        loginProcess->start(loginPath);
        if (loginProcess->waitForStarted()) {
            this->close();
        } else {
            QMessageBox::warning(this, "Error", "Could not start login application");
        }
    } else {
        QMessageBox::warning(this, "Error", "Login application not found. Please ensure it's located in the 'login' folder.");
    }
}

void MainWindow::on_labelAlreadyHaveAccount_linkActivated(const QString &link)
{
    Q_UNUSED(link)
    qDebug() << "Login link clicked!";
    openLoginPage();
}
