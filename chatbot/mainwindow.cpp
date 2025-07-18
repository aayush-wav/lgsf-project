// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QTextCursor>
#include <QScrollBar>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->inputLineEdit->setPlaceholderText("Ask ई - BADAPATRA anything");
    ui->responseTextEdit->setReadOnly(true);
    ui->responseTextEdit->setStyleSheet("background-color: #1C1C1C; color: rgba(255, 255, 255, 0.9); border: none;");
    ui->responseTextEdit->setLineWrapMode(QTextEdit::WidgetWidth);

    typingTimer = new QTimer(this);
    typingTimer->setInterval(30);
    connect(typingTimer, &QTimer::timeout, this, &MainWindow::onTypingTimeout);
    connect(ui->inputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::on_sendButton_clicked);

    QJsonDocument doc = loadIntents("D:/LGSF/back-end/json/responses.json");
    if (!doc.isNull()) {
        QJsonArray intentsArray = doc.array();
        intentList = parseIntents(intentsArray);
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:/LGSF/back-end/database/service_data.db");
    if (!db.open()) {
        qDebug() << "Failed to open DB:" << db.lastError().text();
    }
}

QJsonDocument MainWindow::loadIntents(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return {};
    }
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
            if (input == pattern.toLower()) {
                return &intent;
            }
        }
    }
    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown") {
            return &intent;
        }
    }
    return nullptr;
}

QString MainWindow::fetchServiceData(const QString &serviceName)
{
    QSqlQuery query;
    query.prepare("SELECT office_name, service_no, required_documents, charge FROM services WHERE LOWER(service_name) = LOWER(:serviceName)");
    query.bindValue(":serviceName", serviceName);
    if (!query.exec()) {
        qDebug() << "Query error:" << query.lastError().text();
        return "";
    }
    if (query.next()) {
        QString officeName = query.value(0).toString();
        QString serviceNo = query.value(1).toString();
        QString reqDocs = query.value(2).toString();
        QString charge = query.value(3).toString();
        return QString("To apply for the service <b>%1</b>, please visit <b>%2</b>, Office No. <b>%3</b>.<br><br><b>Required Documents:</b><br>%4<br><br><b>Fee:</b> %5")
            .arg(serviceName, officeName, serviceNo, reqDocs, charge);
    }
    return "";
}

void MainWindow::startTypingAnimation(const QString &text)
{
    pendingText = text;
    typedText.clear();
    currentCharIndex = 0;
    ui->responseTextEdit->append("<p style='text-align:left; color: rgba(255, 255, 255, 0.8); margin: 5px 10px;'></p>");
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
        cursor.insertHtml("<p style='text-align:left; color: rgba(255, 255, 255, 0.8); margin: 5px 10px;'>"
                          + typedText.toHtmlEscaped() + "</p>");
        ui->responseTextEdit->verticalScrollBar()->setValue(ui->responseTextEdit->verticalScrollBar()->maximum());
    } else {
        typingTimer->stop();
    }
}

void MainWindow::handleUserInput(const QString &userText)
{
    ui->responseTextEdit->append("<p style='text-align:right; color: rgba(255, 255, 255, 0.9); margin: 5px 10px;'>"
                                 + userText.toHtmlEscaped() + "</p>");

    const Intent* matched = matchIntent(userText);

    if (matched) {
        if (matched->tag == "service_info") {
            QString response = fetchServiceData(userText);
            if (!response.isEmpty())
                startTypingAnimation(response);
            else
                startTypingAnimation("Sorry, I don't have details on that specific service currently.");
        } else {
            startTypingAnimation(matched->response);
        }
    } else {
        startTypingAnimation("Sorry, I couldn't understand that.");
    }
}

void MainWindow::on_sendButton_clicked()
{
    QString userText = ui->inputLineEdit->text();
    if (userText.isEmpty()) return;
    if (ui->badapatraLabel) ui->badapatraLabel->hide();
    handleUserInput(userText);
    ui->inputLineEdit->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(db.isOpen()) db.close();
}
