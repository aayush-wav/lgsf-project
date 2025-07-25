#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QtSql>
#include <QDebug>

class DatabaseManager
{
public:
    static DatabaseManager& instance()
    {
        static DatabaseManager instance;
        return instance;
    }

    bool connect()
    {
        db = QSqlDatabase::addDatabase("QPSQL");
        db.setHostName("localhost");
        db.setDatabaseName("YourDatabaseName");
        db.setUserName("your_db_user");
        db.setPassword("your_db_password");
        db.setPort(5432);

        if (!db.open()) {
            qDebug() << "DB Connection Error:" << db.lastError().text();
            return false;
        }
        qDebug() << "Database connected successfully.";
        return true;
    }

    QSqlDatabase getDB()
    {
        return db;
    }

private:
    DatabaseManager() {}
    DatabaseManager(const DatabaseManager&) = delete;
    void operator=(const DatabaseManager&) = delete;

    QSqlDatabase db;
};

#endif // DATABASE_MANAGER_H
