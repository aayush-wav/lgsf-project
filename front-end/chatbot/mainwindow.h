#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>
#include <QTimer>
#include <QSqlDatabase>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Intent {
    QString tag;
    QStringList patterns;
    QString response;
};

class QLabel;

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

    int currentCharIndex;
    QString pendingText;
    QString typedText;
    QTimer *typingTimer;

    QLabel *typingLabel;

    void setupDatabase();
    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    const Intent* matchIntent(const QString &userInput);
    QString fetchServiceData(const QString &serviceKeyword, QString responseTemplate);

    void addUserMessage(const QString &text);
    void addBotMessage(const QString &text);
    void startTypingAnimation(const QString &text);



private slots:
    void onTypingTimeout();
    void handleSendButtonClicked();
    void handleUserInput(const QString &userText);
};

#endif
