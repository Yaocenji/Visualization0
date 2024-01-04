#ifndef MYREQUESTER_H
#define MYREQUESTER_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVector>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

struct myresult {
    QJsonObject json;
};

class myrequester : public QObject {
    Q_OBJECT
public:
    explicit myrequester(QObject *parent = nullptr);

public:
    // 发送post请求
    void sendRequest(QString suffix, QVector<QVector<QString>> filters,
                     QVector<QString> fields, QString sort, bool reverse,
                     int results, uint page);

public slots:
    // 接受响应结果
    void finishRequest(QNetworkReply *reply);

signals:
    void finish(QJsonObject *jsonInfo, QNetworkReply *rawReply);

public:
    QString urlbase0 = {"https://api.vndb.org/kana/"};

    QJsonObject emptyJsonObj;

    QNetworkRequest request;
    QNetworkAccessManager *naManager;
};

#endif // MYREQUESTER_H
