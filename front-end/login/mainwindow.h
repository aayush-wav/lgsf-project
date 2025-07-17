#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_pushButton_Login_clicked();

private:
    Ui::MainWindow *ui;

    QLineEdit *passwordLineEdit;
    QAction *togglePasswordAction;
    bool passwordVisible = false;

    void setupUI();
    void togglePasswordVisibility();
};

#endif // MAINWINDOW_H
