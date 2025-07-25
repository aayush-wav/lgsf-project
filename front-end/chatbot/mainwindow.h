#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QLabel>

namespace Ui {
class MainWindow;
}

struct Intent {
    QString tag;
    QStringList patterns;
    QStringList responses;
};

struct NumberedOption {
    int serviceId;
    QString serviceName;
    QString displayText;
    QString intentTag;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTypingTimeout();
    void handleSendButtonClicked();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QVector<Intent> intentList;
    QVector<NumberedOption> currentOptions;
    int currentCharIndex;
    QTimer *typingTimer;
    QLabel *typingLabel;
    QString pendingText;
    QString typedText;
    QString lastIntentTag;
    bool waitingForNumberSelection;

    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    QString getRandomResponse(const Intent &intent);
    bool isNumericInput(const QString &input);
    const Intent *matchIntent(const QString &userInput);
    double calculateMatchScore(const QStringList &inputWords, const QStringList &patternWords);
    bool isServiceRelated(const QString &input, const QString &intentTag);
    void startTypingAnimation(const QString &text);
    void addTimeLabelToTypingMessage();
    QString generateNumberedServiceList(const QString &keyword, const QString &intentTag);
    QString handleNumberSelection(int number, const QString &userInput);
    QString fetchServiceData(const QString &userInput, QString responseTemplate);
    QString formatServiceResponse(QSqlQuery &query, QString responseTemplate);
    void addUserMessage(const QString &text);
    void addBotMessage(const QString &text);
    QString getOfficeHoursResponse();
    void handleUserInput(const QString &userText);
    double calculateSimilarity(const QString &str1, const QString &str2);
    const Intent* findIntentByTag(const QString &tag);
    QString generateContextualResponse(const QString &userInput, const Intent *intent, const QString &baseResponse);
    void clearChat();
    void setupDatabase();
};

#endif
