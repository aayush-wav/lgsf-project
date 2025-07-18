#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "Current working directory:" << QDir::currentPath();

    QJsonDocument doc = loadIntents("LGSF/back-end/responses.json");
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

    // If no match found, return the "unknown" intent
    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown") {
            qDebug() << "Returning 'unknown' intent";
            return &intent;
        }
    }

    return nullptr;
}

void MainWindow::on_sendButton_clicked()
{
    QString userText = ui->inputLineEdit->text();

    if (userText.isEmpty()) return;

    const Intent* matched = matchIntent(userText);

    ui->responseTextEdit->append(userText);

    if (matched) {
        ui->responseTextEdit->append(matched->response);
    } else {
        ui->responseTextEdit->append("Bot: Sorry, I couldn't understand that.");
    }

    ui->inputLineEdit->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}
