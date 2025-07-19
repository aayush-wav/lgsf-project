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
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *widget;
    QLineEdit *inputLineEdit;
    QPushButton *sendButton;
    QTextEdit *responseTextEdit;
    QLabel *badapatraLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 720);
        QFont font;
        font.setPointSize(13);
        MainWindow->setFont(font);
        MainWindow->setStyleSheet(QString::fromUtf8("background-color: rgb(28, 28, 28);"));
        widget = new QWidget(MainWindow);
        widget->setObjectName("widget");
        inputLineEdit = new QLineEdit(widget);
        inputLineEdit->setObjectName("inputLineEdit");
        inputLineEdit->setGeometry(QRect(150, 590, 741, 51));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("0xProto Nerd Font")});
        font1.setPointSize(13);
        font1.setWeight(QFont::Medium);
        font1.setKerning(false);
        font1.setStyleStrategy(QFont::PreferDefault);
        inputLineEdit->setFont(font1);
        inputLineEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::PreventContextMenu);
        inputLineEdit->setStyleSheet(QString::fromUtf8("border-radius: 6;\n"
"background-color: rgba(88, 88, 88,88,8);\n"
"padding: 10px;"));
        inputLineEdit->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
        sendButton = new QPushButton(widget);
        sendButton->setObjectName("sendButton");
        sendButton->setGeometry(QRect(900, 590, 171, 51));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("0xProto Nerd Font")});
        font2.setPointSize(13);
        font2.setBold(true);
        font2.setKerning(false);
        sendButton->setFont(font2);
        sendButton->setStyleSheet(QString::fromUtf8("color:  #2148C0;\n"
"background-color: rgb(255, 255, 255);\n"
"border-radius: 6;"));
        responseTextEdit = new QTextEdit(widget);
        responseTextEdit->setObjectName("responseTextEdit");
        responseTextEdit->setGeometry(QRect(150, 40, 1031, 540));
        QFont font3;
        font3.setFamilies({QString::fromUtf8("0xProto Nerd Font Mono")});
        font3.setPointSize(12);
        responseTextEdit->setFont(font3);
        responseTextEdit->setFrameShape(QFrame::Shape::NoFrame);
        responseTextEdit->setFrameShadow(QFrame::Shadow::Plain);
        responseTextEdit->setTextInteractionFlags(Qt::TextInteractionFlag::TextSelectableByKeyboard|Qt::TextInteractionFlag::TextSelectableByMouse);
        badapatraLabel = new QLabel(widget);
        badapatraLabel->setObjectName("badapatraLabel");
        badapatraLabel->setGeometry(QRect(468, 30, 345, 111));
        QFont font4;
        font4.setPointSize(22);
        font4.setBold(true);
        font4.setKerning(false);
        badapatraLabel->setFont(font4);
        badapatraLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        MainWindow->setCentralWidget(widget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1280, 27));
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
        sendButton->setText(QCoreApplication::translate("MainWindow", "SEND", nullptr));
        badapatraLabel->setText(QCoreApplication::translate("MainWindow", "\340\244\210 - BADAPATRA", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
