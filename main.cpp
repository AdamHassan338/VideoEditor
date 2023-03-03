#include "mainwindow.h"

#include <QApplication>
#include <videoreader.h>
#include "videoreader.h"
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
