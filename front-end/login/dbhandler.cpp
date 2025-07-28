#include "dbhandler.h"
#include <QDebug>
#include <QCryptographicHash>

DBHandler::DBHandler() {}

DBHandler& DBHandler::instance()
{
    static DBHandler instance;
    return instance;
}

bool DBHandler::connectToDB()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QPSQL");
    }

    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("lgsf");
    db.setUserName("postgres");
    db.setPassword("tosinnp");  // Change this!

    if (!db.open()) {
        qDebug() << "DB Connection Failed:" << db.lastError().text();
        return false;
    }

    qDebug() << "DB Connection Successful!";
    return true;
}

bool DBHandler::createUser(const QString &username, const QString &passwordHash)
{
    if (usernameExists(username))
        return false;

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", passwordHash);

    if (!query.exec()) {
        qDebug() << "Failed to insert user:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DBHandler::usernameExists(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Failed to check username:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

bool DBHandler::authenticateUser(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Failed to fetch password:" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        QString storedHash = query.value(0).toString();
        return storedHash == hashPassword(password);
    }

    return false;
}

QString DBHandler::hashPassword(const QString &password)
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}
