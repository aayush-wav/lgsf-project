#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QLineEdit>
#include <QAction>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QProcess>
#include <QResizeEvent>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QFile>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButton_Login_clicked();
    void on_loginBtn_clicked();
    void on_labelBadapatra_linkActivated(const QString &link);
    void on_usernameLineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_labelNewUser_linkActivated(const QString &link);

private:
    Ui::MainWindow *ui;
    QAction *togglePasswordAction;
    bool passwordVisible;
    QSqlDatabase database;

    // Database configuration
    struct DatabaseConfig {
        QString hostName;
        QString databaseName;
        QString userName;
        QString password;
        int port;
    };
    DatabaseConfig dbConfig;

    // Database and authentication methods
    bool initializeDatabase();
    bool loadDatabaseConfig();
    bool connectToDatabase();
    bool createTablesIfNotExist();
    QString hashPassword(const QString &password);
    bool verifyLogin(const QString &username, const QString &password);

    // UI methods
    void setupUI();
    void togglePasswordVisibility();
    void makeResponsive();
    void updateLayout();

    // Navigation methods
    void openSignupPage();
    void openMainUI();

    void setupKeyboardNavigation();
    void setupValidation();
    void validateFields();
    void updateFieldStyle(QLineEdit *field, bool isValid);

    // Original widget positions and sizes for responsiveness
    struct WidgetGeometry {
        QRect loginBtn;
        QRect passwordLineEdit;
        QRect usernameLineEdit;
        QRect labelBadapatra;
        QRect labelNewUser;
    };
    WidgetGeometry originalGeometry;
    QSize originalWindowSize;
};

#endif // MAINWINDOW_H
