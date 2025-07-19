#include "loginwindow.h"
#include "./build/login_autogen/include/ui_loginwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LoginWindow)
    , passwordVisible(false)
    , networkManager(new QNetworkAccessManager(this))
    , isLoggingIn(false)
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
    initializeBackend();
    makeResponsive();
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::setupUI()
{
    // Set window title
    this->setWindowTitle("Login - ई - BADAPATRA");
    
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
    connect(togglePasswordAction, &QAction::triggered, this, &LoginWindow::togglePasswordVisibility);

    // Connect the "New User?" label click
    connect(ui->labelNewUser, &QLabel::linkActivated, this, &LoginWindow::on_labelNewUser_linkActivated);
}

bool LoginWindow::initializeBackend()
{
    // Load backend configuration
    if (!loadBackendConfig()) {
        QMessageBox::critical(this, "Configuration Error",
                              "Could not load backend configuration. Please check your settings.");
        return false;
    }

    // Check server connection
    if (!checkServerConnection()) {
        QMessageBox::warning(this, "Server Connection",
                             "Could not connect to backend server. Please ensure the server is running.");
        // Don't return false here - allow user to try anyway
    }

    qDebug() << "Backend configuration loaded successfully";
    return true;
}

bool LoginWindow::loadBackendConfig()
{
    // Try to load from settings file first
    QSettings settings("backend/backend_config.ini", QSettings::IniFormat);

    if (settings.contains("Backend/serverUrl")) {
        backendConfig.serverUrl = settings.value("Backend/serverUrl").toString();
        backendConfig.port = settings.value("Backend/port", 3000).toInt();
        backendConfig.timeoutMs = settings.value("Backend/timeoutMs", 10000).toInt();
    } else {
        // Default configuration - matches your Node.js server
        backendConfig.serverUrl = "http://localhost";
        backendConfig.port = 3000;
        backendConfig.timeoutMs = 10000;

        // Save default configuration
        settings.setValue("Backend/serverUrl", backendConfig.serverUrl);
        settings.setValue("Backend/port", backendConfig.port);
        settings.setValue("Backend/timeoutMs", backendConfig.timeoutMs);
        settings.sync();

        qDebug() << "Created default backend configuration file at: backend/backend_config.ini";
    }

    return true;
}

bool LoginWindow::checkServerConnection()
{
    // Simple check - try to connect to server (you can implement a /health endpoint)
    QString serverUrl = QString("%1:%2").arg(backendConfig.serverUrl).arg(backendConfig.port);

    QNetworkRequest request;
    request.setUrl(QUrl(serverUrl));
    request.setRawHeader("User-Agent", "Qt Login App");

    QNetworkReply *reply = networkManager->head(request);
    QTimer::singleShot(2000, reply, &QNetworkReply::abort); // 2 second timeout for connection check

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    bool connected = (reply->error() == QNetworkReply::NoError ||
                      reply->error() == QNetworkReply::ContentNotFoundError); // 404 is okay, means server is running

    reply->deleteLater();
    return connected;
}

void LoginWindow::performLogin(const QString &username, const QString &password)
{
    if (isLoggingIn) {
        return; // Prevent multiple simultaneous requests
    }

    isLoggingIn = true;
    showLoading(true);

    // Create JSON object for the request
    QJsonObject json;
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    // Create network request
    QString loginUrl = "http://localhost:3000/login";
    QNetworkRequest request;
    request.setUrl(QUrl(loginUrl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "Qt Login App");

    // Send POST request
    QNetworkReply *reply = networkManager->post(request, data);

    // Set timeout
    QTimer::singleShot(backendConfig.timeoutMs, reply, &QNetworkReply::abort);

    // Connect reply finished signal
    connect(reply, &QNetworkReply::finished, this, &LoginWindow::onLoginReplyFinished);
}

void LoginWindow::onLoginReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    isLoggingIn = false;
    showLoading(false);

    if (reply->error() == QNetworkReply::NoError) {
        // Parse JSON response
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);

        if (doc.isNull()) {
            QMessageBox::critical(this, "Error", "Invalid response from server");
            reply->deleteLater();
            return;
        }

        QJsonObject obj = doc.object();
        bool success = obj["success"].toBool();
        QString message = obj["message"].toString();

        if (success) {
            QMessageBox::information(this, "Success", "Login successful!");
            qDebug() << "Login successful for user:" << ui->usernameLineEdit->text();

            // Open main UI
            openMainUI();
        } else {
            QMessageBox::warning(this, "Login Failed", message.isEmpty() ? "Invalid credentials" : message);
            qDebug() << "Login failed for user:" << ui->usernameLineEdit->text();

            // Clear password field for security
            ui->passwordLineEdit->clear();
        }
    } else if (reply->error() == QNetworkReply::TimeoutError) {
        QMessageBox::critical(this, "Network Error",
                              "Request timed out. Please check your connection and try again.");
    } else {
        // Handle network error
        QString errorMsg = "Failed to connect to server";
        if (reply->error() == QNetworkReply::ConnectionRefusedError) {
            errorMsg = "Server is not running. Please start the backend server and try again.";
        }

        QMessageBox::critical(this, "Network Error", errorMsg);
        qDebug() << "Network error:" << reply->errorString();
    }

    reply->deleteLater();
}

void LoginWindow::showLoading(bool show)
{
    ui->loginBtn->setEnabled(!show);
    ui->loginBtn->setText(show ? "Logging in..." : "Login");
    ui->usernameLineEdit->setEnabled(!show);
    ui->passwordLineEdit->setEnabled(!show);
}

void LoginWindow::openSignupPage()
{
    // Create QProcess with parent for automatic cleanup
    QProcess *signupProcess = new QProcess(this);
    
    // Build a list of possible paths
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/../signup/build/SIGNUP.exe",             // Release build path
        QCoreApplication::applicationDirPath() + "/../signup/build/debug/SIGNUP.exe",       // Debug build path
        "../signup/build/SIGNUP.exe",                                                      // Alternative release path
        "../signup/build/debug/SIGNUP.exe",                                                // Alternative debug path
        "../../signup/build/SIGNUP.exe",                                                   // Relative release path
        "../../signup/build/debug/SIGNUP.exe"                                              // Relative debug path
    };

    QString signupPath;
    for (const QString &path : searchPaths) {
        if (QFile::exists(path)) {
            signupPath = path;
            break;
        }
    }

    if (!signupPath.isEmpty()) {
        // Connect finished signal for cleanup
        connect(signupProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                signupProcess, &QProcess::deleteLater);
        
        // Connect error signal for better error reporting
        connect(signupProcess, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
            QString errorMsg = "Failed to start signup application: ";
            switch (error) {
                case QProcess::FailedToStart:
                    errorMsg += "The process failed to start.";
                    break;
                case QProcess::Crashed:
                    errorMsg += "The process crashed.";
                    break;
                default:
                    errorMsg += "An unknown error occurred.";
            }
            QMessageBox::critical(this, "Error", errorMsg);
        });

        signupProcess->start(signupPath);
        if (!signupProcess->waitForStarted(5000)) {  // 5 second timeout
            QMessageBox::warning(this, "Error", 
                QString("Could not start signup application: %1").arg(signupProcess->errorString()));
            signupProcess->deleteLater();
        }
    } else {
        QMessageBox::warning(this, "Error",
                             "Signup application not found. Please ensure it's built in the 'signup/build' folder.");
    }
}

void LoginWindow::openMainUI()
{
    // Launch chatbot UI application
    QProcess *chatbotProcess = new QProcess(this);
    
    // Build a list of possible paths for the chatbot executable
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/../chatbot/build/chatbot.exe",             // Release build path
        QCoreApplication::applicationDirPath() + "/../chatbot/build/debug/chatbot.exe",       // Debug build path
        "../chatbot/build/chatbot.exe",                                                      // Alternative release path
        "../chatbot/build/debug/chatbot.exe",                                                // Alternative debug path
        "../../chatbot/build/chatbot.exe",                                                   // Relative release path
        "../../chatbot/build/debug/chatbot.exe"                                              // Relative debug path
    };

    QString chatbotPath;
    for (const QString &path : searchPaths) {
        if (QFile::exists(path)) {
            chatbotPath = path;
            break;
        }
    }

    if (!chatbotPath.isEmpty()) {
        // Connect finished signal for cleanup
        connect(chatbotProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                chatbotProcess, &QProcess::deleteLater);
        
        // Connect error signal for better error reporting
        connect(chatbotProcess, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
            QString errorMsg = "Failed to start chatbot application: ";
            switch (error) {
                case QProcess::FailedToStart:
                    errorMsg += "The process failed to start.";
                    break;
                case QProcess::Crashed:
                    errorMsg += "The process crashed.";
                    break;
                default:
                    errorMsg += "An unknown error occurred.";
            }
            QMessageBox::critical(this, "Error", errorMsg);
        });

        chatbotProcess->start(chatbotPath);
        if (chatbotProcess->waitForStarted(5000)) {  // 5 second timeout
            this->close();  // Close login window after successful launch
        } else {
            QMessageBox::warning(this, "Error", 
                QString("Could not start chatbot application: %1").arg(chatbotProcess->errorString()));
            chatbotProcess->deleteLater();
        }
    } else {
        QMessageBox::warning(this, "Error",
            "Chatbot application not found. Please ensure it's built in the 'chatbot/build' folder.");
    }
}

void LoginWindow::makeResponsive()
{
    // Enable responsive behavior
    updateLayout();
}

void LoginWindow::updateLayout()
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

void LoginWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateLayout();
}

bool LoginWindow::eventFilter(QObject *obj, QEvent *event)
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

void LoginWindow::togglePasswordVisibility()
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
void LoginWindow::on_loginBtn_clicked()
{
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();

    // Validate input
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Please enter both username and password.");
        return;
    }

    // Perform login via HTTP request
    performLogin(username, password);
}

// New User label click handler
void LoginWindow::on_labelNewUser_linkActivated(const QString &link)
{
    Q_UNUSED(link)
    openSignupPage();
}

// Legacy handlers (keeping for compatibility)
void LoginWindow::on_pushButton_Login_clicked()
{
    // This might be called if you have another login button
    on_loginBtn_clicked();
}

void LoginWindow::on_labelBadapatra_linkActivated(const QString &link)
{
    Q_UNUSED(link)
    // Handle main label click if needed
}

void LoginWindow::on_usernameLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    Q_UNUSED(arg1)
    Q_UNUSED(arg2)
    // Handle cursor position change if needed
}
