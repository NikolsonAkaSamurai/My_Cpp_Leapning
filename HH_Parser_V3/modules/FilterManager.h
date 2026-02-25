// FilterManager.h
#ifndef FILTERMANAGER_H
#define FILTERMANAGER_H

#include <QObject>
#include <QSet>
#include <QStringList>
#include "../core/Vacancy.h"

class FilterManager : public QObject {
    Q_OBJECT

public:
    explicit FilterManager(QObject* parent = nullptr);

    // Стоп-слова
    void addStopWord(const QString& word);
    void removeStopWord(const QString& word);
    QStringList getStopWords() const;
    bool containsStopWord(const QString& text) const;

    // Черный список
    void addToBlacklist(const QString& vacancyId);
    void removeFromBlacklist(const QString& vacancyId);
    bool isBlacklisted(const QString& vacancyId) const;
    QSet<QString> getBlacklist() const;

    // Фильтрация
    QList<Vacancy> filterVacancies(const QList<Vacancy>& vacancies) const;

    // UI
    void showStopWordsDialog(QWidget* parent);

    signals:
        void stopWordsChanged();
    void blacklistChanged();

private:
    void loadFilters();
    void loadStopWords();
    void saveStopWords();
    void loadBlacklist();
    void saveBlacklist();

private:
    QSet<QString> m_stopWords;
    QSet<QString> m_blacklist;
};

#endif // FILTERMANAGER_H