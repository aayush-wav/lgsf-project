// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:            // ← Add this section if missing
    void handleSend();

private:
    void addMessage(const QString &text, bool isUser);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
