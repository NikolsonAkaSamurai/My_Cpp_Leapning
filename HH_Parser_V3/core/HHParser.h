// HHParser.h
#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QThread>
#include "Vacancy.h"

class HHParser : public QObject {
    Q_OBJECT

public:
    explicit HHParser(QObject *parent = nullptr) : QObject(parent) {
        m_manager = new QNetworkAccessManager(this);
        m_rateTimer = new QTimer(this);
        m_rateTimer->setSingleShot(true);
        connect(m_rateTimer, &QTimer::timeout, this, &HHParser::onRateLimitTimer);
    }

    ~HHParser() {
        stopParsing();
    }

    // Настройки
    void setArea(int areaId) { m_areaId = areaId; }
    void setPerPage(int count) { m_perPage = qBound(1, count, 100); }
    void setDelay(int ms) { m_delayMs = qMax(500, ms); }

    // Основные методы
    void searchVacancies(const QString &query, int maxPages = -1) {
        if (m_isRunning) {
            emit errorOccurred("Парсинг уже запущен");
            return;
        }

        m_currentQuery = query;
        m_maxPages = maxPages;
        m_parsedVacancies.clear();
        m_parsedIds.clear();
        m_currentPage = 0;
        m_retryCount = 0;
        m_isRunning = true;

        QString urlStr = QString("https://api.hh.ru/vacancies?text=%1&area=%2&per_page=%3&page=0")
                             .arg(query).arg(m_areaId).arg(m_perPage);

        QNetworkReply *reply = makeRequest(urlStr);
        if (reply) connect(reply, &QNetworkReply::finished, this, &HHParser::onSearchPageFinished);
    }

    void stopParsing() {
        m_isRunning = false;
        m_manager->disconnect();
        m_rateTimer->stop();
    }

    void fetchFullVacancy(const QString &vacancyId) {
        QString urlStr = QString("https://api.hh.ru/vacancies/%1").arg(vacancyId);
        QNetworkReply *reply = makeRequest(urlStr);
        if (reply) connect(reply, &QNetworkReply::finished, this, &HHParser::onFullVacancyFinished);
    }

    bool isRunning() const { return m_isRunning; }
    int getTotalFound() const { return m_totalFound; }

signals:
    void searchStarted(const QString &query, int totalFound);
    void pageParsed(int page, int totalPages, int currentCount);
    void vacancyParsed(const Vacancy &vacancy);
    void searchFinished(const QList<Vacancy> &vacancies);
    void fullVacancyFetched(const Vacancy &vacancy);
    void fullVacancyError(const QString &id, const QString &error);
    void errorOccurred(const QString &error);
    void rateLimitExceeded();

private slots:
    void onSearchPageFinished() {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        if (!reply) return;
        reply->deleteLater();

        if (!m_isRunning) return;
        if (!handleNetworkError(reply)) {
            if (m_retryCount < m_maxRetries) {
                m_retryCount++;
                QTimer::singleShot(m_delayMs * 2, [this]() {
                    searchVacancies(m_currentQuery, m_maxPages);
                });
            } else {
                emit errorOccurred("Превышено количество попыток");
                m_isRunning = false;
            }
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();

        if (m_currentPage == 0) {
            m_totalFound = obj.value("found").toInt();
            m_totalPages = obj.value("pages").toInt();
            if (m_maxPages > 0) m_totalPages = qMin(m_totalPages, m_maxPages);
            emit searchStarted(m_currentQuery, m_totalFound);
            if (m_totalFound == 0) {
                emit searchFinished(m_parsedVacancies);
                m_isRunning = false;
                return;
            }
        }

        QJsonArray items = obj.value("items").toArray();
        for (const auto &item : items) parseVacancyCard(item.toObject());

        emit pageParsed(m_currentPage + 1, m_totalPages, m_parsedVacancies.size());

        m_currentPage++;
        m_retryCount = 0;

        if (m_currentPage < m_totalPages && m_isRunning) {
            QTimer::singleShot(m_delayMs, [this]() {
                if (m_isRunning) {
                    QString urlStr = QString("https://api.hh.ru/vacancies?text=%1&area=%2&per_page=%3&page=%4")
                                         .arg(m_currentQuery).arg(m_areaId).arg(m_perPage).arg(m_currentPage);
                    QNetworkReply *reply = makeRequest(urlStr);
                    if (reply) connect(reply, &QNetworkReply::finished, this, &HHParser::onSearchPageFinished);
                }
            });
        } else {
            emit searchFinished(m_parsedVacancies);
            m_isRunning = false;
        }
    }

    void onFullVacancyFinished() {
        QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
        if (!reply) return;
        reply->deleteLater();

        if (!handleNetworkError(reply)) {
            emit fullVacancyError("unknown", reply->errorString());
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();
        Vacancy vacancy = parseFullVacancy(obj);
        vacancy.setFullDataFlag(true);
        emit fullVacancyFetched(vacancy);
    }

    void onRateLimitTimer() {
        m_rateLimitActive = false;
    }

private:
    QNetworkReply* makeRequest(const QString &urlStr) {
        if (m_rateLimitActive) {
            emit rateLimitExceeded();
            return nullptr;
        }

        QUrl url(urlStr);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, "HH-Parser/2.0");
        request.setRawHeader("Accept", "application/json");
        return m_manager->get(request);
    }

    bool handleNetworkError(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) return true;

        if (reply->error() == QNetworkReply::ContentAccessDenied) {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (statusCode == 429) {
                m_rateLimitActive = true;
                m_rateTimer->start(60000);
                emit rateLimitExceeded();
                return false;
            }
        }

        emit errorOccurred(reply->errorString());
        return false;
    }

    void parseVacancyCard(const QJsonObject &obj) {
        QString id = obj.value("id").toString();
        if (m_parsedIds.contains(id)) return;

        Vacancy vacancy;
        vacancy.setId(id);
        vacancy.setName(obj.value("name").toString());
        vacancy.setUrl(obj.value("alternate_url").toString());
        vacancy.setPublishedAt(obj.value("published_at").toString());

        QJsonObject employer = obj.value("employer").toObject();
        vacancy.setCompany(employer.value("name").toString());

        QJsonObject salary = obj.value("salary").toObject();
        if (!salary.isEmpty()) {
            vacancy.setSalaryFrom(salary.value("from").toInt());
            vacancy.setSalaryTo(salary.value("to").toInt());
            vacancy.setSalaryCurrency(salary.value("currency").toString());
        }

        vacancy.setFullDataFlag(false);
        m_parsedVacancies.append(vacancy);
        m_parsedIds.insert(id, true);
        emit vacancyParsed(vacancy);
    }

    Vacancy parseFullVacancy(const QJsonObject &obj) {
        Vacancy vacancy;
        vacancy.setId(obj.value("id").toString());
        vacancy.setName(obj.value("name").toString());
        vacancy.setUrl(obj.value("alternate_url").toString());
        vacancy.setPublishedAt(obj.value("published_at").toString());

        QJsonObject employer = obj.value("employer").toObject();
        vacancy.setCompany(employer.value("name").toString());

        QJsonObject salary = obj.value("salary").toObject();
        if (!salary.isEmpty()) {
            vacancy.setSalaryFrom(salary.value("from").toInt());
            vacancy.setSalaryTo(salary.value("to").toInt());
            vacancy.setSalaryCurrency(salary.value("currency").toString());
        }

        vacancy.setDescription(obj.value("description").toString());

        QJsonObject address = obj.value("address").toObject();
        QString addr = address.value("raw").toString();
        if (addr.isEmpty()) {
            QStringList parts;
            if (!address.value("city").toString().isEmpty()) parts << address.value("city").toString();
            if (!address.value("street").toString().isEmpty()) parts << address.value("street").toString();
            if (!address.value("building").toString().isEmpty()) parts << address.value("building").toString();
            addr = parts.isEmpty() ? "Адрес не указан" : parts.join(", ");
        }
        vacancy.setAddress(addr);

        QJsonObject experience = obj.value("experience").toObject();
        vacancy.setExperience(experience.value("name").toString());

        QJsonObject employment = obj.value("employment").toObject();
        vacancy.setEmployment(employment.value("name").toString());

        QJsonObject schedule = obj.value("schedule").toObject();
        vacancy.setSchedule(schedule.value("name").toString());

        return vacancy;
    }

private:
    QNetworkAccessManager *m_manager;
    QTimer *m_rateTimer;

    QString m_currentQuery;
    int m_areaId = 2;
    int m_perPage = 20;
    int m_delayMs = 1000;
    int m_maxRetries = 3;

    int m_totalFound = 0;
    int m_totalPages = 0;
    int m_currentPage = 0;
    int m_maxPages = -1;
    bool m_isRunning = false;
    int m_retryCount = 0;
    bool m_rateLimitActive = false;

    QList<Vacancy> m_parsedVacancies;
    QHash<QString, bool> m_parsedIds;
};
