#include <QApplication>
#include <QFile>
#include "themewidget.h"
#include <QMainWindow>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow window;

    QFile file("D:/other/Untitled1.json");

    QString errMsg;
    QFileDevice::FileError err = QFileDevice::NoError;
    if (!file.open(QIODevice::ReadOnly)) {
        errMsg = file.errorString();
        err = file.error();
        qDebug() << errMsg << err;
    }

    QByteArray data = file.readAll();
    file.close();

    ThemeWidget *widget = new ThemeWidget(QJsonDocument::fromJson(data).object());
    window.setCentralWidget(widget);
    window.resize(900, 600);
    window.show();
    return a.exec();
}
