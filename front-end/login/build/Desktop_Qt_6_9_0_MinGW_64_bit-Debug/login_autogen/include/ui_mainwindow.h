/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *loginBtn;
    QLineEdit *passwordLineEdit;
    QLineEdit *usernameLineEdit;
    QLabel *labelBadapatra;
    QLabel *labelNewUser;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1123, 651);
        QFont font;
        font.setFamilies({QString::fromUtf8("montserrat")});
        font.setPointSize(16);
        font.setKerning(false);
        MainWindow->setFont(font);
        MainWindow->setAcceptDrops(true);
        MainWindow->setAutoFillBackground(false);
        MainWindow->setStyleSheet(QString::fromUtf8("*{\n"
"        background-image: url(:/res/background.png);\n"
"        background-position: center;\n"
"        background-size:100% 100%;\n"
"        font-family: montserrat;\n"
"        background-color:  #1C1C1C;\n"
"}\n"
""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setStyleSheet(QString::fromUtf8("background-image: url(:/res/background.png);\n"
"background-position: center;\n"
"background-size: 100% 100%;\n"
"background-repeat: no-repeat;\n"
"background: transparent;\n"
""));
        loginBtn = new QPushButton(centralwidget);
        loginBtn->setObjectName("loginBtn");
        loginBtn->setGeometry(QRect(410, 443, 300, 45));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("montserrat")});
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setKerning(false);
        loginBtn->setFont(font1);
        loginBtn->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);\n"
"border-radius: 4;\n"
"color: #2148C0;"));
        passwordLineEdit = new QLineEdit(centralwidget);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setGeometry(QRect(410, 355, 300, 45));
        usernameLineEdit = new QLineEdit(centralwidget);
        usernameLineEdit->setObjectName("usernameLineEdit");
        usernameLineEdit->setGeometry(QRect(410, 290, 300, 45));
        labelBadapatra = new QLabel(centralwidget);
        labelBadapatra->setObjectName("labelBadapatra");
        labelBadapatra->setGeometry(QRect(390, 168, 345, 111));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("montserrat")});
        font2.setPointSize(22);
        font2.setBold(true);
        font2.setKerning(false);
        labelBadapatra->setFont(font2);
        labelBadapatra->setStyleSheet(QString::fromUtf8("text-alignment: center;"));
        labelBadapatra->setAlignment(Qt::AlignmentFlag::AlignCenter);
        labelNewUser = new QLabel(centralwidget);
        labelNewUser->setObjectName("labelNewUser");
        labelNewUser->setGeometry(QRect(410, 410, 300, 20));
        labelNewUser->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        labelNewUser->setAlignment(Qt::AlignmentFlag::AlignCenter);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1123, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        loginBtn->setText(QCoreApplication::translate("MainWindow", "LOGIN", nullptr));
        labelBadapatra->setText(QCoreApplication::translate("MainWindow", "\340\244\210 - BADAPATRA", nullptr));
        labelNewUser->setText(QCoreApplication::translate("MainWindow", "New User?", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
