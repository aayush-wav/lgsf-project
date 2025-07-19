#include "loginwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("ई - BADAPATRA Login");
    LoginWindow w;
    w.show();
    return a.exec();
}
