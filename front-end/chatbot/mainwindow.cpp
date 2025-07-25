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
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentCharIndex(0),
    typingTimer(new QTimer(this)),
    typingLabel(nullptr),
    lastIntentTag(""),
    waitingForNumberSelection(false)
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

    QJsonDocument doc = loadIntents("D:/LGSF/back-end/json/responses.json");
    if (!doc.isNull()) {
        intentList = parseIntents(doc.array());
        qDebug() << "Loaded" << intentList.size() << "intents";
    } else {
        qDebug() << "Failed to load intents";
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
    db.setPort(5432);
    db.setDatabaseName("LgsfInfo");
    db.setUserName("postgres");
    db.setPassword("00618");

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", QString("Failed to connect to database:\n%1").arg(db.lastError().text()));
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
        if (obj["responses"].isArray()) {
            for (const QJsonValue &r : obj["responses"].toArray())
                intent.responses << r.toString();
        } else {
            intent.responses << obj["response"].toString();
        }
        intents.append(intent);
    }
    return intents;
}

QString MainWindow::getRandomResponse(const Intent &intent) {
    if (intent.responses.isEmpty()) return "";
    int index = QRandomGenerator::global()->bounded(intent.responses.size());
    return intent.responses.at(index);
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

    // First check if we're waiting for number selection
    if (waitingForNumberSelection && isNumericInput(input)) {
        for (const Intent &intent : intentList) {
            if (intent.tag == "number_selection") {
                return &intent;
            }
        }
    }

    // Very simple pattern matching - just check if any pattern is contained in the input
    for (const Intent &intent : intentList) {
        for (const QString &pattern : intent.patterns) {
            QString lowerPattern = pattern.toLower();
            if (input.contains(lowerPattern)) {
                qDebug() << "Matched pattern:" << pattern << "for intent:" << intent.tag;
                return &intent;
            }
        }
    }

    // If no match found, return unknown intent
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
        ui->sendButton->setEnabled(true);
        addTimeLabelToTypingMessage();
        typingLabel = nullptr;
    }
}

QString MainWindow::generateNumberedServiceList(const QString &keyword, const QString &intentTag)
{
    if (!db.isOpen()) {
        return "I'm sorry, but I can't access the database right now. Please try again later or contact the office directly.";
    }

    currentOptions.clear();
    QSqlQuery query(db);

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

    return QString("I couldn't find any services related to \"%1\".").arg(keyword);
}

QString MainWindow::handleNumberSelection(int number, const QString &userInput)
{
    if (number < 1 || number > currentOptions.size()) {
        return QString("Please select a number between 1 and %1, or ask me a new question.").arg(currentOptions.size());
    }

    NumberedOption selectedOption = currentOptions[number - 1];

    QSqlQuery query(db);
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
    if (!db.isOpen()) {
        return "I'm sorry, but I can't access the database right now. Please try again later or contact the office directly.";
    }

    QSqlQuery query(db);

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

    return QString("I couldn't find specific information about \"%1\".").arg(userInput);
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

    QString formattedResponse = QString("%1\n\n").arg(service_name);

    formattedResponse += QString("Office: %1\n").arg(office_name);
    formattedResponse += QString("Ministry: %1\n").arg(ministry_name);
    if (!service_no.isEmpty())
        formattedResponse += QString("Service No: %1\n").arg(service_no);

    formattedResponse += "\nRequired Documents:\n";
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

    formattedResponse += "\nFee: ";
    if (!charge.isEmpty() && charge != "No charge" && charge != "Free" && charge != "-" && charge != "—") {
        formattedResponse += QString("%1\n").arg(charge);
    } else {
        formattedResponse += "Free of charge\n";
    }

    formattedResponse += QString("Processing Time: %1\n").arg(time_taken.isEmpty() ? "Please contact office" : time_taken);

    if (!contact_section.isEmpty() && contact_section != "-" && contact_section != "—") {
        formattedResponse += QString("\nContact: %1\n").arg(contact_section);
    }

    if (!responsible_officer.isEmpty() && responsible_officer != "-" && responsible_officer != "—") {
        formattedResponse += QString("Responsible Officer: %1\n").arg(responsible_officer);
    }

    if (!remarks.isEmpty() && remarks != "-" && remarks != "—") {
        formattedResponse += QString("\nAdditional Notes: %1\n").arg(remarks);
    }

    formattedResponse += "\nNeed more help? Ask me about office hours or other services!";

    lastIntentTag = "";
    return formattedResponse;
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
    startTypingAnimation(text);
}

QString MainWindow::getOfficeHoursResponse()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString dayOfWeek = currentDateTime.toString("dddd");
    int hour = currentDateTime.time().hour();

    if (dayOfWeek == "Saturday") {
        return "The office is closed today (Saturday - holiday).";
    } else if (dayOfWeek == "Friday") {
        if (hour >= 10 && hour < 13) {
            return "Perfect! The office is currently open!";
        } else {
            return "The office is currently closed.";
        }
    } else {
        if (hour >= 10 && hour < 16) {
            return "Excellent! The office is currently open!";
        } else {
            return "The office is currently closed.";
        }
    }
}

void MainWindow::handleUserInput(const QString &userText)
{
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
        ui->sendButton->setEnabled(true);
        return;
    }

    if (userText.isEmpty())
        return;

    if (ui->badapatraLabel && ui->badapatraLabel->isVisible()) {
        ui->badapatraLabel->hide();
    }

    if (userText.toLower() == "clear" || userText.toLower() == "cls") {
        clearChat();
        ui->inputLineEdit->clear();
        lastIntentTag = "";
        waitingForNumberSelection = false;
        currentOptions.clear();
        return;
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
            response = getRandomResponse(*matched);

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
        }
    }
    else
    {
        waitingForNumberSelection = false;
        currentOptions.clear();
        const Intent *unknownIntent = findIntentByTag("unknown");
        if (unknownIntent) {
            response = getRandomResponse(*unknownIntent);
        } else {
            response = "I didn't quite understand that.";
        }
        lastIntentTag = "";
    }
    addBotMessage(response);
}

const Intent* MainWindow::findIntentByTag(const QString &tag) {
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
            response += "\n\nExpress Service available.";
        }

        if (lowerInput.contains("online") || lowerInput.contains("internet") || lowerInput.contains("digital")) {
            response += "\n\nOnline Services available.";
        }
    }

    return response;
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
    if (ui->badapatraLabel && !ui->badapatraLabel->isVisible()) {
        ui->badapatraLabel->show();
    }
}
