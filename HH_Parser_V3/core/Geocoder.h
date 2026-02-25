// Geocoder.h
#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>
#include "Point.h"

class Geocoder : public QObject {
    Q_OBJECT

public:
    explicit Geocoder(QObject *parent = nullptr)
        : QObject(parent), m_manager(new QNetworkAccessManager(this)) {}

    // Синхронный метод — получает адрес, возвращает Point
    Point geocode(const QString &address) {
        QUrl url("https://photon.komoot.io/api/");
        QUrlQuery query;
        query.addQueryItem("q", address);
        url.setQuery(query);

        QNetworkRequest request(url);
        // User-Agent можно оставить любой, Photon не блокирует
        request.setHeader(QNetworkRequest::UserAgentHeader, "GeocoderTest/1.0");

        QNetworkReply *reply = m_manager->get(request);

        // Ждём ответ
        QEventLoop loop;
        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        QTimer::singleShot(5000, &loop, &QEventLoop::quit); // таймаут 5 сек
        loop.exec();

        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Ошибка сети:" << reply->errorString();
            reply->deleteLater();
            return Point(0, 0);
        }

        QByteArray data = reply->readAll();
        reply->deleteLater();

        // Отладка — посмотрим, что пришло
        qDebug() << "Ответ от Photon:" << data.left(200);

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();
        QJsonArray features = obj["features"].toArray();

        if (features.isEmpty()) {
            qWarning() << "Ничего не найдено для адреса:" << address;
            return Point(0, 0);
        }

        QJsonObject first = features[0].toObject();
        QJsonObject geometry = first["geometry"].toObject();
        QJsonArray coordinates = geometry["coordinates"].toArray();

        if (coordinates.size() < 2) return Point(0, 0);

        // В Photon координаты приходят как [lon, lat]
        double lon = coordinates[0].toDouble();
        double lat = coordinates[1].toDouble();

        return Point(lat, lon);
    }

private:
    QNetworkAccessManager *m_manager;
};