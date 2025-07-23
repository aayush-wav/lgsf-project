#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QSqlDatabase>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Intent {
    QString tag;
    QVector<QString> patterns;
    QString response;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVector<Intent> intentList;
    QSqlDatabase db;
    QTimer *typingTimer;
    int currentCharIndex;
    QString pendingText;
    QString typedText;

    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    const Intent* matchIntent(const QString &userInput);
    QString fetchServiceData(const QString &serviceKeyword, QString responseTemplate);
    void setupDatabase();

    void addUserMessage(const QString &text);
    void addBotMessage(const QString &text);

private slots:
    void onTypingTimeout();
    void handleSendButtonClicked();
    void handleUserInput(const QString &userText);
};

#endif
