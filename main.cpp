#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    MainWindow w;
    w.setWindowTitle("框选识别");
    w.setWindowIcon(QIcon("icon.png"));
    w.show();
    return a.exec();
}
