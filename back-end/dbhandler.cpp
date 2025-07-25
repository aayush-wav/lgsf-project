#include "dbhandler.h"

DBHandler& DBHandler::instance() {
    static DBHandler instance;
    return instance;
}

DBHandler::DBHandler(QObject *parent) : QObject(parent) {
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("localhost");       
    m_db.setDatabaseName("postgres");    
    m_db.setUserName("postgres");       
    m_db.setPassword("00618");  
    m_db.setPort(5432);                 
}

bool DBHandler::connectToDB() {
    if (!m_db.open()) {
        QMessageBox::critical(nullptr, "Error", "DB Connection Failed: " + m_db.lastError().text());
        return false;
    }
    return true;
}

QString DBHandler::hashPassword(const QString &password) {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

bool DBHandler::createUser(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password_hash) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));
    return query.exec();
}

bool DBHandler::authenticateUser(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT password_hash FROM users WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) {
        return query.value(0).toString() == hashPassword(password);
    }
    return false;
}