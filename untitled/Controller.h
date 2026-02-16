// controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QSet>
#include "vacancy.h"

struct Profile {
    QString name;
    QString displayName;
    QString folderPath;
    QDateTime created;
    QDateTime lastModified;
    QList<Vacancy> vacancies;

    bool isEmpty() const { return name.isEmpty(); }

    void saveMetadata() const;
    void loadMetadata();
    void saveVacancies() const;
    void loadVacancies();
    void updateStats();
};

class Controller : public QObject {
    Q_OBJECT

public:
    static Controller& instance() {
        static Controller controller;
        return controller;
    }

    // Управление профилями
    bool createProfile(const QString &name, const QString &displayName = "");
    bool deleteProfile(const QString &name);
    bool renameProfile(const QString &oldName, const QString &newName);
    bool duplicateProfile(const QString &sourceName, const QString &newName);
    bool loadProfile(const QString &name);
    QList<Profile> getAllProfiles() const { return m_profiles; }
    Profile* getCurrentProfile() { return m_currentProfile.isEmpty() ? nullptr : &m_currentProfile; }
    void scanProfiles();

    // Работа с вакансиями
    void addVacancyToCurrent(const Vacancy &v);
    void addVacanciesToCurrent(const QList<Vacancy> &vacancies);
    bool removeVacancyFromCurrent(const QString &vacancyId);
    Vacancy* findVacancyInCurrent(const QString &vacancyId);
    void updateVacancyInCurrent(const Vacancy &v);
    void clearCurrentVacancies();

    // Статистика
    QMap<QString, int> getSkillsStats() const;
    QMap<QString, int> getCompaniesStats() const;
    QList<int> getSalaryDistribution(int bins = 10) const;
    double getAverageSalary() const;
    int getVacanciesWithSalary() const;

    // Экспорт/Импорт
    bool exportToJson(const QString &filename) const;
    bool exportToCsv(const QString &filename) const;
    bool importFromJson(const QString &filename);

    // Поиск
    QList<Vacancy> searchInCurrent(const QString &query) const;
    QList<Vacancy> filterBySalary(int minSalary, int maxSalary) const;
    QList<Vacancy> filterByCompany(const QString &company) const;

signals:
    void profileCreated(const QString &name);
    void profileDeleted(const QString &name);
    void profileRenamed(const QString &oldName, const QString &newName);
    void profileLoaded(const QString &name, int vacancyCount);
    void profileChanged();
    void vacancyAdded(const Vacancy &v);
    void vacancyRemoved(const QString &id);
    void vacancyUpdated(const QString &id);
    void errorOccurred(const QString &error);

private:
    Controller() = default;
    ~Controller() = default;
    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;

    bool ensureProfilesDir();
    void saveCurrentProfile();

private:
    QList<Profile> m_profiles;
    Profile m_currentProfile;
};

#endif // CONTROLLER_H