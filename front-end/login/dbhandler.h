#ifndef DBHANDLER_H
#define DBHANDLER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>

class DBHandler {
public:
    static DBHandler& instance();

    bool connectToDB();
    bool createUser(const QString &username, const QString &passwordHash);
    bool usernameExists(const QString &username);
    bool authenticateUser(const QString &username, const QString &password);
    QString hashPassword(const QString &password);  // THIS was missing before

private:
    DBHandler();
    QSqlDatabase db;
};

#endif // DBHANDLER_H
