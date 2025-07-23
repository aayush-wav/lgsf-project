#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QTextCursor>
#include <QScrollBar>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
     : QMainWindow(parent), ui(new Ui::MainWindow), currentCharIndex(0), typingTimer(new QTimer(this))

{
    ui->setupUi(this);

    ui->inputLineEdit->setPlaceholderText("Ask ई - BADAPATRA anything");
    ui->responseTextEdit->setReadOnly(true);
    ui->responseTextEdit->setStyleSheet("background-color: #1C1C1C; color: rgba(255, 255, 255, 0.9); border: none;");
    ui->responseTextEdit->setLineWrapMode(QTextEdit::WidgetWidth);

    typingTimer->setInterval(30);
    connect(typingTimer, &QTimer::timeout, this, &MainWindow::onTypingTimeout);
    connect(ui->inputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::handleSendButtonClicked);

    QJsonDocument doc = loadIntents("D:/LGSF/back-end/json/responses.json");
    if (!doc.isNull()) {
        intentList = parseIntents(doc.array());
    }

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
    if (!db.open()) {
        qWarning() << "Database connection failed:" << db.lastError().text();
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
    for (const QJsonValue &val : intentsArray) {
        if (!val.isObject()) continue;

        QJsonObject obj = val.toObject();
        Intent intent;
        intent.tag = obj["intent"].toString();

        if (obj["patterns"].isArray()) {
            for (const QJsonValue &p : obj["patterns"].toArray())
                intent.patterns << p.toString();
        }

        intent.response = obj["response"].toString();
        intents.append(intent);
    }
    return intents;
}

const Intent* MainWindow::matchIntent(const QString &userInput)
{
    QString input = userInput.trimmed().toLower();

    for (const Intent &intent : intentList) {
        for (const QString &pattern : intent.patterns) {
            if (input == pattern.toLower())
                return &intent;
        }
    }

    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown")
            return &intent;
    }
    return nullptr;
}

void MainWindow::startTypingAnimation(const QString &text)
{
    pendingText = text;
    typedText.clear();
    currentCharIndex = 0;
    ui->sendButton->setEnabled(false);
    ui->responseTextEdit->append("<p style='text-align:left; color: rgba(255,255,255,0.8); margin:5px 10px;'></p>");
    typingTimer->start();
}

void MainWindow::onTypingTimeout()
{
    if (currentCharIndex < pendingText.length()) {
        typedText += pendingText[currentCharIndex++];

        QTextCursor cursor = ui->responseTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        cursor.movePosition(QTextCursor::PreviousBlock);
        cursor.select(QTextCursor::BlockUnderCursor);

        cursor.removeSelectedText();
        cursor.insertHtml("<p style='text-align:left; color: rgba(255,255,255,0.8); margin:5px 10px;'>" + typedText.toHtmlEscaped() + "</p>");
        ui->responseTextEdit->verticalScrollBar()->setValue(ui->responseTextEdit->verticalScrollBar()->maximum());
    } else {
        typingTimer->stop();
        ui->sendButton->setEnabled(true);
    }
}

QString MainWindow::fetchServiceData(const QString &serviceKeyword, QString responseTemplate)
{
    if (!db.isOpen()) {
        return "Database is not connected. Please check your settings.";
    }

    QSqlQuery query(db);
    query.prepare("SELECT service_name, office_name, service_no, required_documents, charge FROM services WHERE LOWER(service_name) LIKE LOWER(:keyword) LIMIT 1");
    query.bindValue(":keyword", "%" + serviceKeyword + "%");
    if (!query.exec()) {
        return "Failed to fetch data.";
    }

    if (query.next()) {
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

void MainWindow::handleUserInput(const QString &userText)
{
    ui->responseTextEdit->append("<p style='text-align:right; color: rgba(255,255,255,0.9); margin: 5px 10px;'>" + userText.toHtmlEscaped() + "</p>");

    const Intent* matched = matchIntent(userText);

    if (matched) {
        QString response = matched->response;
        if (response.contains("{") && response.contains("}")) {
            response = fetchServiceData(userText, response);
        }
        startTypingAnimation(response);
    } else {
        startTypingAnimation("Sorry, I couldn't understand that.");
    }
}

void MainWindow::handleSendButtonClicked()
{
    QString userText = ui->inputLineEdit->text().trimmed();
    if (userText.isEmpty()) return;

    if (ui->badapatraLabel) ui->badapatraLabel->hide();

    handleUserInput(userText);
    ui->inputLineEdit->clear();
}
