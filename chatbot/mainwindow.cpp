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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->inputLineEdit->setPlaceholderText("Ask ई - BADAPATRA");

    connect(ui->inputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::on_sendButton_clicked);

    qDebug() << "Current working directory:" << QDir::currentPath();

    QJsonDocument doc = loadIntents("D:/LGSF/back-end/json/responses.json");
    if (!doc.isNull()) {
        QJsonArray intentsArray = doc.array();
        intentList = parseIntents(intentsArray);
        qDebug() << "Loaded intents count:" << intentList.size();
    } else {
        qWarning() << "Failed to load intents.";
    }
}

QJsonDocument MainWindow::loadIntents(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open intent file:" << fileName;
        return {};
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error in" << fileName << ":" << parseError.errorString();
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
            if (input.contains(pattern.toLower())) {
                qDebug() << "Matched intent:" << intent.tag;
                return &intent;
            }
        }
    }

    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown") {
            qDebug() << "Returning 'unknown' intent";
            return &intent;
        }
    }

    return nullptr;
}

void MainWindow::startTypingAnimation(const QString &text)
{
    if (typingTimer) {
        typingTimer->stop();
        typingTimer->deleteLater();
    }

    pendingText = text;
    typedText.clear();
    currentCharIndex = 0;

    ui->responseTextEdit->append("<p style='text-align:left; color:white; margin: 5px 10px;' class='botReply'></p>");

    typingTimer = new QTimer(this);

    connect(typingTimer, &QTimer::timeout, this, [=]() mutable {
        if (currentCharIndex < pendingText.length()) {
            typedText += pendingText[currentCharIndex++];

            QTextCursor cursor = ui->responseTextEdit->textCursor();
            cursor.movePosition(QTextCursor::End);
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.removeSelectedText();
            cursor.deletePreviousChar();

            ui->responseTextEdit->append(
                "<p style='text-align:left; color:white; margin: 5px 10px;' class='botReply'>"
                + typedText.toHtmlEscaped() + "</p>");

            ui->responseTextEdit->verticalScrollBar()->setValue(ui->responseTextEdit->verticalScrollBar()->maximum());
        } else {
            typingTimer->stop();
            typingTimer->deleteLater();
        }
    });

    typingTimer->start(30);
}

void MainWindow::handleUserInput(const QString &userText)
{
    const Intent* matched = matchIntent(userText);

    ui->responseTextEdit->append(
        "<p style='text-align:right; color:white; margin: 5px 10px;'>"
        + userText.toHtmlEscaped() + "</p>");

    if (matched) {
        startTypingAnimation(matched->response);
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
}
