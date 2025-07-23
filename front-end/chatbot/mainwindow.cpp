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
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentCharIndex(0),
    typingTimer(new QTimer(this)),
    typingLabel(nullptr)
{
    ui->setupUi(this);
    ui->inputLineEdit->setPlaceholderText("Ask ई - BADAPATRA anything");
    ui->sendButton->setEnabled(true);
    typingTimer->setInterval(30);
    connect(typingTimer, &QTimer::timeout, this, &MainWindow::onTypingTimeout);
    connect(ui->inputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::handleSendButtonClicked);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::handleSendButtonClicked);
    QJsonDocument doc = loadIntents("D:/LGSF/back-end/json/responses.json");
    if (!doc.isNull())
        intentList = parseIntents(doc.array());
    setupDatabase();
}

MainWindow::~MainWindow()
{
    if (db.isOpen())
        db.close();
    delete ui;
}

void MainWindow::setupDatabase()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("LgsfInfo");
    db.setUserName("postgres");
    db.setPassword("00618");
    if (!db.open())
        qWarning() << "Database connection failed:" << db.lastError().text();
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
        if (obj["patterns"].isArray())
            for (const QJsonValue &p : obj["patterns"].toArray())
                intent.patterns << p.toString();
        intent.response = obj["response"].toString();
        intents.append(intent);
    }
    return intents;
}

const Intent *MainWindow::matchIntent(const QString &userInput)
{
    QString input = userInput.trimmed().toLower();
    for (const Intent &intent : intentList)
        for (const QString &pattern : intent.patterns)
            if (input == pattern.toLower())
                return &intent;
    for (const Intent &intent : intentList)
        if (intent.tag == "unknown")
            return &intent;
    return nullptr;
}

void MainWindow::startTypingAnimation(const QString &text)
{
    if (typingLabel)
    {
        ui->chatLayout->removeWidget(typingLabel);
        typingLabel->deleteLater();
        typingLabel = nullptr;
    }
    pendingText = text;
    typedText.clear();
    currentCharIndex = 0;
    ui->sendButton->setEnabled(false);
    typingLabel = new QLabel("");
    typingLabel->setStyleSheet("background: transparent; color: rgb(180, 180, 180);");
    typingLabel->setAlignment(Qt::AlignLeft);
    typingLabel->setWordWrap(true);
    typingLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(typingLabel);
    layout->addStretch();
    QWidget *container = new QWidget();
    container->setLayout(layout);
    ui->chatLayout->addWidget(container);
    QScrollBar *bar = ui->chatScrollArea->verticalScrollBar();
    bar->setValue(bar->maximum());
    typingTimer->start();
}

void MainWindow::onTypingTimeout()
{
    if (currentCharIndex < pendingText.length())
    {
        typedText += pendingText[currentCharIndex++];
        typingLabel->setText(typedText.toHtmlEscaped());
        QScrollBar *bar = ui->chatScrollArea->verticalScrollBar();
        bar->setValue(bar->maximum());
    }
    else
    {
        typingTimer->stop();
        ui->sendButton->setEnabled(true);
        typingLabel = nullptr;
    }
}

QString MainWindow::fetchServiceData(const QString &serviceKeyword, QString responseTemplate)
{
    if (!db.isOpen())
        return "Database is not connected. Please check your settings.";
    QSqlQuery query(db);
    query.prepare("SELECT service_name, office_name, service_no, required_documents, charge FROM services WHERE LOWER(service_name) LIKE LOWER(:keyword) LIMIT 1");
    query.bindValue(":keyword", "%" + serviceKeyword + "%");
    if (!query.exec())
        return "Failed to fetch data.";
    if (query.next())
    {
        QString service_name = query.value("service_name").toString();
        QString office_name = query.value("office_name").toString();
        QString service_no = query.value("service_no").toString();
        QString required_documents = query.value("required_documents").toString();
        QString charge = query.value("charge").toString();
        responseTemplate.replace("{service_name}", service_name);
        responseTemplate.replace("{office_name}", office_name);
        responseTemplate.replace("{service_no}", service_no);
        responseTemplate.replace("{required_documents}", required_documents);
        responseTemplate.replace("{charge}", charge);
        return responseTemplate;
    }
    return "Service information not found.";
}

void MainWindow::addUserMessage(const QString &text)
{
    QLabel *label = new QLabel(text);
    label->setStyleSheet("background: transparent; color: rgb(220, 220, 220);");
    label->setAlignment(Qt::AlignRight);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addStretch();
    layout->addWidget(label);
    QWidget *container = new QWidget();
    container->setLayout(layout);
    ui->chatLayout->addWidget(container);
    QScrollBar *bar = ui->chatScrollArea->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::addBotMessage(const QString &text)
{
    QLabel *label = new QLabel(text);
    label->setStyleSheet("background: transparent; color: rgb(180, 180, 180);");
    label->setAlignment(Qt::AlignLeft);
    label->setWordWrap(true);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    layout->addStretch();
    QWidget *container = new QWidget();
    container->setLayout(layout);
    ui->chatLayout->addWidget(container);
    QScrollBar *bar = ui->chatScrollArea->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::handleUserInput(const QString &userText)
{
    addUserMessage(userText);
    const Intent *matched = matchIntent(userText);
    QString response;
    if (matched)
    {
        response = matched->response;
        if (response.contains("{") && response.contains("}"))
            response = fetchServiceData(userText, response);
    }
    else
    {
        response = "Sorry, I couldn't understand that.";
    }
    startTypingAnimation(response);
}

void MainWindow::handleSendButtonClicked()
{
    QString userText = ui->inputLineEdit->text().trimmed();
    if (userText.isEmpty())
        return;
    if (ui->badapatraLabel)
        ui->badapatraLabel->hide();
    handleUserInput(userText);
    ui->inputLineEdit->clear();
}
