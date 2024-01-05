#include "myrequester.h"

myrequester::myrequester(QObject *parent) : QObject{parent} {
    naManager = new QNetworkAccessManager;
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      QVariant("application/json"));
    // 连接信号与槽函数
    QObject::connect(naManager, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(finishRequest(QNetworkReply *)));
}

void myrequester::sendRequest(QString suffix, QVector<QVector<QString>> filters,
                              QVector<QString> fields, QString sort,
                              bool reverse, int results, uint page) {
    QString urlStr = urlbase0 + suffix;
    QUrl url = QUrl::fromUserInput(urlStr);
    request.setUrl(url);

    QJsonObject argumentsJson;

    // filter信息
    QJsonArray filterArray;
    // 是否有多个过滤器
    if (filters.size() >= 2) {
        filterArray.push_back("and");
        // 遍历
        for (auto &i : filters) {
            QJsonArray thisFilter;
            for (auto &j : i) {
                thisFilter.push_back(j);
            }
            filterArray.push_back(thisFilter);
        }
    } else if (filters.size() == 1) {
        for (auto &j : filters[0]) {
            filterArray.push_back(j);
        }
    }
    // 将filter数组插入
    argumentsJson.insert("filters", filterArray);

    // fields信息
    QString fieldString = "";
    for (int i = 0; i < fields.size(); ++i) {
        fieldString += fields[i];
        if (i != fields.size() - 1) fieldString += ", ";
    }
    // 将field插入
    argumentsJson.insert("fields", fieldString);

    // sort信息
    argumentsJson.insert("sort", sort);

    // reverse信息
    argumentsJson.insert("reverse", reverse);

    // results信息
    argumentsJson.insert("results", results);

    // page信息
    argumentsJson.insert("page", int(page));

    // 其他信息
    //    argumentsJson.insert("user", NULL);
    //    argumentsJson.insert("count", false);
    //    argumentsJson.insert("compact_filters", false);
    //    argumentsJson.insert("normalized_filters", false);

    qDebug() << "ArgJson:"
             << "\n"
             << argumentsJson << "\n";

    // 发出请求
    naManager->post(request, QJsonDocument(argumentsJson).toJson());
}

void myrequester::finishRequest(QNetworkReply *reply) {
    QString loginResultJson;
    QByteArray bytes = reply->readAll();
    // 获取http状态码
    QVariant statusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid()) {
        qDebug() << "status code = " << statusCode.toInt();
    }

    QNetworkReply::NetworkError err = reply->error();
    if (err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
        // 发出信号
        emit finishVn(&emptyJsonObj, reply);
    } else {
        // 获取返回内容
        QJsonDocument doc = QJsonDocument::fromJson(bytes);
        QJsonObject json = doc.object();

        // qDebug() << "ResultJson: \n" << json << "\n\n";
        loginResultJson = QString::fromStdString(bytes.toStdString());
        // 处理返回数据
        // 发出信号
        if (reply->request().url().toString().contains("/vn",
                                                       Qt::CaseSensitive)) {
            qDebug() << "receive vn data";
            emit finishVn(&json, reply);
        } else if (reply->request().url().toString().contains(
                       "/tag", Qt::CaseSensitive)) {
            qDebug() << "receive tag data";
            emit finishTag(&json, reply);
        }
    }
}
