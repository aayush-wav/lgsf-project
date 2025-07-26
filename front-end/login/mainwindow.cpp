#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , passwordVisible(false)
{
    ui->setupUi(this);

    originalWindowSize = this->size();
    originalGeometry.loginBtn = ui->loginBtn->geometry();
    originalGeometry.passwordLineEdit = ui->passwordLineEdit->geometry();
    originalGeometry.usernameLineEdit = ui->usernameLineEdit->geometry();
    originalGeometry.labelBadapatra = ui->labelBadapatra->geometry();
    originalGeometry.labelNewUser = ui->labelNewUser->geometry();

    setupUI();
    initializeDatabase();
    makeResponsive();
}

MainWindow::~MainWindow()
{
    if (database.isOpen()) {
        database.close();
    }
    delete ui;
}

void MainWindow::setupUI()
{
    this->showMaximized();

    ui->usernameLineEdit->setPlaceholderText("Username");
    ui->passwordLineEdit->setPlaceholderText("Password");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    ui->labelNewUser->setTextFormat(Qt::RichText);
    ui->labelNewUser->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->labelNewUser->setText("<a href=\"#\" style=\"color: white; text-decoration: underline;\">New User?</a>");

    ui->usernameLineEdit->installEventFilter(this);
    ui->passwordLineEdit->installEventFilter(this);

    togglePasswordAction = new QAction(QIcon(":/icons/eye_closed.png"), "", this);

    QIcon eyeIcon(":/icons/eye_closed.png");
    QPixmap eyePixmap = eyeIcon.pixmap(24, 24);

    // Create white version of the icon
    QPixmap whitePixmap(eyePixmap.size());
    whitePixmap.fill(Qt::white);
    whitePixmap.setMask(eyePixmap.createMaskFromColor(Qt::transparent));

    togglePasswordAction->setIcon(QIcon(whitePixmap));

    ui->passwordLineEdit->addAction(togglePasswordAction, QLineEdit::TrailingPosition);
    ui->passwordLineEdit->setStyleSheet("QLineEdit { padding-right: 30px; }");

    connect(togglePasswordAction, &QAction::triggered, this, &MainWindow::togglePasswordVisibility);

    connect(ui->labelNewUser, &QLabel::linkActivated, this, &MainWindow::on_labelNewUser_linkActivated);

    setupKeyboardNavigation();
    setupValidation();
}

void MainWindow::setupKeyboardNavigation()
{
    connect(ui->usernameLineEdit, &QLineEdit::returnPressed, this, [this]() {
        ui->passwordLineEdit->setFocus();
    });

    connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, [this]() {
        on_loginBtn_clicked();
    });

    setTabOrder(ui->usernameLineEdit, ui->passwordLineEdit);
    setTabOrder(ui->passwordLineEdit, ui->loginBtn);
}

void MainWindow::setupValidation()
{
    ui->usernameLineEdit->setProperty("required", true);
    ui->passwordLineEdit->setProperty("required", true);

    connect(ui->usernameLineEdit, &QLineEdit::textChanged, this, &MainWindow::validateFields);
    connect(ui->passwordLineEdit, &QLineEdit::textChanged, this, &MainWindow::validateFields);

    validateFields();
}

void MainWindow::validateFields()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    updateFieldStyle(ui->usernameLineEdit, !username.isEmpty());
    updateFieldStyle(ui->passwordLineEdit, !password.isEmpty());
}

void MainWindow::updateFieldStyle(QLineEdit *field, bool isValid)
{
    if (isValid) {
        if (field == ui->passwordLineEdit) {
            field->setStyleSheet("QLineEdit { padding-right: 30px; }");
        } else {
            field->setStyleSheet("");
        }
    } else {
        if (field == ui->passwordLineEdit) {
            field->setStyleSheet("QLineEdit { border: 2px solid #ffcccc; padding-right: 30px; }");
        } else {
            field->setStyleSheet("QLineEdit { border: 2px solid #ffcccc; }");
        }
    }
}

bool MainWindow::initializeDatabase()
{
    if (!loadDatabaseConfig()) {
        QMessageBox::critical(this, "Configuration Error",
                              "Could not load database configuration. Please check your settings.");
        return false;
    }

    if (!connectToDatabase()) {
        return false;
    }

    if (!createTablesIfNotExist()) {
        return false;
    }

    qDebug() << "Database initialized successfully";
    return true;
}

bool MainWindow::loadDatabaseConfig()
{
    QSettings settings("database/db_config.ini", QSettings::IniFormat);

    if (settings.contains("Database/hostName")) {
        dbConfig.hostName = settings.value("Database/hostName").toString();
        dbConfig.databaseName = settings.value("Database/databaseName").toString();
        dbConfig.userName = settings.value("Database/userName").toString();
        dbConfig.password = settings.value("Database/password").toString();
        dbConfig.port = settings.value("Database/port", 5432).toInt();
    } else {
        dbConfig.hostName = "localhost";
        dbConfig.databaseName = "UserInfo";
        dbConfig.userName = "postgres";
        dbConfig.password = "00618";
        dbConfig.port = 5432;

        settings.setValue("Database/hostName", dbConfig.hostName);
        settings.setValue("Database/databaseName", dbConfig.databaseName);
        settings.setValue("Database/userName", dbConfig.userName);
        settings.setValue("Database/password", dbConfig.password);
        settings.setValue("Database/port", dbConfig.port);
        settings.sync();

        qDebug() << "Created default database configuration file at: database/db_config.ini";
        qDebug() << "Please update the configuration with your actual database credentials.";
    }

    return true;
}

bool MainWindow::connectToDatabase()
{
    database = QSqlDatabase::addDatabase("QPSQL");
    database.setHostName(dbConfig.hostName);
    database.setDatabaseName(dbConfig.databaseName);
    database.setUserName(dbConfig.userName);
    database.setPassword(dbConfig.password);
    database.setPort(dbConfig.port);

    if (!database.open()) {
        QMessageBox::critical(this, "Database Connection Error",
                              "Could not connect to PostgreSQL database:\n" +
                                  database.lastError().text() + "\n\n" +
                                  "Please check your database configuration in database/db_config.ini");
        return false;
    }

    qDebug() << "Connected to PostgreSQL database successfully";
    return true;
}

bool MainWindow::createTablesIfNotExist()
{
    QFile sqlFile("database/UserInfo.sql");
    if (sqlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&sqlFile);
        QString sqlContent = in.readAll();
        sqlFile.close();

        QStringList statements = sqlContent.split(';', Qt::SkipEmptyParts);

        QSqlQuery query;
        for (const QString &statement : statements) {
            QString trimmedStatement = statement.trimmed();
            if (!trimmedStatement.isEmpty()) {
                if (!query.exec(trimmedStatement)) {
                    qDebug() << "SQL execution warning:" << query.lastError().text();
                    qDebug() << "Statement:" << trimmedStatement;
                }
            }
        }

        qDebug() << "Executed SQL file: database/UserInfo.sql";
    } else {
        QSqlQuery query;
        QString createUsersTable = "CREATE TABLE IF NOT EXISTS users ("
                                   "id SERIAL PRIMARY KEY, "
                                   "username VARCHAR(255) UNIQUE NOT NULL, "
                                   "password_hash VARCHAR(255) NOT NULL, "
                                   "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";

        if (!query.exec(createUsersTable)) {
            QMessageBox::critical(this, "Database Error",
                                  "Could not create users table: " + query.lastError().text());
            return false;
        }

        qDebug() << "Created basic users table (UserInfo.sql file not found)";
    }

    return true;
}

QString MainWindow::hashPassword(const QString &password)
{
    QByteArray hashed = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return hashed.toHex();
}

bool MainWindow::verifyLogin(const QString &username, const QString &password)
{
    QString hashedPassword = hashPassword(password);

    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE username = $1 AND password_hash = $2");
    query.bindValue(0, username);
    query.bindValue(1, hashedPassword);

    if (!query.exec()) {
        qDebug() << "Database query error:" << query.lastError().text();
        return false;
    }

    return query.next();
}

void MainWindow::openSignupPage()
{
    QProcess *signupProcess = new QProcess(this);

    QString signupPath = "../signup/signup.exe";
    if (!QFile::exists(signupPath)) {
        signupPath = "../signup/signup";
    }
    if (!QFile::exists(signupPath)) {
        signupPath = "./signup/signup.exe";
    }
    if (!QFile::exists(signupPath)) {
        signupPath = "./signup/signup";
    }

    if (QFile::exists(signupPath)) {
        signupProcess->start(signupPath);
        if (!signupProcess->waitForStarted()) {
            QMessageBox::warning(this, "Error", "Could not start signup application");
        }
    } else {
        QMessageBox::warning(this, "Error",
                             "Signup application not found. Please ensure it's located in the 'signup' folder.");
    }
}

void MainWindow::openMainUI()
{
    QProcess *mainUIProcess = new QProcess(this);

    QString mainUIPath = "../main-ui/main-ui.exe";
    if (!QFile::exists(mainUIPath)) {
        mainUIPath = "../main-ui/main-ui";
    }
    if (!QFile::exists(mainUIPath)) {
        mainUIPath = "./main-ui/main-ui.exe";
    }
    if (!QFile::exists(mainUIPath)) {
        mainUIPath = "./main-ui/main-ui";
    }

    if (QFile::exists(mainUIPath)) {
        mainUIProcess->start(mainUIPath);
        if (mainUIProcess->waitForStarted()) {
            this->close();
        } else {
            QMessageBox::warning(this, "Error", "Could not start main UI application");
        }
    } else {
        QMessageBox::warning(this, "Error",
                             "Main UI application not found. Please ensure it's located in the 'main-ui' folder.");
    }
}

void MainWindow::makeResponsive()
{
    updateLayout();
}

void MainWindow::updateLayout()
{
    QSize currentSize = this->size();

    double scaleX = (double)currentSize.width() / originalWindowSize.width();
    double scaleY = (double)currentSize.height() / originalWindowSize.height();

    auto scaleGeometry = [&](const QRect &original) -> QRect {
        return QRect(
            (int)(original.x() * scaleX),
            (int)(original.y() * scaleY),
            (int)(original.width() * scaleX),
            (int)(original.height() * scaleY)
            );
    };

    ui->loginBtn->setGeometry(scaleGeometry(originalGeometry.loginBtn));
    ui->passwordLineEdit->setGeometry(scaleGeometry(originalGeometry.passwordLineEdit));
    ui->usernameLineEdit->setGeometry(scaleGeometry(originalGeometry.usernameLineEdit));
    ui->labelBadapatra->setGeometry(scaleGeometry(originalGeometry.labelBadapatra));
    ui->labelNewUser->setGeometry(scaleGeometry(originalGeometry.labelNewUser));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateLayout();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if ((obj == ui->usernameLineEdit || obj == ui->passwordLineEdit) &&
        event->type() == QEvent::FocusIn)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
        if (lineEdit) {
            lineEdit->setPlaceholderText("");
            if (lineEdit->text().isEmpty()) {
                lineEdit->setStyleSheet("");
            }
        }
    }

    if ((obj == ui->usernameLineEdit || obj == ui->passwordLineEdit) &&
        event->type() == QEvent::FocusOut)
    {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
        if (lineEdit && lineEdit->text().isEmpty()) {
            if (lineEdit == ui->usernameLineEdit)
                lineEdit->setPlaceholderText("Username");
            else if (lineEdit == ui->passwordLineEdit)
                lineEdit->setPlaceholderText("Password");

            lineEdit->setStyleSheet("QLineEdit { border: 2px solid #ffcccc; }");
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::togglePasswordVisibility()
{
    passwordVisible = !passwordVisible;
    if (passwordVisible) {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);

        QIcon eyeIcon(":/icons/eye_open.png");
        QPixmap eyePixmap = eyeIcon.pixmap(24, 24);

        // Create white version of the icon
        QPixmap whitePixmap(eyePixmap.size());
        whitePixmap.fill(Qt::white);
        whitePixmap.setMask(eyePixmap.createMaskFromColor(Qt::transparent));

        togglePasswordAction->setIcon(QIcon(whitePixmap));
    } else {
        ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

        QIcon eyeIcon(":/icons/eye_closed.png");
        QPixmap eyePixmap = eyeIcon.pixmap(24, 24);

        // Create white version of the icon
        QPixmap whitePixmap(eyePixmap.size());
        whitePixmap.fill(Qt::white);
        whitePixmap.setMask(eyePixmap.createMaskFromColor(Qt::transparent));

        togglePasswordAction->setIcon(QIcon(whitePixmap));
    }
}

void MainWindow::on_loginBtn_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    if (username.isEmpty() && password.isEmpty()) {
        QMessageBox::warning(this, "Login Error",
                             "Both username and password are required.\nPlease fill in both fields.");
        ui->usernameLineEdit->setFocus();
        return;
    }

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Login Error",
                             "Username is required.\nPlease enter your username.");
        ui->usernameLineEdit->setFocus();
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "Login Error",
                             "Password is required.\nPlease enter your password.");
        ui->passwordLineEdit->setFocus();
        return;
    }

    if (verifyLogin(username, password)) {
        QMessageBox::information(this, "Success", "Login verified successfully!");
        qDebug() << "Login successful for user:" << username;
        openMainUI();
    } else {
        QMessageBox::warning(this, "Login Failed",
                             "Invalid username or password.\nPlease check your credentials and try again.");
        qDebug() << "Login failed for user:" << username;
        ui->passwordLineEdit->clear();
        ui->usernameLineEdit->setFocus();
        ui->usernameLineEdit->selectAll();
    }
}

void MainWindow::on_labelNewUser_linkActivated(const QString &link)
{
    Q_UNUSED(link)
    openSignupPage();
}

void MainWindow::on_pushButton_Login_clicked()
{
    on_loginBtn_clicked();
}

void MainWindow::on_labelBadapatra_linkActivated(const QString &link)
{
    Q_UNUSED(link)
}

void MainWindow::on_usernameLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    Q_UNUSED(arg1)
    Q_UNUSED(arg2)
}
