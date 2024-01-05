#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QBasicTimer>
#include <QMainWindow>

#include "myrequester.h"
#include "themewidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *e) override;

public slots:
    void finishRequest();

    void sendRequest(QString suffix, int totelNumber,
                     QVector<QVector<QString>> filters, QVector<QString> fields,
                     QString sort, bool reverse);

    void RerequestVN();

public:
    QBasicTimer vnTimer;
    myrequester *m;
    ThemeWidget *widget;
};
#endif // MAINWINDOW_H
