#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QScrollBar>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QMessageBox>
#include <QSplitter>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QRandomGenerator>
#include <QUuid>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>

ConversationButton::ConversationButton(const QString &conversationId, const QString &text, QWidget *parent)
    : QPushButton(text, parent), m_conversationId(conversationId)
{
    setStyleSheet(R"(
        QPushButton {
            background-color: #404040;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 13px;
            color: #cccccc;
            text-align: left;
            min-height: 20px;
        }
        QPushButton:hover {
            background-color: #4a4a4a;
        }
        QPushButton:pressed, QPushButton:checked {
            background-color: #4a9eff;
            color: #ffffff;
        }
    )");
    setCheckable(true);
    setMinimumHeight(45);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QPushButton::customContextMenuRequested, [this, conversationId](const QPoint &pos) {
        QMenu *menu = new QMenu(this);
        QAction *deleteAction = menu->addAction("Delete Conversation");


        connect(deleteAction, &QAction::triggered, [this, conversationId]() {
            if (QMessageBox::question(this, "Delete Conversation",
                                      "Are you sure you want to delete this conversation?",
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

                MainWindow* mainWindow = qobject_cast<MainWindow*>(this->window());
                if (mainWindow) {
                    mainWindow->deleteConversation(conversationId);
                }
            }
        });
        menu->exec(mapToGlobal(pos));
        menu->deleteLater();
    });
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentCharIndex(0),
    typingTimer(new QTimer(this)),
    typingLabel(nullptr),
    lastIntentTag(""),
    waitingForNumberSelection(false),
    hasStartedChatting(false),
    sidebarVisible(false),
    currentConversationId(""),
    newConversationButton(nullptr),
    deleteConversationButton(nullptr)
{
    ui->setupUi(this);
    setupUI();

    typingTimer->setInterval(30);
    connect(typingTimer, &QTimer::timeout, this, &MainWindow::onTypingTimeout);


    QJsonDocument doc = loadIntents("D:/LGSF/back-end/json/responses.json");
    if (!doc.isNull())
        intentList = parseIntents(doc.array());

    setupDatabase();
    if(testDatabaseConnection()) {
        createDatabaseTables();
        loadAllConversations();


        if(conversations.isEmpty()) {
            createNewConversation();
        } else {

            loadConversation(conversations.first().conversationId);
        }
    }
}

MainWindow::~MainWindow()
{
    if (db.isOpen())
        db.close();
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("ई-BADAPATRA - Government Services Assistant");
    setMinimumSize(1000, 700);
    resize(1200, 800);

    setStyleSheet(R"(
        QMainWindow {
            background-color: #1a1a1a;
            color: #ffffff;
        }
    )");

    centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSidebar();
    setupChatArea();
}

void MainWindow::setupSidebar()
{
    sidebar = new QFrame();
    sidebar->setFixedWidth(0);
    sidebar->setStyleSheet(R"(
        QFrame {
            background-color: #2d2d2d;
            border-right: 1px solid #444444;
        }
    )");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 20, 15, 20);
    sidebarLayout->setSpacing(15);

    QLabel *sidebarTitle = new QLabel("Conversations");
    sidebarTitle->setStyleSheet(R"(
        QLabel {
            font-family: 'Segoe UI', sans-serif;
            font-size: 16px;
            font-weight: bold;
            color: #ffffff;
            padding: 10px 0px;
        }
    )");
    sidebarLayout->addWidget(sidebarTitle);


    newConversationButton = new QPushButton("+ New Conversation");
    newConversationButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4a9eff;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 13px;
            color: #ffffff;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #3d8ce6;
        }
        QPushButton:pressed {
            background-color: #3579cc;
        }
    )");
    newConversationButton->setMinimumHeight(45);
    connect(newConversationButton, &QPushButton::clicked, [this]() {
        createNewConversation();
    });
    sidebarLayout->addWidget(newConversationButton);


    historyScrollArea = new QScrollArea();
    historyScrollArea->setStyleSheet(R"(
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        QScrollBar:vertical {
            background-color: #404040;
            width: 6px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background-color: #606060;
            border-radius: 3px;
        }
    )");
    historyScrollArea->setWidgetResizable(true);
    historyScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    historyWidget = new QWidget();
    historyLayout = new QVBoxLayout(historyWidget);
    historyLayout->setAlignment(Qt::AlignTop);
    historyLayout->setSpacing(8);

    historyScrollArea->setWidget(historyWidget);
    sidebarLayout->addWidget(historyScrollArea);


    deleteConversationButton = new QPushButton("🗑 Delete Current");
    deleteConversationButton->setStyleSheet(R"(
        QPushButton {
            background-color: #dc3545;
            border: none;
            border-radius: 8px;
            padding: 12px;
            font-size: 13px;
            color: #ffffff;
        }
        QPushButton:hover {
            background-color: #c82333;
        }
        QPushButton:pressed {
            background-color: #bd2130;
        }
        QPushButton:disabled {
            background-color: #666666;
            color: #999999;
        }
    )");
    deleteConversationButton->setMinimumHeight(45);
    connect(deleteConversationButton, &QPushButton::clicked, this, &MainWindow::deleteCurrentConversation);
    sidebarLayout->addWidget(deleteConversationButton);

    mainLayout->addWidget(sidebar);

    sidebarAnimation = new QPropertyAnimation(sidebar, "maximumWidth");
    sidebarAnimation->setDuration(300);
    sidebarAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void MainWindow::setupChatArea()
{
    chatArea = new QWidget();
    chatArea->setStyleSheet("background-color: #1a1a1a;");

    chatAreaLayout = new QVBoxLayout(chatArea);
    chatAreaLayout->setContentsMargins(0, 0, 0, 0);
    chatAreaLayout->setSpacing(0);

    headerFrame = new QFrame();
    headerFrame->setFixedHeight(60);
    headerFrame->setStyleSheet(R"(
        QFrame {
            background-color: #2d2d2d;
            border-bottom: 1px solid #444444;
        }
    )");

    headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(20, 0, 20, 0);

    menuButton = new QPushButton("☰");
    menuButton->setFixedSize(40, 40);
    menuButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            font-size: 18px;
            color: #ffffff;
            border-radius: 8px;
        }
        QPushButton:hover {
            background-color: #404040;
        }
        QPushButton:pressed {
            background-color: #505050;
        }
    )");
    connect(menuButton, &QPushButton::clicked, this, &MainWindow::toggleSidebar);
    headerLayout->addWidget(menuButton);

    headerLabel = new QLabel("ई - BADAPATRA");
    headerLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Segoe UI', sans-serif;
            font-size: 20px;
            font-weight: bold;
            color: #4a9eff;
            margin-left: 15px;
        }
    )");
    headerLayout->addWidget(headerLabel);
    headerLayout->addStretch();

    chatAreaLayout->addWidget(headerFrame);

    QWidget *chatContent = new QWidget();
    QVBoxLayout *chatContentLayout = new QVBoxLayout(chatContent);
    chatContentLayout->setContentsMargins(0, 0, 0, 0);
    chatContentLayout->setSpacing(0);

    welcomeLabel = new QLabel("ई - BADAPATRA");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Segoe UI', sans-serif;
            font-size: 48px;
            font-weight: bold;
            color: #FFFFFF;
            background-color: transparent;
            margin: 50px;
        }
    )");

    ui->chatScrollArea = new QScrollArea();
    ui->chatScrollArea->setStyleSheet(R"(
        QScrollArea {
            border: none;
            background-color: #1a1a1a;
        }
        QScrollBar:vertical {
            background-color: #404040;
            width: 8px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background-color: #606060;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #707070;
        }
    )");

    QWidget* scrollWidget = new QWidget();
    ui->chatLayout = new QVBoxLayout(scrollWidget);
    ui->chatLayout->setAlignment(Qt::AlignTop);
    ui->chatLayout->setSpacing(15);
    ui->chatLayout->setContentsMargins(20, 20, 20, 20);

    ui->chatLayout->addStretch();
    ui->chatLayout->addWidget(welcomeLabel, 0, Qt::AlignCenter);
    ui->chatLayout->addStretch();

    ui->chatScrollArea->setWidget(scrollWidget);
    ui->chatScrollArea->setWidgetResizable(true);
    ui->chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->chatScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    chatContentLayout->addWidget(ui->chatScrollArea);

    QFrame *inputFrame = new QFrame();
    inputFrame->setFixedHeight(80);
    inputFrame->setStyleSheet(R"(
        QFrame {
            background-color: #2d2d2d;
            border-top: 1px solid #444444;
        }
    )");

    QHBoxLayout *inputLayout = new QHBoxLayout(inputFrame);
    inputLayout->setContentsMargins(20, 15, 20, 15);
    inputLayout->setSpacing(15);

    ui->inputLineEdit = new QLineEdit();
    ui->inputLineEdit->setPlaceholderText("Ask ई - BADAPATRA anything about government services...");
    ui->inputLineEdit->setStyleSheet(R"(
        QLineEdit {
            background-color: #404040;
            border: 2px solid #555555;
            border-radius: 25px;
            padding: 12px 20px;
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
            color: #ffffff;
        }
        QLineEdit:focus {
            border-color: #4a9eff;
            outline: none;
        }
        QLineEdit::placeholder {
            color: #888888;
        }
    )");

    ui->sendButton = new QPushButton("SEND");
    ui->sendButton->setFixedSize(80, 50);
    ui->sendButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4a9eff;
            border: none;
            border-radius: 25px;
            color: white;
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #3d8ce6;
        }
        QPushButton:pressed {
            background-color: #3579cc;
        }
        QPushButton:disabled {
            background-color: #666666;
            color: #999999;
        }
    )");

    connect(ui->inputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::handleSendButtonClicked);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::handleSendButtonClicked);

    inputLayout->addWidget(ui->inputLineEdit);
    inputLayout->addWidget(ui->sendButton);

    chatContentLayout->addWidget(inputFrame);
    chatAreaLayout->addWidget(chatContent);

    mainLayout->addWidget(chatArea);
}

void MainWindow::setupDatabase()
{
    conversationsDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/conversations";
    QDir().mkpath(conversationsDir);
    qDebug() << "Conversations directory:" << conversationsDir;

    db = QSqlDatabase::addDatabase("QSQLITE", "conversations");
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    db.setDatabaseName(dbPath + "/badapatra_conversations.db");

    if (!db.open()) {
        qWarning() << "Conversation database connection failed:" << db.lastError().text();
        QMessageBox::critical(this, "Database Error",
                              QString("Failed to create/open conversation database:\n%1")
                                  .arg(db.lastError().text()));
    } else {
        qDebug() << "SQLite conversation database connected successfully!";
        qDebug() << "Database location:" << db.databaseName();
    }

    servicesDb = QSqlDatabase::addDatabase("QPSQL", "services");
    servicesDb.setHostName("localhost");
    servicesDb.setPort(5432);
    servicesDb.setDatabaseName("LgsfInfo");
    servicesDb.setUserName("postgres");
    servicesDb.setPassword("00618");

    if (!servicesDb.open()) {
        qWarning() << "Services database connection failed:" << servicesDb.lastError().text();
        qDebug() << "App will work with limited functionality (no service lookups)";
    } else {
        qDebug() << "PostgreSQL services database connected successfully!";
    }
}

bool MainWindow::testDatabaseConnection()
{
    bool conversationsOk = false;
    bool servicesOk = false;


    if (db.isOpen()) {
        QSqlQuery testQuery(db);
        if (testQuery.exec("SELECT 1")) {
            conversationsOk = true;
            qDebug() << "Conversations database test successful";
        }
    }


    if (servicesDb.isOpen()) {
        QSqlQuery testQuery(servicesDb);
        if (testQuery.exec("SELECT 1")) {
            servicesOk = true;
            qDebug() << "Services database test successful";
        }
    }

    return conversationsOk;
}

void MainWindow::createDatabaseTables()
{
    QSqlQuery query(db);

    QString createConversationsTable = R"(
        CREATE TABLE IF NOT EXISTS conversations (
            conversation_id TEXT PRIMARY KEY,
            conversation_name TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            last_activity DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!query.exec(createConversationsTable)) {
        qWarning() << "Failed to create conversations table:" << query.lastError().text();
        return;
    }

    QString createMessagesTable = R"(
        CREATE TABLE IF NOT EXISTS messages (
            message_id INTEGER PRIMARY KEY AUTOINCREMENT,
            conversation_id TEXT REFERENCES conversations(conversation_id) ON DELETE CASCADE,
            message_text TEXT NOT NULL,
            message_type TEXT NOT NULL CHECK (message_type IN ('user', 'bot')),
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!query.exec(createMessagesTable)) {
        qWarning() << "Failed to create messages table:" << query.lastError().text();
        return;
    }

    query.exec("CREATE INDEX IF NOT EXISTS idx_messages_conversation_id ON messages(conversation_id)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_messages_timestamp ON messages(timestamp)");

    qDebug() << "Conversation database tables created successfully";
}
void MainWindow::createNewConversation(const QString &firstMessage)
{
    if (typingTimer->isActive()) {
        typingTimer->stop();
        if (typingLabel) {
            typingLabel->setText(pendingText);
            addTimeLabelToTypingMessage();
            saveMessage(pendingText, "bot");
            typingLabel = nullptr;
        }
        ui->sendButton->setText("SEND");
        ui->sendButton->setEnabled(true);
    }

    clearChatDisplay();

    currentConversationId = "";
    waitingForNumberSelection = false;
    currentOptions.clear();
    lastIntentTag = "";

    if (db.isOpen()) {
        QString conversationId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        QString conversationName = firstMessage.isEmpty() ? "New Conversation" : generateConversationName(firstMessage);

        QSqlQuery query(db);
        query.prepare("INSERT INTO conversations (conversation_id, conversation_name) VALUES (?, ?)");
        query.addBindValue(conversationId);
        query.addBindValue(conversationName);

        if (query.exec()) {
            currentConversationId = conversationId;

            Conversation newConv;
            newConv.conversationId = conversationId;
            newConv.conversationName = conversationName;
            newConv.createdAt = QDateTime::currentDateTime();
            newConv.lastActivity = QDateTime::currentDateTime();

            conversations.prepend(newConv);
            refreshConversationList();

            qDebug() << "New conversation created:" << conversationId;
        } else {
            qWarning() << "Failed to create new conversation:" << query.lastError().text();
        }
    } else {
        currentConversationId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        qDebug() << "New conversation created (no database):" << currentConversationId;
    }
}

void MainWindow::saveMessage(const QString &messageText, const QString &messageType)
{
    if (currentConversationId.isEmpty()) {
        if (messageType == "user") {
            createNewConversation(messageText);
        } else {
            return;
        }
    }

    // Save to JSON file
    saveConversationToJson(currentConversationId);

    // Also save to SQLite for metadata
    if (db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("INSERT INTO messages (conversation_id, message_text, message_type) VALUES (?, ?, ?)");
        query.addBindValue(currentConversationId);
        query.addBindValue(messageText);
        query.addBindValue(messageType);
        query.exec();

        // Update conversation metadata
        QSqlQuery updateQuery(db);
        updateQuery.prepare("UPDATE conversations SET last_activity = CURRENT_TIMESTAMP WHERE conversation_id = ?");
        updateQuery.addBindValue(currentConversationId);
        updateQuery.exec();

        // Update conversation name if needed
        if (messageType == "user") {
            for (auto &conv : conversations) {
                if (conv.conversationId == currentConversationId) {
                    if (conv.conversationName == "New Conversation") {
                        QString newName = generateConversationName(messageText);
                        updateConversationName(currentConversationId, newName);
                        conv.conversationName = newName;
                        refreshConversationList();
                    }
                    break;
                }
            }
        }
    }

    qDebug() << "Message saved:" << messageType << messageText.left(50);
}

void MainWindow::saveConversationToJson(const QString &conversationId)
{
    QString filePath = conversationsDir + "/" + conversationId + ".json";

    QJsonObject conversationObj;
    QJsonArray messagesArray;

    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        conversationObj = doc.object();
        messagesArray = conversationObj["messages"].toArray();
        file.close();
    }

    if (!conversationObj.contains("conversation_id")) {
        conversationObj["conversation_id"] = conversationId;
        conversationObj["created_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    }
    conversationObj["last_activity"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonArray newMessagesArray;

    for (int i = 0; i < ui->chatLayout->count(); ++i) {
        QLayoutItem* item = ui->chatLayout->itemAt(i);
        if (!item || !item->widget()) continue;

        QWidget* wrapper = item->widget();
        if (wrapper == welcomeLabel) continue;

        QList<QLabel*> labels = wrapper->findChildren<QLabel*>();
        for (QLabel* label : labels) {
            QString text = label->text();
            if (text.isEmpty() || text.length() < 3) continue;

            QString style = label->styleSheet();
            QString messageType = "bot";
            if (style.contains("#4a9eff") || style.contains("background-color: #4a9eff")) {
                messageType = "user";
            }

            if (text.contains(QRegularExpression("^\\d{2}:\\d{2}$"))) continue;

            QJsonObject messageObj;
            messageObj["text"] = text;
            messageObj["type"] = messageType;
            messageObj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

            newMessagesArray.append(messageObj);
        }
    }

    conversationObj["messages"] = newMessagesArray;

    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(conversationObj);
        file.write(doc.toJson());
        file.close();
        qDebug() << "Conversation saved to JSON:" << filePath;
    }
}

void MainWindow::loadConversationFromJson(const QString &conversationId)
{
    QString filePath = conversationsDir + "/" + conversationId + ".json";
    QFile file(filePath);

    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        qDebug() << "No JSON file found for conversation:" << conversationId;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject conversationObj = doc.object();
    QJsonArray messagesArray = conversationObj["messages"].toArray();

    if (messagesArray.isEmpty()) {
        qDebug() << "No messages found in conversation:" << conversationId;
        return;
    }

    hasStartedChatting = true;
    updateWelcomeLabelVisibility();

    for (const QJsonValue &messageVal : messagesArray) {
        QJsonObject messageObj = messageVal.toObject();
        QString text = messageObj["text"].toString();
        QString type = messageObj["type"].toString();

        if (text.isEmpty()) continue;

        if (type == "user") {
            addUserMessageFromHistory(text);
        } else {
            addBotMessageFromHistory(text);
        }
    }

    QTimer::singleShot(100, [this]() {
        ui->chatScrollArea->verticalScrollBar()->setValue(
            ui->chatScrollArea->verticalScrollBar()->maximum()
            );
    });

    qDebug() << "Loaded" << messagesArray.size() << "messages from JSON";
}

void MainWindow::loadConversation(const QString &conversationId)
{
    if (typingTimer->isActive()) {
        typingTimer->stop();
        if (typingLabel) {
            typingLabel->setText(pendingText);
            addTimeLabelToTypingMessage();
            saveMessage(pendingText, "bot");
            typingLabel = nullptr;
        }
        ui->sendButton->setText("SEND");
        ui->sendButton->setEnabled(true);
    }

    currentConversationId = conversationId;

    clearChatDisplay();

    waitingForNumberSelection = false;
    currentOptions.clear();
    lastIntentTag = "";

    loadConversationFromJson(conversationId);

    for (ConversationButton *button : conversationButtons) {
        button->setChecked(button->getConversationId() == conversationId);
    }

    qDebug() << "Conversation loaded:" << conversationId;
}
void MainWindow::loadAllConversations()
{
    conversations.clear();

    QSqlQuery query(db);
    if (query.exec("SELECT conversation_id, conversation_name, created_at, last_activity FROM conversations ORDER BY last_activity DESC")) {
        while (query.next()) {
            Conversation conv;
            conv.conversationId = query.value("conversation_id").toString();
            conv.conversationName = query.value("conversation_name").toString();
            conv.createdAt = query.value("created_at").toDateTime();
            conv.lastActivity = query.value("last_activity").toDateTime();

            conversations.append(conv);
        }

        refreshConversationList();
        qDebug() << "Loaded" << conversations.size() << "conversations";
    } else {
        qWarning() << "Failed to load conversations:" << query.lastError().text();
    }
}

void MainWindow::deleteConversation(const QString &conversationId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM conversations WHERE conversation_id = ?");
    query.addBindValue(conversationId);

    if (query.exec()) {
        conversations.removeIf([conversationId](const Conversation &conv) {
            return conv.conversationId == conversationId;
        });

        if (currentConversationId == conversationId) {
            if (!conversations.isEmpty()) {
                loadConversation(conversations.first().conversationId);
            } else {
                createNewConversation();
            }
        }

        refreshConversationList();
        qDebug() << "Conversation deleted:" << conversationId;
    } else {
        qWarning() << "Failed to delete conversation:" << query.lastError().text();
    }
}
void MainWindow::deleteCurrentConversation()
{
    if (!currentConversationId.isEmpty()) {
        if (QMessageBox::question(this, "Delete Conversation",
                                  "Are you sure you want to delete the current conversation?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            deleteConversation(currentConversationId);
        }
    }
}

void MainWindow::updateConversationName(const QString &conversationId, const QString &name)
{
    QSqlQuery query(db);
    query.prepare("UPDATE conversations SET conversation_name = ? WHERE conversation_id = ?");
    query.addBindValue(name);
    query.addBindValue(conversationId);
    query.exec();
}

void MainWindow::refreshConversationList()
{

    for (ConversationButton *button : conversationButtons) {
        historyLayout->removeWidget(button);
        button->deleteLater();
    }
    conversationButtons.clear();

    for (const Conversation &conv : conversations) {
        ConversationButton *button = new ConversationButton(conv.conversationId, conv.conversationName, this);
        connect(button, &QPushButton::clicked, this, &MainWindow::onConversationClicked);

        if (conv.conversationId == currentConversationId) {
            button->setChecked(true);
        }

        conversationButtons.append(button);
        historyLayout->addWidget(button);
    }
   deleteConversationButton->setEnabled(!conversations.isEmpty());
}

QString MainWindow::generateConversationName(const QString &firstMessage)
{
    QString name = firstMessage.trimmed();
    if (name.length() > 30) {
        name = name.left(27) + "...";
    }
    if (name.isEmpty()) {
        name = "New Conversation";
    }
    return name;
}

void MainWindow::onConversationClicked()
{
    ConversationButton *button = qobject_cast<ConversationButton*>(sender());
    if (button) {
        loadConversation(button->getConversationId());
    }
}

void MainWindow::clearChatDisplay()
{
    if (typingTimer->isActive()) {
        typingTimer->stop();
        if (typingLabel) {
            typingLabel->setText(pendingText);
            addTimeLabelToTypingMessage();
            saveMessage(pendingText, "bot");
            typingLabel = nullptr;
        }
        ui->sendButton->setText("SEND");
        ui->sendButton->setEnabled(true);
    }

    clearChatLayoutCompletely();

    hasStartedChatting = false;
    waitingForNumberSelection = false;
    currentOptions.clear();
    lastIntentTag = "";

    pendingText.clear();
    typedText.clear();
    currentCharIndex = 0;
    typingLabel = nullptr;

    welcomeLabel->show();
    ui->chatLayout->addStretch();
    ui->chatLayout->addWidget(welcomeLabel, 0, Qt::AlignCenter);
    ui->chatLayout->addStretch();

    QTimer::singleShot(50, [this]() {
        ui->chatScrollArea->verticalScrollBar()->setValue(0);
    });
}
void MainWindow::displayConversationMessages(const Conversation &conversation)
{
    for (const Message &message : conversation.messages) {
        if (message.messageType == "user") {
            addUserMessageFromHistory(message.messageText);
        } else {
            addBotMessageFromHistory(message.messageText);
        }
    }
}

void MainWindow::addUserMessageFromHistory(const QString &text)
{
    QLabel *messageLabel = new QLabel(text);
    messageLabel->setWordWrap(true);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    messageLabel->setAlignment(Qt::AlignLeft);
    messageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    messageLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Segoe UI', sans-serif;
            font-size: 15px;
            color: #ffffff;
            background-color: #4a9eff;
            border-radius: 15px;
            padding: 15px 20px;
            margin: 5px 5px 5px 50px;
        }
    )");

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(messageLabel);
    vLayout->setSpacing(2);
    vLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *container = new QWidget();
    container->setLayout(vLayout);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(container);

    QWidget *wrapper = new QWidget();
    wrapper->setLayout(hLayout);
    wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->chatLayout->addWidget(wrapper);
}

void MainWindow::addBotMessageFromHistory(const QString &text)
{
    QLabel *messageLabel = new QLabel(text);
    messageLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Segoe UI', sans-serif;
            font-size: 15px;
            color: #e0e0e0;
            background-color: #2d2d2d;
            border-radius: 15px;
            padding: 15px 20px;
            margin: 5px 50px 5px 5px;
        }
    )");
    messageLabel->setAlignment(Qt::AlignLeft);
    messageLabel->setWordWrap(true);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(messageLabel);
    vLayout->setSpacing(2);
    vLayout->setContentsMargins(0,0,0,0);

    QWidget* messageContainer = new QWidget();
    messageContainer->setLayout(vLayout);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(messageContainer);
    hLayout->addStretch();

    QWidget *wrapper = new QWidget();
    wrapper->setLayout(hLayout);
    wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->chatLayout->addWidget(wrapper);
}

void MainWindow::toggleSidebar()
{
    if (sidebarVisible) {
        sidebarAnimation->setStartValue(280);
        sidebarAnimation->setEndValue(0);
    } else {
        sidebarAnimation->setStartValue(0);
        sidebarAnimation->setEndValue(280);
    }

    sidebarAnimation->start();
    sidebarVisible = !sidebarVisible;
}

void MainWindow::updateWelcomeLabelVisibility()
{
    if (hasStartedChatting && welcomeLabel->isVisible()) {
        ui->chatLayout->removeWidget(welcomeLabel);
        welcomeLabel->hide();

        QLayoutItem* item;
        while ((item = ui->chatLayout->takeAt(0))) {
            delete item;
        }

        ui->chatLayout->setAlignment(Qt::AlignTop);
    }
}

QJsonDocument MainWindow::loadIntents(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError)
        return {};
    return doc;
}

QVector<Intent> MainWindow::parseIntents(const QJsonArray &intentsArray)
{
    QVector<Intent> intents;
    for (const QJsonValue &val : intentsArray)
    {
        if (!val.isObject())
            continue;
        QJsonObject obj = val.toObject();
        Intent intent;
        intent.tag = obj["intent"].toString();

        if (obj["patterns"].isArray()) {
            for (const QJsonValue &p : obj["patterns"].toArray()) {
                intent.patterns << p.toString();
            }
        }

        if (obj.contains("responses")) {
            if (obj["responses"].isArray()) {
                for (const QJsonValue &r : obj["responses"].toArray()) {
                    intent.responses << r.toString();
                }
            }
        } else if (obj.contains("response")) {
            intent.responses << obj["response"].toString();
        } else {
            intent.responses << "I don't have a response for that.";
        }

        intents.append(intent);
    }
    return intents;
}

bool MainWindow::isNumericInput(const QString &input)
{
    QString trimmed = input.trimmed();
    bool ok;
    int num = trimmed.toInt(&ok);
    return ok && num >= 1 && num <= 20;
}

const Intent *MainWindow::matchIntent(const QString &userInput)
{
    QString input = userInput.trimmed().toLower();
    qDebug() << "Matching intent for:" << input;

    if (waitingForNumberSelection && isNumericInput(input)) {
        for (const Intent &intent : intentList) {
            if (intent.tag == "number_selection") {
                return &intent;
            }
        }
    }

    input = input.remove(QRegularExpression("[.,!?;:]")).simplified();

    if (input.contains("marriage") || input.contains("bibaha") || input.contains("विवाह")) {
        for (const Intent &intent : intentList) {
            if (intent.tag == "certificate_services") {
                for (const QString &pattern : intent.patterns) {
                    if (pattern.toLower().contains("marriage") ||
                        pattern.toLower().contains("bibaha") ||
                        pattern.toLower().contains("विवाह")) {
                        return &intent;
                    }
                }
            }
        }
    }

    if (input.contains("divorce") || input.contains("talak") || input.contains("तलाक")) {
        for (const Intent &intent : intentList) {
            if (intent.tag == "certificate_services") {
                return &intent;
            }
        }
    }

    for (const Intent &intent : intentList) {
        for (const QString &pattern : intent.patterns) {
            if (input == pattern.toLower()) {
                qDebug() << "Exact match found:" << pattern << "-> intent:" << intent.tag;
                return &intent;
            }
        }
    }

    QStringList inputWords = input.split(' ', Qt::SkipEmptyParts);

    const Intent *bestMatch = nullptr;
    double bestScore = 0.0;

    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown") continue;

        for (const QString &pattern : intent.patterns) {
            QStringList patternWords = pattern.toLower().split(' ', Qt::SkipEmptyParts);
            double score = calculateMatchScore(inputWords, patternWords);

            if (score > bestScore && score >= 0.6) {
                bestScore = score;
                bestMatch = &intent;
                qDebug() << "Better word match:" << pattern << "score:" << score << "-> intent:" << intent.tag;
            }
        }
    }

    if (bestMatch) {
        return bestMatch;
    }

    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown") continue;

        for (const QString &pattern : intent.patterns) {
            QString lowerPattern = pattern.toLower();

            if (lowerPattern.length() > 3) {
                if (input.contains(lowerPattern) || lowerPattern.contains(input)) {
                    qDebug() << "Partial match found:" << lowerPattern << "-> intent:" << intent.tag;
                    return &intent;
                }
            }

            if (isServiceRelated(input, intent.tag)) {
                qDebug() << "Service-related match:" << intent.tag;
                return &intent;
            }
        }
    }

    if (input.length() > 3) {
        for (const Intent &intent : intentList) {
            if (intent.tag == "unknown") continue;

            for (const QString &pattern : intent.patterns) {
                if (pattern.length() > 3 && calculateSimilarity(input, pattern.toLower()) > 0.75) {
                    qDebug() << "Fuzzy match found:" << pattern << "-> intent:" << intent.tag;
                    return &intent;
                }
            }
        }
    }

    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown") {
            qDebug() << "No match found, returning unknown intent";
            return &intent;
        }
    }

    return nullptr;
}

double MainWindow::calculateMatchScore(const QStringList &inputWords, const QStringList &patternWords)
{
    if (inputWords.isEmpty() || patternWords.isEmpty()) return 0.0;

    int matches = 0;
    int totalWords = qMax(inputWords.size(), patternWords.size());

    for (const QString &inputWord : inputWords) {
        for (const QString &patternWord : patternWords) {
            if (inputWord.length() > 2 && patternWord.length() > 2) {
                if (inputWord == patternWord) {
                    matches++;
                    break;
                } else if (inputWord.contains(patternWord) || patternWord.contains(inputWord)) {
                    matches += 0.5;
                    break;
                }
            }
        }
    }

    return (double)matches / totalWords;
}

bool MainWindow::isServiceRelated(const QString &input, const QString &intentTag)
{
    QMap<QString, QStringList> serviceKeywords;
    serviceKeywords["citizenship_services"] = {"citizenship", "nagarikta", "citizen", "praman", "patra"};
    serviceKeywords["passport_services"] = {"passport", "travel", "document", "visa"};
    serviceKeywords["national_id_services"] = {"national", "id", "card", "identity"};
    serviceKeywords["certificate_services"] = {"birth", "death", "marriage", "certificate", "janma", "mrityu", "bibaha"};
    serviceKeywords["land_services"] = {"land", "registration", "birta", "raikar", "jagga"};
    serviceKeywords["vehicle_services"] = {"vehicle", "driving", "license", "gadi", "chalak"};
    serviceKeywords["business_services"] = {"business", "factory", "workshop", "vyavasaya"};

    if (serviceKeywords.contains(intentTag)) {
        for (const QString &keyword : serviceKeywords[intentTag]) {
            if (input.contains(keyword)) {
                return true;
            }
        }
    }

    return false;
}

double MainWindow::calculateSimilarity(const QString &str1, const QString &str2)
{
    int len1 = str1.length();
    int len2 = str2.length();

    if (len1 == 0) return len2 == 0 ? 1.0 : 0.0;
    if (len2 == 0) return 0.0;

    int commonChars = 0;
    int maxLen = qMax(len1, len2);

    for (int i = 0; i < qMin(len1, len2); ++i) {
        if (str1[i] == str2[i]) {
            commonChars++;
        }
    }

    for (int i = 0; i < len1 - 1; ++i) {
        QString substr = str1.mid(i, 2);
        if (str2.contains(substr)) {
            commonChars++;
        }
    }

    return (double)commonChars / maxLen;
}

void MainWindow::startTypingAnimation(const QString &text)
{
    if (typingTimer->isActive()) {
        typingTimer->stop();
        if (typingLabel) {
            typingLabel->setText(pendingText);
            addTimeLabelToTypingMessage();
            typingLabel = nullptr;
        }
    }

    pendingText = text;
    typedText.clear();
    currentCharIndex = 0;
    ui->sendButton->setText("STOP");
    ui->sendButton->setEnabled(true);

    typingLabel = new QLabel("");
    typingLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Segoe UI', sans-serif;
            font-size: 15px;
            color: #e0e0e0;
            background-color: #2d2d2d;
            border-radius: 15px;
            padding: 15px 20px;
            margin: 5px 50px 5px 5px;
        }
    )");
    typingLabel->setAlignment(Qt::AlignLeft);
    typingLabel->setWordWrap(true);
    typingLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    typingLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(typingLabel);
    vLayout->setSpacing(2);
    vLayout->setContentsMargins(0,0,0,0);

    QWidget* messageContainer = new QWidget();
    messageContainer->setLayout(vLayout);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(messageContainer);
    hLayout->addStretch();

    QWidget *wrapper = new QWidget();
    wrapper->setLayout(hLayout);
    wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->chatLayout->addWidget(wrapper);
    QCoreApplication::processEvents();
    ui->chatScrollArea->verticalScrollBar()->setValue(ui->chatScrollArea->verticalScrollBar()->maximum());
    typingTimer->start();
}

void MainWindow::addTimeLabelToTypingMessage()
{
    if (typingLabel) {
        QWidget* messageContainer = typingLabel->parentWidget();
        if (messageContainer) {
            QVBoxLayout* vLayout = qobject_cast<QVBoxLayout*>(messageContainer->layout());
            if (vLayout) {
                bool timeLabelExists = false;
                for (int i = 0; i < vLayout->count(); ++i) {
                    if (qobject_cast<QLabel*>(vLayout->itemAt(i)->widget()) &&
                        vLayout->itemAt(i)->widget() != typingLabel) {
                        timeLabelExists = true;
                        break;
                    }
                }

                if (!timeLabelExists) {
                    QLabel *timeLabel = new QLabel(QDateTime::currentDateTime().toString("HH:mm"));
                    timeLabel->setStyleSheet(R"(
                        QLabel {
                            font-family: 'Segoe UI', sans-serif;
                            font-size: 11px;
                            color: #888888;
                            background: transparent;
                            margin-left: 5px;
                        }
                    )");
                    timeLabel->setAlignment(Qt::AlignLeft);
                    vLayout->addWidget(timeLabel);
                }
            }
        }
    }
}

void MainWindow::onTypingTimeout()
{
    if (currentCharIndex < pendingText.length())
    {
        typedText += pendingText[currentCharIndex++];
        if (typingLabel) {
            typingLabel->setText(typedText);
            ui->chatScrollArea->verticalScrollBar()->setValue(ui->chatScrollArea->verticalScrollBar()->maximum());
        }
    }
    else
    {
        typingTimer->stop();
        ui->sendButton->setText("SEND");
        ui->sendButton->setEnabled(true);

        if (typingLabel) {
            addTimeLabelToTypingMessage();
        }

        saveMessage(pendingText, "bot");

        typingLabel = nullptr;

        pendingText.clear();
        typedText.clear();
        currentCharIndex = 0;
    }
}

QString MainWindow::generateNumberedServiceList(const QString &keyword, const QString &intentTag)
{
    if (!servicesDb.isOpen()) {
        return "I'm sorry, but I can't access the services database right now. Please try again later or contact the office directly.";
    }

    currentOptions.clear();
    QSqlQuery query(servicesDb);

    QString searchQuery;
    if (keyword.toLower().contains("marriage") || keyword.toLower().contains("bibaha")) {
        searchQuery = R"(
            SELECT
                s.service_id,
                s.service_name,
                o.office_name
            FROM services s
            JOIN offices o ON s.office_id = o.office_id
            WHERE LOWER(s.service_name) LIKE LOWER('%marriage%')
               OR LOWER(s.service_name) LIKE LOWER('%bibaha%')
            ORDER BY s.service_name
            LIMIT 5
        )";
        query.prepare(searchQuery);
    }
    else if (keyword.toLower().contains("divorce") || keyword.toLower().contains("talak")) {
        searchQuery = R"(
            SELECT
                s.service_id,
                s.service_name,
                o.office_name
            FROM services s
            JOIN offices o ON s.office_id = o.office_id
            WHERE LOWER(s.service_name) LIKE LOWER('%divorce%')
               OR LOWER(s.service_name) LIKE LOWER('%talak%')
            ORDER BY s.service_name
            LIMIT 5
        )";
        query.prepare(searchQuery);
    }
    else {
        query.prepare(R"(
            SELECT
                s.service_id,
                s.service_name,
                o.office_name
            FROM services s
            JOIN offices o ON s.office_id = o.office_id
            WHERE LOWER(s.service_name) LIKE LOWER(:keyword)
            ORDER BY s.service_name
            LIMIT 10
        )");
        query.bindValue(":keyword", "%" + keyword + "%");
    }

    if (query.exec() && query.next()) {
        QString response = QString("I found these services related to \"%1\":\n\n").arg(keyword);
        int index = 1;

        do {
            NumberedOption option;
            option.serviceId = query.value("service_id").toInt();
            option.serviceName = query.value("service_name").toString();
            option.displayText = query.value("service_name").toString();
            option.intentTag = intentTag;

            currentOptions.append(option);

            response += QString("%1. %2\n   Available at: %3\n\n")
                            .arg(index++)
                            .arg(option.serviceName)
                            .arg(query.value("office_name").toString());

        } while (query.next() && index <= 10);

        response += "Please type the number (1-" + QString::number(currentOptions.size()) + ") of the service you want detailed information about.";

        waitingForNumberSelection = true;
        return response;
    }

    return QString("I couldn't find any services related to \"%1\".\n\nTry asking about:\n• Citizenship Certificate\n• Passport services\n• National ID Card\n• Birth/Death/Marriage certificates\n• Land registration\n• Business licenses\n• Trekking permits\n\nOr ask me \"what services are available?\" to see all options!")
        .arg(keyword);
}

QString MainWindow::handleNumberSelection(int number, const QString &userInput)
{
    if (number < 1 || number > currentOptions.size()) {
        return QString("Please select a number between 1 and %1, or ask me a new question.").arg(currentOptions.size());
    }

    NumberedOption selectedOption = currentOptions[number - 1];

    QSqlQuery query(servicesDb);
    query.prepare(R"(
        SELECT
            s.service_name,
            o.office_name,
            m.ministry_name,
            s.service_no,
            rd.document_text as required_documents,
            s.charge,
            s.time_taken,
            s.contact_section,
            s.responsible_officer,
            s.grievance_section,
            s.monitoring_officer,
            s.remarks
        FROM services s
        JOIN offices o ON s.office_id = o.office_id
        JOIN ministries m ON o.ministry_id = m.ministry_id
        LEFT JOIN required_documents rd ON s.service_id = rd.service_id
        WHERE s.service_id = :service_id
    )");

    query.bindValue(":service_id", selectedOption.serviceId);

    if (query.exec() && query.next()) {
        waitingForNumberSelection = false;
        currentOptions.clear();
        return formatServiceResponse(query, "");
    }

    waitingForNumberSelection = false;
    currentOptions.clear();
    return "Sorry, I couldn't retrieve the details for that service. Please try again or ask me directly about the service.";
}

QString MainWindow::fetchServiceData(const QString &userInput, QString responseTemplate)
{
    if (!servicesDb.isOpen()) {
        return "I'm sorry, but I can't access the services database right now. Please try again later or contact the office directly.";
    }

    QSqlQuery query(servicesDb);

    query.prepare(R"(
        SELECT
            s.service_name,
            o.office_name,
            m.ministry_name,
            s.service_no,
            rd.document_text as required_documents,
            s.charge,
            s.time_taken,
            s.contact_section,
            s.responsible_officer,
            s.grievance_section,
            s.monitoring_officer,
            s.remarks
        FROM services s
        JOIN offices o ON s.office_id = o.office_id
        JOIN ministries m ON o.ministry_id = m.ministry_id
        LEFT JOIN required_documents rd ON s.service_id = rd.service_id
        WHERE LOWER(s.service_name) = LOWER(:service_name)
        LIMIT 1
    )");

    query.bindValue(":service_name", userInput.trimmed());

    if (query.exec() && query.next()) {
        return formatServiceResponse(query, responseTemplate);
    }

    QMap<QString, QStringList> intentToKeywords;
    intentToKeywords["citizenship_services"] = {"Citizenship Certificate", "citizenship", "nagarikta"};
    intentToKeywords["national_id_services"] = {"National ID Card", "national id", "id card"};
    intentToKeywords["passport_services"] = {"passport", "travel document"};
    intentToKeywords["visa_services"] = {"visa", "travel permit"};
    intentToKeywords["certificate_services"] = {"Certificate", "birth certificate", "death certificate", "marriage certificate"};
    intentToKeywords["land_services"] = {"Land", "land registration", "birta", "raikar"};
    intentToKeywords["vehicle_services"] = {"Vehicle", "driving license", "vehicle registration"};
    intentToKeywords["business_services"] = {"Business", "factory", "workshop"};
    intentToKeywords["trekking_permits"] = {"Trekking", "permit", "mountain"};
    intentToKeywords["labor_services"] = {"Labor", "labour", "employment"};

    QStringList searchKeywords = intentToKeywords.value(lastIntentTag, QStringList() << userInput);

    for (const QString &keyword : searchKeywords) {
        QString numberedList = generateNumberedServiceList(keyword, lastIntentTag);
        if (numberedList.contains("I found these services")) {
            return numberedList;
        }
    }

    return QString("I couldn't find specific information about \"%1\".\n\nTry asking about:\n• Citizenship Certificate\n• Passport services\n• National ID Card\n• Birth/Death/Marriage certificates\n• Land registration\n• Business licenses\n• Trekking permits\n\nOr ask me \"what services are available?\" to see all options!")
        .arg(userInput);
}

QString MainWindow::formatServiceResponse(QSqlQuery &query, QString responseTemplate)
{
    QString service_name = query.value("service_name").toString();
    QString office_name = query.value("office_name").toString();
    QString ministry_name = query.value("ministry_name").toString();
    QString service_no = query.value("service_no").toString();
    QString required_documents = query.value("required_documents").toString();
    QString charge = query.value("charge").toString();
    QString time_taken = query.value("time_taken").toString();
    QString contact_section = query.value("contact_section").toString();
    QString responsible_officer = query.value("responsible_officer").toString();
    QString grievance_section = query.value("grievance_section").toString();
    QString monitoring_officer = query.value("monitoring_officer").toString();
    QString remarks = query.value("remarks").toString();

    QString formattedResponse = QString("📋 %1\n\n").arg(service_name);

    formattedResponse += QString("🏢 Office: %1\n").arg(office_name);
    formattedResponse += QString("🏛️ Ministry: %1\n").arg(ministry_name);
    if (!service_no.isEmpty())
        formattedResponse += QString("🔢 Service No: %1\n").arg(service_no);

    formattedResponse += "\n📄 Required Documents:\n";
    if (!required_documents.isEmpty() && required_documents != "-" && required_documents != "—") {
        QStringList docs = required_documents.split(",");
        for (const QString &doc : docs) {
            QString trimmedDoc = doc.trimmed();
            if (!trimmedDoc.isEmpty()) {
                formattedResponse += QString("   • %1\n").arg(trimmedDoc);
            }
        }
    } else {
        formattedResponse += "   • Please contact the office for document requirements\n";
    }

    formattedResponse += "\n💰 Fee: ";
    if (!charge.isEmpty() && charge != "No charge" && charge != "Free" && charge != "-" && charge != "—") {
        formattedResponse += QString("%1\n").arg(charge);
    } else {
        formattedResponse += "Free of charge\n";
    }

    formattedResponse += QString("⏱️ Processing Time: %1\n").arg(time_taken.isEmpty() ? "Please contact office" : time_taken);

    if (!contact_section.isEmpty() && contact_section != "-" && contact_section != "—") {
        formattedResponse += QString("\n📞 Contact: %1\n").arg(contact_section);
    }

    if (!responsible_officer.isEmpty() && responsible_officer != "-" && responsible_officer != "—") {
        formattedResponse += QString("👤 Responsible Officer: %1\n").arg(responsible_officer);
    }

    if (!remarks.isEmpty() && remarks != "-" && remarks != "—") {
        formattedResponse += QString("\n📝 Additional Notes: %1\n").arg(remarks);
    }

    formattedResponse += "\n💡 Need more help? Ask me about office hours or other services!";

    lastIntentTag = "";
    return formattedResponse;
}

void MainWindow::addUserMessage(const QString &text)
{
    saveMessage(text, "user");

    QLabel *messageLabel = new QLabel(text);
    messageLabel->setWordWrap(true);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    messageLabel->setAlignment(Qt::AlignLeft);
    messageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    messageLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Segoe UI', sans-serif;
            font-size: 15px;
            color: #ffffff;
            background-color: #4a9eff;
            border-radius: 15px;
            padding: 15px 20px;
            margin: 5px 5px 5px 50px;
        }
    )");

    QLabel *timeLabel = new QLabel(QDateTime::currentDateTime().toString("HH:mm"));
    timeLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Segoe UI', sans-serif;
            font-size: 11px;
            color: #888888;
            background: transparent;
            margin-right: 5px;
        }
    )");
    timeLabel->setAlignment(Qt::AlignRight);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(messageLabel);
    vLayout->addWidget(timeLabel);
    vLayout->setSpacing(2);
    vLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *container = new QWidget();
    container->setLayout(vLayout);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addStretch();
    hLayout->addWidget(container);

    QWidget *wrapper = new QWidget();
    wrapper->setLayout(hLayout);
    wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    messageWidgets.append(wrapper);

    ui->chatLayout->addWidget(wrapper);

    QTimer::singleShot(50, [this]() {
        ui->chatScrollArea->verticalScrollBar()->setValue(
            ui->chatScrollArea->verticalScrollBar()->maximum()
            );
    });
}

void MainWindow::addBotMessage(const QString &text)
{
    startTypingAnimation(text);
}

QString MainWindow::getOfficeHoursResponse()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dayOfWeek = currentDateTime.toString("dddd");
    int hour = currentDateTime.time().hour();

    if (dayOfWeek == "Saturday") {
        return "🏢 The office is closed today (Saturday - holiday).\n\n⏰ Office Hours:\n• Monday-Thursday: 10:00 AM - 4:00 PM\n• Friday: 10:00 AM - 1:00 PM\n• Saturday: Closed (Holiday)\n\n📅 Plan your visit accordingly!";
    } else if (dayOfWeek == "Friday") {
        if (hour >= 10 && hour < 13) {
            return "✅ Perfect! The office is currently open!\n\n📅 Today (Friday): 10:00 AM - 1:00 PM\n🕐 Current time: Office is open\n\n⚠️ Remember: Friday has shorter hours than weekdays.";
        } else {
            return "❌ The office is currently closed.\n\n📅 Friday Hours: 10:00 AM - 1:00 PM only\n\n💡 Alternative:\n• Visit Monday-Thursday: 10:00 AM - 4:00 PM\n• Or come back next Friday during operating hours";
        }
    } else {
        if (hour >= 10 && hour < 16) {
            return "✅ Excellent! The office is currently open!\n\n📅 Today's Hours: 10:00 AM - 4:00 PM\n🟢 Status: Open and ready to serve you\n\n⭐ Weekdays offer the longest service hours.";
        } else {
            return "❌ The office is currently closed.\n\n⏰ Regular Hours:\n• Monday-Thursday: 10:00 AM - 4:00 PM\n• Friday: 10:00 AM - 1:00 PM\n• Saturday: Closed\n\n🚪 Please visit during operating hours for service.";
        }
    }
}

void MainWindow::handleUserInput(const QString &userText)
{
    QString input = userText.trimmed().toLower();
    if (input == "clear" || input == "cls") {
        if (typingTimer->isActive()) {
            typingTimer->stop();
            if (typingLabel) {
                typingLabel->setText(pendingText);
                addTimeLabelToTypingMessage();
                saveMessage(pendingText, "bot");
                typingLabel = nullptr;
            }
            ui->sendButton->setText("SEND");
            ui->sendButton->setEnabled(true);
        }

        clearChat();
        lastIntentTag = "";
        waitingForNumberSelection = false;
        currentOptions.clear();
        return;
    }
}

QString MainWindow::selectRandomResponse(const QStringList &responses)
{
    if (responses.isEmpty()) {
        return "I don't have a response for that.";
    }

    int randomIndex = QRandomGenerator::global()->bounded(responses.size());
    return responses.at(randomIndex);
}

void MainWindow::handleSendButtonClicked()
{
    QString userText = ui->inputLineEdit->text().trimmed();

    if (typingTimer->isActive()) {
        typingTimer->stop();
        if (typingLabel) {
            typingLabel->setText(pendingText);
            addTimeLabelToTypingMessage();
            // Save the bot message that was being typed
            saveMessage(pendingText, "bot");
            typingLabel = nullptr;
        }
        ui->sendButton->setText("SEND");
        ui->sendButton->setEnabled(true);
        return;
    }

    if (userText.isEmpty())
        return;

    handleUserInput(userText);
    if (userText.trimmed().toLower() == "clear" || userText.trimmed().toLower() == "cls") {
        ui->inputLineEdit->clear();
        return;
    }

    if (!hasStartedChatting) {
        hasStartedChatting = true;
        updateWelcomeLabelVisibility();
    }

    addUserMessage(userText);
    ui->inputLineEdit->clear();

    if (waitingForNumberSelection && isNumericInput(userText)) {
        bool ok;
        int number = userText.trimmed().toInt(&ok);
        if (ok) {
            QString response = handleNumberSelection(number, userText);
            addBotMessage(response);
            return;
        }
    }

    const Intent *matched = matchIntent(userText);
    QString response;
    if (matched)
    {
        if (matched->tag == "number_selection" && waitingForNumberSelection) {
            bool ok;
            int number = userText.trimmed().toInt(&ok);
            if (ok) {
                response = handleNumberSelection(number, userText);
            } else {
                response = "Please enter a valid number from the list above.";
            }
        } else {
            waitingForNumberSelection = false;
            currentOptions.clear();

            lastIntentTag = matched->tag;
            response = selectRandomResponse(matched->responses);

            if (matched->tag == "day_query") {
                QString currentDay = QDateTime::currentDateTime().toString("dddd");
                response.replace("{current_day}", currentDay);
                response.replace("{office_hours_response}", getOfficeHoursResponse());
            }
            else if (matched->tag == "office_hours") {
                response = getOfficeHoursResponse();
            }

            if (response.contains("{") && response.contains("}")) {
                response = fetchServiceData(userText, response);
            }

            response = generateContextualResponse(userText, matched, response);
        }
    }
    else
    {
        waitingForNumberSelection = false;
        currentOptions.clear();
        response = "I didn't quite understand that. 🤔\n\nTry asking about:\n• \"Citizenship certificate\"\n• \"Passport services\"\n• \"What day is today?\"\n• \"Office hours\"\n\nWhat would you like to know?";
        lastIntentTag = "";
    }
    addBotMessage(response);
}

const Intent* MainWindow::findIntentByTag(const QString &tag) const
{
    for (const Intent &intent : intentList) {
        if (intent.tag == tag) {
            return &intent;
        }
    }
    return nullptr;
}

QString MainWindow::generateContextualResponse(const QString &userInput, const Intent *intent, const QString &baseResponse)
{
    QString response = baseResponse;
    QString lowerInput = userInput.toLower();

    if (intent->tag.contains("service") || intent->tag.contains("_services")) {
        if (lowerInput.contains("urgent") || lowerInput.contains("tatkal") || lowerInput.contains("emergency") || lowerInput.contains("fast")) {
            response += "\n\n⚡ Express Service: Some services offer tatkal/express processing with additional fees. Please ask the office about expedited options.";
        }

        if (lowerInput.contains("online") || lowerInput.contains("internet") || lowerInput.contains("digital")) {
            response += "\n\n💻 Online Services: Many services now have online application options. Check the official government portal for digital services.";
        }

        if (lowerInput.contains("document") || lowerInput.contains("paper") || lowerInput.contains("kagaj")) {
            response += "\n\n📄 Document Tip: Always bring original documents along with photocopies. Some services may require notarized copies.";
        }

        if (lowerInput.contains("fee") || lowerInput.contains("cost") || lowerInput.contains("charge") || lowerInput.contains("paisa")) {
            response += "\n\n💰 Payment Tip: Fees may vary by district. Some services are free for certain categories (students, senior citizens).";
        }
    }

    if (intent->tag == "citizenship_services") {
        response += "\n\n💡 Pro Tip: Citizenship services are usually handled at District Administration Offices (DAO). Bring your parents' citizenship certificates.";
    }
    else if (intent->tag == "passport_services") {
        response += "\n\n✈️ Travel Tip: Apply for passport well in advance of your travel date. Check visa requirements for your destination country.";
    }
    else if (intent->tag == "national_id_services") {
        response += "\n\n🆔 ID Tip: National ID is mandatory for many services. Keep both physical and digital copies safe.";
    }

    return response;
}

void MainWindow::testDatabaseQueries()
{
    qDebug() << "=== Testing Database Connection ===";

    if (!db.isOpen()) {
        qDebug() << "Database is not open";
        return;
    }

    qDebug() << "Database is open";

    QSqlQuery query(db);

    if (query.exec("SELECT COUNT(*) FROM ministries")) {
        query.next();
        qDebug() << "Ministries table has" << query.value(0).toInt() << "records";
    } else {
        qDebug() << "Failed to query ministries table:" << query.lastError().text();
    }

    if (query.exec("SELECT COUNT(*) FROM offices")) {
        query.next();
        qDebug() << "Offices table has" << query.value(0).toInt() << "records";
    } else {
        qDebug() << "Failed to query offices table:" << query.lastError().text();
    }

    if (query.exec("SELECT COUNT(*) FROM services")) {
        query.next();
        qDebug() << "Services table has" << query.value(0).toInt() << "records";
    } else {
        qDebug() << "Failed to query services table:" << query.lastError().text();
    }

    if (query.exec("SELECT COUNT(*) FROM required_documents")) {
        query.next();
        qDebug() << "Required_documents table has" << query.value(0).toInt() << "records";
    } else {
        qDebug() << "Failed to query required_documents table:" << query.lastError().text();
    }

    qDebug() << "\n=== Testing Sample Services ===";
    query.prepare(R"(
        SELECT
            s.service_name,
            o.office_name,
            s.service_no
        FROM services s
        JOIN offices o ON s.office_id = o.office_id
        LIMIT 5
    )");

    if (query.exec()) {
        qDebug() << "JOIN query successful. Sample services:";
        while (query.next()) {
            qDebug() << "  •" << query.value("service_name").toString()
                << "at" << query.value("office_name").toString()
                << "(Service #" << query.value("service_no").toString() << ")";
        }
    } else {
        qDebug() << "JOIN query failed:" << query.lastError().text();
    }

    qDebug() << "\n=== Testing Citizenship Search ===";
    query.prepare(R"(
        SELECT
            s.service_name,
            o.office_name,
            s.service_no,
            rd.document_text,
            s.charge
        FROM services s
        JOIN offices o ON s.office_id = o.office_id
        LEFT JOIN required_documents rd ON s.service_id = rd.service_id
        WHERE LOWER(s.service_name) LIKE LOWER('%citizenship%')
        LIMIT 3
    )");

    if (query.exec()) {
        qDebug() << "Citizenship search successful:";
        while (query.next()) {
            qDebug() << "  • Service:" << query.value("service_name").toString();
            qDebug() << "    Office:" << query.value("office_name").toString();
            qDebug() << "    Fee:" << query.value("charge").toString();
            qDebug() << "    Documents:" << query.value("document_text").toString().left(100) + "...";
            qDebug() << "";
        }
    } else {
        qDebug() << "Citizenship search failed:" << query.lastError().text();
    }

    qDebug() << "=== Database Test Complete ===\n";
}

void MainWindow::resetTypingState()
{
    if (typingTimer->isActive()) {
        typingTimer->stop();
    }

    if (typingLabel) {
        typingLabel = nullptr;
    }

    pendingText.clear();
    typedText.clear();
    currentCharIndex = 0;

    ui->sendButton->setText("SEND");
    ui->sendButton->setEnabled(true);
}

void MainWindow::clearChat()
{
    if (typingTimer->isActive()) {
        typingTimer->stop();
        if (typingLabel) {
            typingLabel->setText(pendingText);
            addTimeLabelToTypingMessage();
            saveMessage(pendingText, "bot");
            typingLabel = nullptr;
        }
        ui->sendButton->setText("SEND");
        ui->sendButton->setEnabled(true);
    }

    clearChatLayoutCompletely();

    hasStartedChatting = false;
    lastIntentTag = "";
    waitingForNumberSelection = false;
    currentOptions.clear();
    pendingText.clear();
    typedText.clear();
    currentCharIndex = 0;

    welcomeLabel->show();
    ui->chatLayout->addStretch();
    ui->chatLayout->addWidget(welcomeLabel, 0, Qt::AlignCenter);
    ui->chatLayout->addStretch();

    QTimer::singleShot(50, [this]() {
        ui->chatScrollArea->verticalScrollBar()->setValue(0);
    });
}
void MainWindow::clearChatLayoutCompletely()
{
    if (welcomeLabel->parent()) {
        ui->chatLayout->removeWidget(welcomeLabel);
    }

    QLayoutItem* item;
    while ((item = ui->chatLayout->takeAt(0))) {
        if (item->widget()) {
            item->widget()->setParent(nullptr);
            item->widget()->deleteLater();
        }
        delete item;
    }

    messageWidgets.clear();

    QCoreApplication::processEvents();
}
