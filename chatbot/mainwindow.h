#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>

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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_sendButton_clicked();
    void onTypingTimeout();

private:
    Ui::MainWindow *ui;
    QVector<Intent> intentList;

    QString pendingText;
    QString typedText;
    int currentCharIndex = 0;
    QTimer *typingTimer = nullptr;

    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    const Intent* matchIntent(const QString &userInput);
    void startTypingAnimation(const QString &text);
    void handleUserInput(const QString &userInput);
};

#endif // MAINWINDOW_H
