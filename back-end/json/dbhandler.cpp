#include "dbhandler.h"

DBHandler& DBHandler::instance() {
    static DBHandler instance;
    return instance;
}

DBHandler::DBHandler(QObject *parent) : QObject(parent) {
    QSettings settings("config/database.ini", QSettings::IniFormat);
    
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName(settings.value("Database/hostname", "localhost").toString());
    m_db.setDatabaseName(settings.value("Database/dbname", "postgres").toString());
    m_db.setUserName(settings.value("Database/username", "postgres").toString());
    m_db.setPassword(settings.value("Database/password", "").toString());
    m_db.setPort(settings.value("Database/port", 5432).toInt());
}

bool DBHandler::connectToDB() {
    if (!m_db.open()) {
        QMessageBox::critical(nullptr, "Error", "DB Connection Failed: " + m_db.lastError().text());
        return false;
    }
    return true;
}

QString DBHandler::hashPassword(const QString &password) {
    // Generate a random salt
    QByteArray salt = QCryptographicHash::hash(
        QDateTime::currentDateTime().toString().toUtf8(),
        QCryptographicHash::Sha256
    );
    
    // Combine password with salt
    QByteArray saltedPass = password.toUtf8() + salt;
    
    // Return salt:hash format
    return salt.toHex() + ':' + QCryptographicHash::hash(saltedPass, QCryptographicHash::Sha256).toHex();
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
    try {
        query.prepare("SELECT password_hash FROM users WHERE username = ?");
        query.addBindValue(username);
        
        if (!query.exec()) {
            qDebug() << "Authentication query failed:" << query.lastError().text();
            return false;
        }
        
        if (query.next()) {
            QString storedHash = query.value(0).toString();
            QStringList hashParts = storedHash.split(':');
            if (hashParts.size() != 2) return false;
            
            QByteArray salt = QByteArray::fromHex(hashParts[0].toLatin1());
            QByteArray saltedPass = password.toUtf8() + salt;
            QString calculatedHash = salt.toHex() + ':' + 
                QCryptographicHash::hash(saltedPass, QCryptographicHash::Sha256).toHex();
            
            return (storedHash == calculatedHash);
        }
        return false;
    } catch (const std::exception &e) {
        qDebug() << "Exception in authenticateUser:" << e.what();
        return false;
    }
}