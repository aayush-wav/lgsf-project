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
#include <QDateTime>
#include <QUuid>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Intent {
    QString tag;
    QStringList patterns;
    QStringList responses;
};

struct NumberedOption {
    QString displayText;
    QString serviceName;
    int serviceId;
    QString intentTag;
};

struct Message {
    int messageId;
    QString conversationId;
    QString messageText;
    QString messageType;
    QDateTime timestamp;
};

struct Conversation {
    QString conversationId;
    QString conversationName;
    QDateTime createdAt;
    QDateTime lastActivity;
    QVector<Message> messages;
};

class ConversationButton : public QPushButton
{
    Q_OBJECT
public:
    ConversationButton(const QString &conversationId, const QString &text, QWidget *parent = nullptr);
    QString getConversationId() const { return m_conversationId; }

private:
    QString m_conversationId;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void deleteConversation(const QString &conversationId);

private slots:
    void onTypingTimeout();
    void handleSendButtonClicked();
    void toggleSidebar();
    void onConversationClicked();
    void deleteCurrentConversation();

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
    QSqlDatabase servicesDb;
    void resetTypingState();
    QString conversationsDir;
    void saveConversationToJson(const QString &conversationId);
    void loadConversationFromJson(const QString &conversationId);
    void clearChatLayoutCompletely();
    QList<QWidget*> messageWidgets;

    // UI components
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

    QString currentConversationId;
    QVector<Conversation> conversations;
    QVector<ConversationButton*> conversationButtons;
    QPushButton *newConversationButton;
    QPushButton *deleteConversationButton;

    void setupDatabase();
    void createDatabaseTables();
    bool testDatabaseConnection();

    void createNewConversation(const QString &firstMessage = "");
    void saveMessage(const QString &messageText, const QString &messageType);
    void loadConversation(const QString &conversationId);
    void loadAllConversations();
    void updateConversationName(const QString &conversationId, const QString &name);
    void refreshConversationList();
    QString generateConversationName(const QString &firstMessage);

    void setupUI();
    void setupSidebar();
    void setupChatArea();
    void updateWelcomeLabelVisibility();
    void clearChatDisplay();
    void displayConversationMessages(const Conversation &conversation);

    void addUserMessageFromHistory(const QString &text);
    void addBotMessageFromHistory(const QString &text);

    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    const Intent *matchIntent(const QString &userInput);
    const Intent *findIntentByTag(const QString &tag) const;
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
    void testDatabaseQueries();
    QString getOfficeHoursResponse();
    bool isNumericInput(const QString &input);
    QString selectRandomResponse(const QStringList &responses);
};

#endif
