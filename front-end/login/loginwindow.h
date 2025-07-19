#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QLineEdit>
#include <QAction>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QProcess>
#include <QResizeEvent>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_pushButton_Login_clicked();
    void on_loginBtn_clicked();
    void on_labelBadapatra_linkActivated(const QString &link);
    void on_usernameLineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_labelNewUser_linkActivated(const QString &link);
    void onLoginReplyFinished();

private:
    Ui::LoginWindow *ui;
    QAction *togglePasswordAction;
    bool passwordVisible;

    // Network manager for HTTP requests
    QNetworkAccessManager *networkManager;

    // Backend configuration
    struct BackendConfig {
        QString serverUrl;
        int port;
        int timeoutMs;
    };
    BackendConfig backendConfig;

    // Authentication methods (now using HTTP)
    bool initializeBackend();
    bool loadBackendConfig();
    void performLogin(const QString &username, const QString &password);
    bool checkServerConnection();

    // UI methods
    void setupUI();
    void togglePasswordVisibility();
    void makeResponsive();
    void updateLayout();
    void showLoading(bool show);

    // Navigation methods
    void openSignupPage();
    void openMainUI();

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

    // Loading state
    bool isLoggingIn;
};

#endif // MAINWINDOW_H
