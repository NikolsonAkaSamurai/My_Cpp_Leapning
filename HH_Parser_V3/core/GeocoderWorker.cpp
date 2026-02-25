// core/GeocoderWorker.cpp
#include "GeocoderWorker.h"
#include "Controller.h"  // здесь уже полное определение Controller

GeocoderWorker::GeocoderWorker(Controller* controller, QObject* parent)
    : QObject(parent)
    , m_controller(controller)
    , m_geocoder(new Geocoder(this))
    , m_timer(new QTimer(this))
    , m_isProcessing(false)
{
    m_timer->setInterval(1000); // 1 секунда между запросами
    connect(m_timer, &QTimer::timeout, this, &GeocoderWorker::processNext);
}

GeocoderWorker::~GeocoderWorker()
{
    clearQueue();
    delete m_geocoder;
}

void GeocoderWorker::queueVacancy(const QString& vacancyId)
{
    QMutexLocker locker(&m_mutex);
    m_queue.enqueue(vacancyId);

    if (!m_isProcessing) {
        m_isProcessing = true;
        m_timer->start();
    }
}

void GeocoderWorker::queueVacancies(const QList<QString>& vacancyIds)
{
    QMutexLocker locker(&m_mutex);
    for (const auto& id : vacancyIds) {
        m_queue.enqueue(id);
    }

    if (!m_isProcessing && !m_queue.isEmpty()) {
        m_isProcessing = true;
        m_timer->start();
    }
}

void GeocoderWorker::clearQueue()
{
    QMutexLocker locker(&m_mutex);
    m_queue.clear();
    m_isProcessing = false;
    m_timer->stop();
}

int GeocoderWorker::queueSize()
{
    QMutexLocker locker(&m_mutex);
    return m_queue.size();
}

void GeocoderWorker::processNext()
{
    QString id;
    {
        QMutexLocker locker(&m_mutex);
        if (m_queue.isEmpty()) {
            m_isProcessing = false;
            m_timer->stop();
            return;
        }
        id = m_queue.dequeue();
    }

    // Получаем вакансию через Controller по ID
    Vacancy* vacancy = m_controller->findVacancyInCurrent(id);
    if (!vacancy) {
        emit error(id, "Вакансия не найдена");
        return;
    }

    QString address = vacancy->getAddress();
    if (address.isEmpty()) {
        emit error(id, "Адрес пуст");
        return;
    }

    if (vacancy->hasCoords()) {
        // Уже есть координаты — пропускаем
        return;
    }

    Point coords = m_geocoder->geocode(address);

    if (coords.getX() != 0 || coords.getY() != 0) {
        emit vacancyGeocoded(id, coords.getX(), coords.getY());
    } else {
        emit error(id, "Не удалось найти координаты");
    }

    emit queueProgress(m_queue.size(), m_queue.size() + 1);
}