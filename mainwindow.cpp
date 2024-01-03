#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    myrequester *m = new myrequester(this);
    m->sendRequest("vn", {}, {"title"}, "rating", true, 10, 1);
}

MainWindow::~MainWindow() {
}
