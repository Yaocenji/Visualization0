#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    myrequester *m = new myrequester(this);

    QObject::connect(m, SIGNAL(finish(QJsonObject *, QNetworkReply *)), this,
                     SLOT(finishRequest()));
    m->sendRequest("vn", {}, {"title"}, "rating", true, 10, 1);
}

MainWindow::~MainWindow() {
}

void MainWindow::finishRequest() {
    qDebug() << "mainwindow receive.";
}
