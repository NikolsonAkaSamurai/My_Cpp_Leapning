// hhparser.h
#ifndef HHPARSER_H
#define HHPARSER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include "vacancy.h"

class HHParser : public QObject {
    Q_OBJECT

public:
    explicit HHParser(QObject *parent = nullptr);
    ~HHParser();

    void setArea(int areaId);
    void setPerPage(int count);
    void setDelay(int ms);

    void searchVacancies(const QString &query, int maxPages = -1);
    void stopParsing();
    void fetchFullVacancy(const QString &vacancyId);

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
    void onSearchPageFinished();
    void onFullVacancyFinished();
    void onRateLimitTimer();

private:
    QNetworkReply* makeRequest(const QString &url);
    bool handleNetworkError(QNetworkReply *reply);
    void parseVacancyCard(const QJsonObject &obj);
    Vacancy parseFullVacancy(const QJsonObject &obj);

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

#endif // HHPARSER_H