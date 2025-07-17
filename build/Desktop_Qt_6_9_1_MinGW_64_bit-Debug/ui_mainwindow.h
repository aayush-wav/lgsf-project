/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
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
    QLineEdit *lastNameLineEdit;
    QLineEdit *firstNameLineEdit;
    QPushButton *signupLineEdit;
    QLabel *label_3;
    QLineEdit *passwordLineEdit;
    QLineEdit *retypePasswordLineEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1378, 956);
        MainWindow->setStyleSheet(QString::fromUtf8("*{\n"
"	background-image: url(:/res/lgsf bg.jpeg);\n"
"	font-family:montserrat;\n"
"	background-size:100% 100%;\n"
"	background-color: rgb(28, 28, 28);\n"
"	color: white;\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        lastNameLineEdit = new QLineEdit(centralwidget);
        lastNameLineEdit->setObjectName("lastNameLineEdit");
        lastNameLineEdit->setGeometry(QRect(510, 280, 300, 45));
        firstNameLineEdit = new QLineEdit(centralwidget);
        firstNameLineEdit->setObjectName("firstNameLineEdit");
        firstNameLineEdit->setGeometry(QRect(510, 220, 300, 45));
        signupLineEdit = new QPushButton(centralwidget);
        signupLineEdit->setObjectName("signupLineEdit");
        signupLineEdit->setGeometry(QRect(510, 490, 300, 45));
        QFont font;
        font.setFamilies({QString::fromUtf8("montserrat")});
        font.setPointSize(12);
        font.setBold(true);
        font.setKerning(false);
        signupLineEdit->setFont(font);
        signupLineEdit->setStyleSheet(QString::fromUtf8("background-color: white;\n"
"color: #2148C0;\n"
"border-radius: 4;"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(480, 90, 345, 151));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("montserrat")});
        font1.setPointSize(22);
        font1.setBold(true);
        label_3->setFont(font1);
        label_3->setAlignment(Qt::AlignmentFlag::AlignCenter);
        passwordLineEdit = new QLineEdit(centralwidget);
        passwordLineEdit->setObjectName("passwordLineEdit");
        passwordLineEdit->setGeometry(QRect(510, 340, 300, 45));
        retypePasswordLineEdit = new QLineEdit(centralwidget);
        retypePasswordLineEdit->setObjectName("retypePasswordLineEdit");
        retypePasswordLineEdit->setGeometry(QRect(510, 400, 300, 45));
        MainWindow->setCentralWidget(centralwidget);
        label_3->raise();
        lastNameLineEdit->raise();
        firstNameLineEdit->raise();
        signupLineEdit->raise();
        passwordLineEdit->raise();
        retypePasswordLineEdit->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1378, 19));
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
        lastNameLineEdit->setText(QString());
        firstNameLineEdit->setText(QString());
        signupLineEdit->setText(QCoreApplication::translate("MainWindow", "SIGN UP", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\340\244\207-BADAPATRA", nullptr));
        passwordLineEdit->setText(QString());
        retypePasswordLineEdit->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
