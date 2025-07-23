#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Intent
{
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

    QString pendingText;
    QString typedText;
    int currentCharIndex;
    QTimer *typingTimer;

    void setupDatabase();
    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    const Intent *matchIntent(const QString &userInput);
    QString fetchServiceData(const QString &serviceKeyword, QString responseTemplate);

    void startTypingAnimation(const QString &text);

private slots:
    void onTypingTimeout();
    void handleUserInput(const QString &userText);
    void handleSendButtonClicked();
};

#endif // MAINWINDOW_H
