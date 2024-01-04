#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    myrequester *m = new myrequester(this);

    QObject::connect(m, SIGNAL(finish(QJsonObject *, QNetworkReply *)), this,
                     SLOT(finishRequest()));
    m->sendRequest("vn", {{"tag", "=", "g23"}, {"tag", "=", "g45"}},
                   {"title", "languages", "platforms", "length_minutes",
                    "length_votes", "rating", "votecount"},
                   "rating", true, 5, 1);
}

MainWindow::~MainWindow() {
}

void MainWindow::finishRequest() {
    qDebug() << "mainwindow receive.";
}
