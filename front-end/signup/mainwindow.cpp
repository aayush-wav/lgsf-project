#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , passwordVisible(false)
{
    ui->setupUi(this);
    this->showMaximized();

    ui->firstNameLineEdit->setPlaceholderText("Username");
    ui->lastNameLineEdit->setPlaceholderText("Password");
    ui->lastNameLineEdit->setEchoMode(QLineEdit::Password);



    ui->firstNameLineEdit->installEventFilter(this);
    ui->lastNameLineEdit->installEventFilter(this);

    togglePasswordAction = new QAction(QIcon(":/icons/eye_closed.png"), "", this);
    ui->lastNameLineEdit->addAction(togglePasswordAction, QLineEdit::TrailingPosition);

    connect(togglePasswordAction, &QAction::triggered, this, &MainWindow::togglePasswordVisibility);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == ui->firstNameLineEdit || obj == ui->lastNameLineEdit) &&
        event->type() == QEvent::FocusIn)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
        if (lineEdit)
            lineEdit->setPlaceholderText("");
    }

    if ((obj == ui->firstNameLineEdit || obj == ui->lastNameLineEdit) &&
        event->type() == QEvent::FocusOut)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
        if (lineEdit && lineEdit->text().isEmpty()) {
            if (lineEdit == ui->firstNameLineEdit)
                lineEdit->setPlaceholderText("Username");
            else if (lineEdit == ui->lastNameLineEdit)
                lineEdit->setPlaceholderText("Password");
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
        ui->lastNameLineEdit->setEchoMode(QLineEdit::Normal);
        togglePasswordAction->setIcon(QIcon(":/icons/eye_open.png"));
    } else {
        ui->lastNameLineEdit->setEchoMode(QLineEdit::Password);
        togglePasswordAction->setIcon(QIcon(":/icons/eye_closed.png"));
    }
}
