#include "mainwindow.h"

struct ReqArguments {
    QString suffix;
    int totelNumber;
    QVector<QVector<QString>> filters;
    QVector<QString> fields;
    QString sort;
    bool reverse;
    int results;
    uint page;
} RA[2];

const QVector<QString> vn_fields = {
    "title",        "languages", "platforms", "length_minutes",
    "length_votes", "rating",    "votecount"};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    widget = new ThemeWidget(QJsonObject());
    setCentralWidget(widget);
    resize(1920, 1080);
    show();

    m = new myrequester();
    QObject::connect(m, SIGNAL(finishVn(QJsonObject *, QNetworkReply *)),
                     widget, SLOT(addJsonData(QJsonObject *)));
    QObject::connect(m, SIGNAL(finishTag(QJsonObject *, QNetworkReply *)),
                     widget, SLOT(addTagData(QJsonObject *)));
    QObject::connect(widget->m_ui->filterApplyPushButton, SIGNAL(clicked()),
                     this, SLOT(RerequestVN()));
    //    m->sendRequest("vn", {},
    //                   {"title", "languages", "platforms", "length_minutes",
    //                    "length_votes", "rating", "votecount"},
    //                   "rating", true, 10, 1);

    //    m->sendRequest("tag", {}, {"name, vn_count"}, "vn_count", true, 100,
    //    1);

    this->sendRequest("vn", 500, {}, vn_fields, "rating", true);

    vnTimer.start(100, this);
}

MainWindow::~MainWindow() {
    delete m;
}

void MainWindow::finishRequest() {
    qDebug() << "mainwindow receive.";
}

void MainWindow::timerEvent(QTimerEvent *e) {
    if (RA[0].totelNumber != 0) {
        m->sendRequest(RA[0].suffix, RA[0].filters, RA[0].fields, RA[0].sort,
                       RA[0].reverse, 100, RA[0].page);
        if (RA[0].totelNumber <= 100) {
            RA[0].totelNumber = 0;
            qDebug() << "finish vn request.";
        } else {
            RA[0].totelNumber -= 100;
            qDebug() << "doing vn request.";
        }
        RA[0].page++;
    }
    if (RA[1].totelNumber != 0) {
        m->sendRequest(RA[1].suffix, RA[1].filters, RA[1].fields, RA[1].sort,
                       RA[1].reverse, 100, RA[1].page);
        if (RA[1].totelNumber <= 100) {
            RA[1].totelNumber = 0;
            qDebug() << "finish tag request.";
        } else {
            RA[1].totelNumber -= 100;
            qDebug() << "doing vn request.";
        }
        RA[1].page++;
    }
}

void MainWindow::sendRequest(QString suffix, int totelNumber,
                             QVector<QVector<QString>> filters,
                             QVector<QString> fields, QString sort,
                             bool reverse) {
    if (totelNumber <= 100) {
        m->sendRequest(suffix, filters, fields, sort, reverse, totelNumber, 1);
    } else {
        if (suffix == "vn") {
            RA[0].totelNumber = totelNumber;
            RA[0].suffix = suffix;
            RA[0].filters = filters;
            RA[0].fields = fields;
            RA[0].sort = sort;
            RA[0].reverse = reverse;
            RA[0].results = 100;
            RA[0].page = 1;
        } else if (suffix == "tag") {
            RA[1].totelNumber = totelNumber;
            RA[1].suffix = suffix;
            RA[1].filters = filters;
            RA[1].fields = fields;
            RA[1].sort = sort;
            RA[1].reverse = reverse;
            RA[0].results = 100;
            RA[0].page = 1;
        }
    }
}

void MainWindow::RerequestVN() {
    qDebug() << "rerequest vn.";
    widget->clearVnData();
    //    qDebug() << "arg: " <<
    //    (widget->m_ui->vnTotelNumberLineEdit->text()).toInt()
    //             << widget->getFilter() <<
    //             widget->m_ui->sortComboBox->currentText()
    //             << widget->m_ui->inverseCheckBox->isChecked();
    sendRequest("vn", (widget->m_ui->vnTotelNumberLineEdit->text()).toInt(),
                widget->getFilter(), vn_fields,
                widget->m_ui->sortComboBox->currentText(),
                !widget->m_ui->inverseCheckBox->isChecked());
}
