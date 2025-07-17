// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QTimer>
#include<QDebug>
#include<QString>
#include<QLabel>
#include <QScrollBar>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow constructor reached";
    ui->setupUi(this);

    if (!ui->chatContainerWidget) {
        qDebug() << "chatContainerWidget is NULL!";
    }
    if (!ui->scrollArea) {
        qDebug() << "scrollArea is NULL!";
    }

    // Make sure the scroll area has the widget set
    ui->scrollArea->setWidget(ui->chatContainerWidget);

    // Set layout if missing
    if (!ui->chatContainerWidget->layout()) {
        QVBoxLayout* layout = new QVBoxLayout(ui->chatContainerWidget);
        ui->chatContainerWidget->setLayout(layout);
    }

    // Debug check
    qDebug() << "chatContainerWidget:" << ui->chatContainerWidget;
    qDebug() << "layout:" << ui->chatContainerWidget->layout();
}
void MainWindow::handleSend()
{
    QString text = ui->inputTextEdit->toPlainText().trimmed(); // Step 1 & 2
    if (text.isEmpty()) return;

    addMessage(text, true); // Step 3: true means user message
    ui->inputTextEdit->clear(); // Step 4

    // Step 5: simulate bot reply after 500 ms delay
    QTimer::singleShot(500, this, [=]() {
        qDebug() << "App started correctly";
        addMessage("This is a reply", false);
    });
}

void MainWindow::addMessage(const QString &text, bool isUser) {
    if (!ui->chatContainerWidget || !ui->chatContainerWidget->layout()) return;

    // Message label
    QLabel* messageLabel = new QLabel(text);
    messageLabel->setWordWrap(true);
    messageLabel->setMaximumWidth(400);
    messageLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QString bgColor = isUser ? "#cfe2ff" : "#e1f0ff";
    messageLabel->setStyleSheet(QString(
                                    "background-color: %1;"
                                    "padding: 10px;"
                                    "border-radius: 10px;"
                                    "font-size: 14px;").arg(bgColor));

    // Timestamp label
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm AP");
    QLabel* timeLabel = new QLabel(timestamp);
    timeLabel->setStyleSheet("font-size: 10px; color: gray;");
    timeLabel->setAlignment(Qt::AlignRight);
    timeLabel->setMaximumWidth(400);

    // Vertical layout to stack message and time
    QVBoxLayout* bubbleLayout = new QVBoxLayout;
    bubbleLayout->addWidget(messageLabel);
    bubbleLayout->addWidget(timeLabel);

    // Align bubble left (bot) or right (user)
    QHBoxLayout* rowLayout = new QHBoxLayout;
    if (isUser) {
        rowLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
        rowLayout->addLayout(bubbleLayout);
    } else {
        rowLayout->addLayout(bubbleLayout);
        rowLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }

    QWidget* container = new QWidget;
    container->setLayout(rowLayout);
    ui->chatContainerWidget->layout()->addWidget(container);

    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}


MainWindow::~MainWindow()
{
    delete ui;
}
