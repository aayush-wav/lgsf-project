#include "dbhandler.h"

DBHandler& DBHandler::instance() {
    static DBHandler instance;
    return instance;
}

DBHandler::DBHandler() {
    m_db = QSqlDatabase::addDatabase("QPSQL", "DBHandlerConnection");
    loadDatabaseConfig();
}

bool DBHandler::loadDatabaseConfig() {
    QSettings settings("database/db_config.ini", QSettings::IniFormat);

    if (settings.contains("Database/hostName")) {
        dbConfig.hostName = settings.value("Database/hostName").toString();
        dbConfig.databaseName = settings.value("Database/databaseName").toString();
        dbConfig.userName = settings.value("Database/userName").toString();
        dbConfig.password = settings.value("Database/password").toString();
        dbConfig.port = settings.value("Database/port", 5432).toInt();
    } else {
        dbConfig.hostName = "localhost";
        dbConfig.databaseName = "UserInfo";
        dbConfig.userName = "postgres";
        dbConfig.password = "00618";
        dbConfig.port = 5432;

        settings.setValue("Database/hostName", dbConfig.hostName);
        settings.setValue("Database/databaseName", dbConfig.databaseName);
        settings.setValue("Database/userName", dbConfig.userName);
        settings.setValue("Database/password", dbConfig.password);
        settings.setValue("Database/port", dbConfig.port);
        settings.sync();

        qDebug() << "Created default database configuration file at: database/db_config.ini";
        qDebug() << "Please update the configuration with your actual database credentials.";
    }

    return true;
}

bool DBHandler::connectToDB() {
    m_db.setHostName(dbConfig.hostName);
    m_db.setDatabaseName(dbConfig.databaseName);
    m_db.setUserName(dbConfig.userName);
    m_db.setPassword(dbConfig.password);
    m_db.setPort(dbConfig.port);

    if (!m_db.open()) {
        QMessageBox::critical(nullptr, "Error", "DB Connection Failed: " + m_db.lastError().text());
        return false;
    }

    QSqlQuery query(m_db);
    QString createUsersTable = "CREATE TABLE IF NOT EXISTS users ("
                              "user_id SERIAL PRIMARY KEY, "
                              "username text NOT NULL UNIQUE, "
                              "password_hash text NOT NULL)";
    if (!query.exec(createUsersTable)) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Could not create users table: " + query.lastError().text());
        return false;
    }

    qDebug() << "Connected to PostgreSQL database successfully";
    return true;
}

QString DBHandler::hashPassword(const QString &password) {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

bool DBHandler::createUser(const QString &username, const QString &password) {
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (username, password_hash) VALUES (:username, :password_hash)");
    query.bindValue(":username", username);
    query.bindValue(":password_hash", hashPassword(password));
    if (!query.exec()) {
        qDebug() << "Create user error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DBHandler::authenticateUser(const QString &username, const QString &password) {
    QSqlQuery query(m_db);
    query.prepare("SELECT password_hash FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        return query.value(0).toString() == hashPassword(password);
    }
    qDebug() << "Authentication error:" << query.lastError().text();
    return false;
}

bool DBHandler::usernameExists(const QString &username) {
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    qDebug() << "Username check error:" << query.lastError().text();
    return false;
}