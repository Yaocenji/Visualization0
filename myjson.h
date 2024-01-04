#ifndef MYJSON_H
#define MYJSON_H

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVector>

QVector<QString> JsonToStringArray(QJsonValue jsonValue);

struct galgame {
public:
    QString id;
    QVector<QString> languages;
    int length_minutes;
    int length_votes;
    QVector<QString> platforms;
    float rating;
    QString title;
    int votecount;
    double getDouble(QString tag) {
        if (tag == "length_minutes") return length_minutes;
        if (tag == "length_votes") return length_votes;
        if (tag == "rating") return rating;
        if (tag == "votecount") return votecount;
        qDebug() << "getDouble type error";
        return -1;
    }
};

class myjson {
public:
    myjson(QJsonObject jsonObject);
    QVector<galgame *> galgames;

private:
    // QVector<QString> toStringArray(QJsonValue jsonValue);
};

#endif // MYJSON_H
