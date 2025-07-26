#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , passwordVisible(false)
{
    ui->setupUi(this);

    // Store original geometry for responsiveness
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
    // Maximize window
    this->showMaximized();

    // Setup line edits
    ui->usernameLineEdit->setPlaceholderText("Username");
    ui->passwordLineEdit->setPlaceholderText("Password");
    ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    // Setup "New User?" label as clickable link
    ui->labelNewUser->setTextFormat(Qt::RichText);
    ui->labelNewUser->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->labelNewUser->setText("<a href=\"#\" style=\"color: white; text-decoration: underline;\">New User?</a>");

    // Install event filters for placeholder text behavior
    ui->usernameLineEdit->installEventFilter(this);
    ui->passwordLineEdit->installEventFilter(this);

    // Setup password visibility toggle
    togglePasswordAction = new QAction(QIcon(":/icons/eye_closed.png"), "", this);
    ui->passwordLineEdit->addAction(togglePasswordAction, QLineEdit::TrailingPosition);
    connect(togglePasswordAction, &QAction::triggered, this, &MainWindow::togglePasswordVisibility);

    // Connect the "New User?" label click
    connect(ui->labelNewUser, &QLabel::linkActivated, this, &MainWindow::on_labelNewUser_linkActivated);
}

bool MainWindow::initializeDatabase()
{
    // Load database configuration
    if (!loadDatabaseConfig()) {
        QMessageBox::critical(this, "Configuration Error",
         "Could not load database configuration. Please check your settings.");
        return false;
    }

    // Connect to database
    if (!connectToDatabase()) {
        return false;
    }

    // Create tables if they don't exist
    if (!createTablesIfNotExist()) {
        return false;
    }

    qDebug() << "Database initialized successfully";
    return true;
}

bool MainWindow::loadDatabaseConfig()
{
    // Try to load from settings file first
    QSettings settings("database/db_config.ini", QSettings::IniFormat);

    if (settings.contains("Database/hostName")) {
        dbConfig.hostName = settings.value("Database/hostName").toString();
        dbConfig.databaseName = settings.value("Database/databaseName").toString();
        dbConfig.userName = settings.value("Database/userName").toString();
        dbConfig.password = settings.value("Database/password").toString();
        dbConfig.port = settings.value("Database/port", 5432).toInt();
    } else {
        // Default configuration - you should modify these values
        dbConfig.hostName = "localhost";
        dbConfig.databaseName = "UserInfo";
        dbConfig.userName = "postgres";
        dbConfig.password = "password";  // Change this to your actual password
        dbConfig.port = 5432;

        // Save default configuration
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
    // Create PostgreSQL database connection
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
    // First, try to read and execute the SQL file
    QFile sqlFile("database/UserInfo.sql");
    if (sqlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&sqlFile);
        QString sqlContent = in.readAll();
        sqlFile.close();

        // Split SQL content by semicolons and execute each statement
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
        // Fallback: create basic users table if SQL file doesn't exist
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

    return query.next(); // Returns true if a matching record is found
}

void MainWindow::openSignupPage()
{
    // Launch signup application from signup folder
    QProcess *signupProcess = new QProcess(this);

    // Try different possible paths for the signup executable
    QString signupPath = "../signup/signup.exe";  // Windows
    if (!QFile::exists(signupPath)) {
        signupPath = "../signup/signup";  // Linux/Mac
    }
    if (!QFile::exists(signupPath)) {
        signupPath = "./signup/signup.exe";  // Alternative Windows path
    }
    if (!QFile::exists(signupPath)) {
        signupPath = "./signup/signup";  // Alternative Linux/Mac path
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
    // Launch main UI application from main-ui folder
    QProcess *mainUIProcess = new QProcess(this);

    // Try different possible paths for the main UI executable
    QString mainUIPath = "../main-ui/main-ui.exe";  // Windows
    if (!QFile::exists(mainUIPath)) {
        mainUIPath = "../main-ui/main-ui";  // Linux/Mac
    }
    if (!QFile::exists(mainUIPath)) {
        mainUIPath = "./main-ui/main-ui.exe";  // Alternative Windows path
    }
    if (!QFile::exists(mainUIPath)) {
        mainUIPath = "./main-ui/main-ui";  // Alternative Linux/Mac path
    }

    if (QFile::exists(mainUIPath)) {
        mainUIProcess->start(mainUIPath);
        if (mainUIProcess->waitForStarted()) {
            // Close current login window after successfully launching main UI
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
    // Enable responsive behavior
    updateLayout();
}

void MainWindow::updateLayout()
{
    // Get current window size
    QSize currentSize = this->size();

    // Calculate scaling factors
    double scaleX = (double)currentSize.width() / originalWindowSize.width();
    double scaleY = (double)currentSize.height() / originalWindowSize.height();

    // Apply scaling to all widgets
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
        if (lineEdit)
            lineEdit->setPlaceholderText("");
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
        }
    }
    return QMainWindow::eventFilter(obj, event);
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

// Login button click handler
void MainWindow::on_loginBtn_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    // Validate input
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Please enter both username and password.");
        return;
    }

    // Verify login credentials
    if (verifyLogin(username, password)) {
        QMessageBox::information(this, "Success", "Login verified successfully!");
        qDebug() << "Login successful for user:" << username;

        // Open main UI
        openMainUI();
    } else {
        QMessageBox::warning(this, "Login Failed", "Password or username not verified.");
        qDebug() << "Login failed for user:" << username;

        // Clear password field for security
        ui->passwordLineEdit->clear();
    }
}

// New User label click handler
void MainWindow::on_labelNewUser_linkActivated(const QString &link)
{
    Q_UNUSED(link)
    openSignupPage();
}

// Legacy handlers (keeping for compatibility)
void MainWindow::on_pushButton_Login_clicked()
{
    // This might be called if you have another login button
    on_loginBtn_clicked();
}

void MainWindow::on_labelBadapatra_linkActivated(const QString &link)
{
    Q_UNUSED(link)
    // Handle main label click if needed
}

void MainWindow::on_usernameLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    Q_UNUSED(arg1)
    Q_UNUSED(arg2)
    // Handle cursor position change if needed
}
