#include <QApplication>
#include <QFile>
#include <QMainWindow>
#include <QObject>

#include "mainwindow.h"
#include "themewidget.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QMainWindow window;

    //    QFile file("C:/Users/Dic'kon/Desktop/Untitled-1.json");
    //    QString errMsg;
    //    QFileDevice::FileError err = QFileDevice::NoError;
    //    if (!file.open(QIODevice::ReadOnly)) {
    //        errMsg = file.errorString();
    //        err = file.error();
    //        qDebug() << errMsg << err;
    //    }
    //    QByteArray data = file.readAll();
    //    file.close();

    ThemeWidget *widget = new ThemeWidget(QJsonObject());
    window.setCentralWidget(widget);
    window.resize(900, 600);
    window.show();

    myrequester *m = new myrequester();
    QObject::connect(m, SIGNAL(finish(QJsonObject *, QNetworkReply *)), widget,
                     SLOT(addJsonData(QJsonObject *)));
    m->sendRequest("vn", {{"tag", "=", "g23"}, {"tag", "=", "g45"}},
                   {"title", "languages", "platforms", "length_minutes",
                    "length_votes", "rating", "votecount"},
                   "rating", true, 50, 1);

    //qDebug() << "OpenSSL支持情况:" << QSslSocket::supportsSsl();
    return a.exec();
}
