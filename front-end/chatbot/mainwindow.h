#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Intent {
    QString tag;
    QStringList patterns;
    QString response;
};

struct NumberedOption {
    QString displayText;
    QString serviceName;
    int serviceId;
    QString intentTag;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTypingTimeout();
    void handleSendButtonClicked();
    void toggleSidebar();

private:
    Ui::MainWindow *ui;
    int currentCharIndex;
    QTimer *typingTimer;
    QLabel *typingLabel;
    QString pendingText;
    QString typedText;
    QVector<Intent> intentList;
    QSqlDatabase db;
    QString lastIntentTag;
    QVector<NumberedOption> currentOptions;
    bool waitingForNumberSelection;
    bool hasStartedChatting;

    // UI Components
    QWidget *centralWidget;
    QHBoxLayout *mainLayout;
    QFrame *sidebar;
    QWidget *chatArea;
    QVBoxLayout *chatAreaLayout;
    QFrame *headerFrame;
    QHBoxLayout *headerLayout;
    QPushButton *menuButton;
    QLabel *headerLabel;
    QLabel *welcomeLabel;
    QScrollArea *historyScrollArea;
    QWidget *historyWidget;
    QVBoxLayout *historyLayout;
    QPropertyAnimation *sidebarAnimation;
    bool sidebarVisible;

    void setupUI();
    void setupSidebar();
    void setupChatArea();
    void setupDatabase();
    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    const Intent *matchIntent(const QString &userInput);
    void startTypingAnimation(const QString &text);
    void addTimeLabelToTypingMessage();
    QString fetchServiceData(const QString &serviceKeyword, QString responseTemplate);
    QString formatServiceResponse(QSqlQuery &query, QString responseTemplate);
    QString generateNumberedServiceList(const QString &keyword, const QString &intentTag);
    QString handleNumberSelection(int number, const QString &userInput);
    void addUserMessage(const QString &text);
    void addBotMessage(const QString &text);
    void handleUserInput(const QString &userText);
    void clearChat();
    double calculateSimilarity(const QString &str1, const QString &str2);
    double calculateMatchScore(const QStringList &inputWords, const QStringList &patternWords);
    bool isServiceRelated(const QString &input, const QString &intentTag);
    QString generateContextualResponse(const QString &userInput, const Intent *intent, const QString &baseResponse);
    void testDatabaseConnection();
    QString getOfficeHoursResponse();
    bool isNumericInput(const QString &input);
    void updateWelcomeLabelVisibility();
};

#endif