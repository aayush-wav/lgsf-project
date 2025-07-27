#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include <QMessageBox>
#include <QCryptographicHash>

struct DatabaseConfig {
    QString hostName;
    QString databaseName;
    QString userName;
    QString password;
    int port;
};

class DBHandler {
public:
    static DBHandler& instance();
    bool connectToDB();
    bool createUser(const QString &username, const QString &password);
    bool authenticateUser(const QString &username, const QString &password);
    bool usernameExists(const QString &username);
    QString hashPassword(const QString &password);

private:
    DBHandler();
    bool loadDatabaseConfig();
    QSqlDatabase m_db;
    DatabaseConfig dbConfig;
};

#endif // DBHANDLER_H