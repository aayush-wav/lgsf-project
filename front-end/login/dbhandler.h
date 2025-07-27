#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <QtSql>
#include <QDebug>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSettings>

class DBHandler
{
public:
    static DBHandler& instance();

    bool connectToDB();
    QString hashPassword(const QString &password);
    bool createUser(const QString &username, const QString &password);
    bool authenticateUser(const QString &username, const QString &password);

private:
    DBHandler();
    DBHandler(const DBHandler&) = delete;
    void operator=(const DBHandler&) = delete;

    QSqlDatabase m_db;
    struct DatabaseConfig {
        QString hostName;
        QString databaseName;
        QString userName;
        QString password;
        int port;
    };
    DatabaseConfig dbConfig;
    bool loadDatabaseConfig();
};

#endif // DBHANDLER_H
