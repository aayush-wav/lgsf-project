#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , passwordVisible(false)
{
    ui->setupUi(this);
    this->showMaximized();

    ui->usernameLineEdit->setPlaceholderText("Username");
    ui->passwordLineEdit->setPlaceholderText("Password");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    ui->labelNewUser->setTextFormat(Qt::RichText);
    ui->labelNewUser->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->labelNewUser->setOpenExternalLinks(true);
    ui->labelNewUser->setText("<a href=\"#\">New User?</a>");



    ui->usernameLineEdit->installEventFilter(this);
    ui->passwordLineEdit->installEventFilter(this);

    togglePasswordAction = new QAction(QIcon(":/icons/eye_closed.png"), "", this);
    ui->passwordLineEdit->addAction(togglePasswordAction, QLineEdit::TrailingPosition);

    connect(togglePasswordAction, &QAction::triggered, this, &MainWindow::togglePasswordVisibility);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == ui->usernameLineEdit || obj == ui->passwordLineEdit) &&
        event->type() == QEvent::FocusIn)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
        if (lineEdit)
            lineEdit->setPlaceholderText("");
    }

    if ((obj == ui->usernameLineEdit || obj == ui->passwordLineEdit) &&
        event->type() == QEvent::FocusOut)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
        if (lineEdit && lineEdit->text().isEmpty()) {
            if (lineEdit == ui->usernameLineEdit)
                lineEdit->setPlaceholderText("Username.");
            else if (lineEdit == ui->passwordLineEdit)
                lineEdit->setPlaceholderText("Password.");
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Login_clicked()
{
}

void MainWindow::togglePasswordVisibility()
{
    passwordVisible = !passwordVisible;
    if (passwordVisible) {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
        togglePasswordAction->setIcon(QIcon(":/icons/eye_open.png"));
    } else {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);
        togglePasswordAction->setIcon(QIcon(":/icons/eye_closed.png"));
    }
}
