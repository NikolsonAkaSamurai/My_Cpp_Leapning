// core/GeocoderWorker.h
#pragma once

#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QMutex>
#include "Geocoder.h"

// Forward declaration — чтобы не включать Controller.h сюда
class Controller;

class GeocoderWorker : public QObject {
    Q_OBJECT

public:
    explicit GeocoderWorker(Controller* controller, QObject* parent = nullptr);
    ~GeocoderWorker();

    void queueVacancy(const QString& vacancyId);
    void queueVacancies(const QList<QString>& vacancyIds);
    void clearQueue();
    int queueSize();

    signals:
        void vacancyGeocoded(const QString& id, double lat, double lon);
    void queueProgress(int processed, int total);
    void error(const QString& id, const QString& error);

private slots:
    void processNext();

private:
    Controller* m_controller;
    Geocoder* m_geocoder;
    QTimer* m_timer;
    QQueue<QString> m_queue;
    QMutex m_mutex;
    bool m_isProcessing;
};