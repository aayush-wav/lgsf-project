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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentCharIndex(0),
    typingTimer(new QTimer(this)),
    typingLabel(nullptr),
    lastIntentTag("")
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
    db.setPort(5432);
    db.setDatabaseName("LgsfInfo");
    db.setUserName("postgres");
    db.setPassword("00618");
    
    if (!db.open()) {
        qWarning() << "Database connection failed:" << db.lastError().text();
        qWarning() << "Driver error:" << db.lastError().driverText();
        qWarning() << "Database error:" << db.lastError().databaseText();
        
        QMessageBox::critical(this, "Database Error", 
                            QString("Failed to connect to database:\n%1\n\nPlease ensure:\n"
                                   "1. PostgreSQL service is running\n"
                                   "2. Database 'LgsfInfo' exists\n"
                                   "3. Username/password are correct")
                                   .arg(db.lastError().text()));
    } else {
        qDebug() << "Database connected successfully!";
        
        QSqlQuery testQuery(db);
        if (testQuery.exec("SELECT 1")) {
            qDebug() << "Database test query successful";
        } else {
            qWarning() << "Database test query failed:" << testQuery.lastError().text();
        }
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

QString MainWindow::fetchServiceData(const QString &userInput, QString responseTemplate)
{
    if (!db.isOpen()) {
        return "I'm sorry, but I can't access the database right now. Please try again later or contact the office directly.";
    }
    
    QSqlQuery query(db);
    
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

    QStringList searchKeywords = intentToKeywords.value(lastIntentTag, QStringList() << userInput);
    
    for (const QString &keyword : searchKeywords) {
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
            WHERE LOWER(s.service_name) LIKE LOWER(:keyword)
            ORDER BY s.service_name
        )");
        
        query.bindValue(":keyword", "%" + keyword + "%");
        
        if (query.exec() && query.next()) {
            QStringList serviceNames;
            int index = 1;
            do {
                QString service_name = query.value("service_name").toString();
                QString office_name = query.value("office_name").toString();
                serviceNames << QString("   %1. %2\n      Available at: %3").arg(index++).arg(service_name).arg(office_name);
            } while (query.next() && index <= 10); 
            
            QString results = QString("I found these services related to your request:\n\n%1\n\nPlease type the exact service name from above to get detailed information, or ask me something like:\n• \"How to get [service name]?\"\n• \"What documents needed for [service name]?\"\n• \"[service name] fee\"")
                             .arg(serviceNames.join("\n\n"));
            return results;
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
    

    QString formattedResponse = QString(" **%1**\n\n").arg(service_name);
    
    formattedResponse += QString(" **Office:** %1\n").arg(office_name);
    formattedResponse += QString(" **Ministry:** %1\n").arg(ministry_name);
    if (!service_no.isEmpty())
        formattedResponse += QString("**Service No:** %1\n").arg(service_no);
    
    formattedResponse += "\n **Required Documents:**\n";
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
    
    formattedResponse += "\n**Fee:** ";
    if (!charge.isEmpty() && charge != "No charge" && charge != "Free" && charge != "-" && charge != "—") {
        formattedResponse += QString("%1\n").arg(charge);
    } else {
        formattedResponse += "Free of charge\n";
    }
    
    formattedResponse += QString("**Processing Time:** %1\n").arg(time_taken.isEmpty() ? "Please contact office" : time_taken);
    
    if (!contact_section.isEmpty() && contact_section != "-" && contact_section != "—") {
        formattedResponse += QString("\n **Contact:** %1\n").arg(contact_section);
    }
    
    if (!responsible_officer.isEmpty() && responsible_officer != "-" && responsible_officer != "—") {
        formattedResponse += QString("**Responsible Officer:** %1\n").arg(responsible_officer);
    }
    
    if (!remarks.isEmpty() && remarks != "-" && remarks != "—") {
        formattedResponse += QString("\n **Additional Notes:** %1\n").arg(remarks);
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
        return "The office is closed today (Saturday - holiday). \n\n **Office Hours:**\n• Monday-Thursday: 10:00 AM - 4:00 PM\n• Friday: 10:00 AM - 1:00 PM\n• Saturday: Closed (Holiday)\n\n Plan your visit accordingly!";
    } else if (dayOfWeek == "Friday") {
        if (hour >= 10 && hour < 13) {
            return "Perfect! The office is currently open!\n\n **Today (Friday):** 10:00 AM - 1:00 PM\n **Current time:** Office is open\n\n Remember: Friday has shorter hours than weekdays.";
        } else {
            return "The office is currently closed.\n\n **Friday Hours:** 10:00 AM - 1:00 PM only\n\n**Alternative:**\n• Visit Monday-Thursday: 10:00 AM - 4:00 PM\n• Or come back next Friday during operating hours";
        }
    } else {
        if (hour >= 10 && hour < 16) {
            return "Excellent! The office is currently open!\n\n **Today's Hours:** 10:00 AM - 4:00 PM\n **Status:** Open and ready to serve you\n\nWeekdays offer the longest service hours.";
        } else {
            return "The office is currently closed.\n\n **Regular Hours:**\n• Monday-Thursday: 10:00 AM - 4:00 PM\n• Friday: 10:00 AM - 1:00 PM\n• Saturday: Closed\n\n Please visit during operating hours for service.";
        }
    }
}

void MainWindow::handleUserInput(const QString &userText)
{
    QString input = userText.trimmed().toLower();
    if (input == "clear" || input == "cls") {
        clearChat();
        lastIntentTag = "";
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
        lastIntentTag = matched->tag;
        response = matched->response;
        
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
    else
    {
        response = " I didn't quite understand that. Could you try asking about:\n\n• Government services (citizenship, passport, etc.)\n• Office hours (\"what day is today?\")\n• Specific service information\n\nWhat would you like to know?";
        lastIntentTag = "";
    }
    addBotMessage(response);
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

QString MainWindow::generateContextualResponse(const QString &userInput, const Intent *intent, const QString &baseResponse)
{
    QString response = baseResponse;
    QString lowerInput = userInput.toLower();

    if (intent->tag.contains("service") || intent->tag.contains("_services")) {
        if (lowerInput.contains("urgent") || lowerInput.contains("tatkal") || lowerInput.contains("emergency") || lowerInput.contains("fast")) {
            response += "\n\n **Express Service:** Some services offer tatkal/express processing with additional fees. Please ask the office about expedited options.";
        }
        
        if (lowerInput.contains("online") || lowerInput.contains("internet") || lowerInput.contains("digital")) {
            response += "\n\n **Online Services:** Many services now have online application options. Check the official government portal for digital services.";
        }
        
        if (lowerInput.contains("document") || lowerInput.contains("paper") || lowerInput.contains("kagaj")) {
            response += "\n\n**Document Tip:** Always bring original documents along with photocopies. Some services may require notarized copies.";
        }
        
        if (lowerInput.contains("fee") || lowerInput.contains("cost") || lowerInput.contains("charge") || lowerInput.contains("paisa")) {
            response += "\n\n **Payment Tip:** Fees may vary by district. Some services are free for certain categories (students, senior citizens).";
        }
    }
    
    if (intent->tag == "citizenship_services") {
        response += "\n\n **Pro Tip:** Citizenship services are usually handled at District Administration Offices (DAO). Bring your parents' citizenship certificates.";
    }
    else if (intent->tag == "passport_services") {
        response += "\n\n **Travel Tip:** Apply for passport well in advance of your travel date. Check visa requirements for your destination country.";
    }
    else if (intent->tag == "national_id_services") {
        response += "\n\n **ID Tip:** National ID is mandatory for many services. Keep both physical and digital copies safe.";
    }
    
    return response;
}

void MainWindow::testDatabaseConnection()
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
