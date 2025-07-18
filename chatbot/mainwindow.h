#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Intent Struct outside the class
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

private:
    Ui::MainWindow *ui;

    QVector<Intent> intentList;

    QJsonDocument loadIntents(const QString &fileName);
    QVector<Intent> parseIntents(const QJsonArray &intentsArray);
    const Intent* matchIntent(const QString &userInput);
    void handleUserInput(const QString &userInput);
};

#endif // MAINWINDOW_H
