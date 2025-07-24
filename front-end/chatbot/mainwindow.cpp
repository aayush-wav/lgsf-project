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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentCharIndex(0),
    typingTimer(new QTimer(this)),
    typingLabel(nullptr)
{
    ui->setupUi(this);

    QWidget* scrollWidget = new QWidget();
    ui->chatLayout = new QVBoxLayout(scrollWidget);
    ui->chatLayout->setAlignment(Qt::AlignTop);
    ui->chatLayout->setSpacing(10);
    ui->chatLayout->setContentsMargins(10, 10, 10, 10);

    ui->chatScrollArea->setWidget(scrollWidget);
    ui->chatScrollArea->setWidgetResizable(true);
    ui->chatScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    ui->inputLineEdit->setPlaceholderText("Ask ई - BADAPATRA anything");
    ui->sendButton->setText("SEND");
    ui->sendButton->setText("SEND");
    ui->sendButton->setStyleSheet(
        "QPushButton {"
        "   font-family: '0xNerdFont';"
        "   font-size: 14px;"
        "   color: #2196F3;"
        "   background-color: white;"
        "   border: 1px solid #2196F3;"
        "   border-radius: 4px;"
        "   padding: 5px 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #E3F2FD;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #BBDEFB;"
        "}"
        );
    ui->sendButton->setEnabled(true);
    typingTimer->setInterval(30);
    connect(typingTimer, &QTimer::timeout, this, &MainWindow::onTypingTimeout);
    connect(ui->inputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::handleSendButtonClicked);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::handleSendButtonClicked);


    QJsonDocument doc = loadIntents("D:/LGSF/lgsf-project/back-end/json/responses.json");
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
    
    input = input.remove(QRegularExpression("[.,!?;:]")).simplified();
    
    for (const Intent &intent : intentList) {
        for (const QString &pattern : intent.patterns) {
            if (input == pattern.toLower()) {
                qDebug() << "Exact match:" << pattern << "-> intent:" << intent.tag;
                return &intent;
            }
        }
    }
    
    QStringList inputWords = input.split(' ', Qt::SkipEmptyParts);
    
    const Intent *bestMatch = nullptr;
    int maxMatches = 0;
    
    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown") continue; 
        
        for (const QString &pattern : intent.patterns) {
            QStringList patternWords = pattern.toLower().split(' ', Qt::SkipEmptyParts);
            int matchCount = 0;
            
            for (const QString &inputWord : inputWords) {
                for (const QString &patternWord : patternWords) {
                    if (inputWord.length() > 2 && patternWord.length() > 2) {
                        if (inputWord == patternWord) {
                            matchCount++;
                            break; 
                        }
                    }
                }
            }
            
            if (matchCount > 0 && matchCount > maxMatches) {
                maxMatches = matchCount;
                bestMatch = &intent;
                qDebug() << "Better word match:" << pattern << "matches:" << matchCount << "-> intent:" << intent.tag;
            }
        }
    }
    
    if (bestMatch && maxMatches >= 1) {
        return bestMatch;
    }
    
    for (const Intent &intent : intentList) {
        if (intent.tag == "unknown") continue;
        
        for (const QString &pattern : intent.patterns) {
            QString lowerPattern = pattern.toLower();
            
            if (lowerPattern.length() > 3) {
                if (input.contains(lowerPattern)) {
                    qDebug() << "Partial match (input contains pattern):" << lowerPattern << "-> intent:" << intent.tag;
                    return &intent;
                }
                if (lowerPattern.contains(input) && input.length() > 3) {
                    qDebug() << "Partial match (pattern contains input):" << lowerPattern << "-> intent:" << intent.tag;
                    return &intent;
                }
            }
        }
    }
    
    if (input.length() > 3) {
        for (const Intent &intent : intentList) {
            if (intent.tag == "unknown") continue;
            
            for (const QString &pattern : intent.patterns) {
                if (pattern.length() > 3 && calculateSimilarity(input, pattern.toLower()) > 0.8) {
                    qDebug() << "Fuzzy match:" << pattern << "-> intent:" << intent.tag;
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
    typingLabel->setStyleSheet("font-family: '0xNerdFont'; font-size: 18px; color: rgb(180, 180, 180); background: transparent; padding: 8px;");
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
                    timeLabel->setStyleSheet("font-family: '0xNerdFont'; font-size: 10px; color: rgb(150, 150, 150); background: transparent;");
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
        typingLabel->setText(typedText);
        ui->chatScrollArea->verticalScrollBar()->setValue(ui->chatScrollArea->verticalScrollBar()->maximum());
    }
    else
    {
        typingTimer->stop();
        ui->sendButton->setText("SEND");
        ui->sendButton->setText("SEND");
        ui->sendButton->setEnabled(true);
        addTimeLabelToTypingMessage();
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
    QLabel *messageLabel = new QLabel(text);
    messageLabel->setWordWrap(true);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    messageLabel->setAlignment(Qt::AlignLeft);
    messageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    messageLabel->setMaximumWidth(ui->chatScrollArea->width() * 0.7);
    messageLabel->setStyleSheet("font-family: '0xNerdFont'; font-size: 18px; color: rgb(220, 220, 220); background: transparent; padding: 8px;");

    QLabel *timeLabel = new QLabel(QDateTime::currentDateTime().toString("HH:mm"));
    timeLabel->setStyleSheet("font-family: '0xNerdFont'; font-size: 10px; color: rgb(150, 150, 150); background: transparent;");
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
    hLayout->setContentsMargins(10, 5, 10, 5);

    QWidget *wrapper = new QWidget();
    wrapper->setLayout(hLayout);
    wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    ui->chatLayout->addWidget(wrapper);
    ui->chatScrollArea->verticalScrollBar()->setValue(ui->chatScrollArea->verticalScrollBar()->maximum());
}

void MainWindow::addBotMessage(const QString &text)
{
}

void MainWindow::handleUserInput(const QString &userText)
{
    QString input = userText.trimmed().toLower();
    if (input == "clear" || input == "cls") {
        clearChat();
        return;
    }

    if (ui->badapatraLabel) {
        ui->badapatraLabel->hide();
    }

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

void MainWindow::clearChat()
{
    QLayoutItem* item;
    while ((item = ui->chatLayout->takeAt(0))) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    if (ui->badapatraLabel) {
        ui->badapatraLabel->show();
    }
}

void MainWindow::handleSendButtonClicked()
{
    QString userText = ui->inputLineEdit->text().trimmed();

    if (typingTimer->isActive()) {
        typingTimer->stop();
        if (typingLabel) {
            typingLabel->setText(pendingText);
            addTimeLabelToTypingMessage();
            typingLabel = nullptr;
        }
        ui->sendButton->setText("SEND");
        ui->sendButton->setText("SEND");
        ui->sendButton->setEnabled(true);
        return;
    }

    if (userText.isEmpty())
        return;

    if (ui->badapatraLabel)
        ui->badapatraLabel->hide();

    handleUserInput(userText);
    ui->inputLineEdit->clear();
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

QString MainWindow::generateContextualResponse(const QString &userInput, const Intent *intent)
{
    QString response = intent->response;

    if (intent->tag == "service_info") {
        if (userInput.contains("urgent") || userInput.contains("tatkal") || userInput.contains("emergency")) {
            response += "\n\n**Note:** For urgent processing, tatkal services may be available with additional fees.";
        }
        
        if (userInput.contains("online") || userInput.contains("internet")) {
            response += "\n\n**Tip:** Check if this service is available online for faster processing.";
        }
    }
    
    return response;
}
