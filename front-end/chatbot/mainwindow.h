//mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlDatabase>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Intent {
    QString tag;
    QStringList patterns;
    QString response;
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

private:
    Ui::MainWindow *ui;
    int currentCharIndex;
    QTimer *typingTimer;
    QLabel *typingLabel;
    QString pendingText;
    QString typedText;
    QVector<Intent> intentList;
    QSqlDatabase db;

    void setupDatabase();
    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    const Intent *matchIntent(const QString &userInput);
    void startTypingAnimation(const QString &text);
    void addTimeLabelToTypingMessage();
    QString fetchServiceData(const QString &serviceKeyword, QString responseTemplate);
    void addUserMessage(const QString &text);
    void addBotMessage(const QString &text);
    void handleUserInput(const QString &userText);
    void clearChat();
};

#endif