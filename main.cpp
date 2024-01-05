#include <QApplication>
#include <QBasicTimer>
#include <QFile>
#include <QMainWindow>
#include <QObject>

#include "mainwindow.h"
#include "themewidget.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow window;

    return a.exec();
}
