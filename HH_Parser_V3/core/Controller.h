#pragma once

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
#include "Vacancy.h"
#include "GeocoderWorker.h"  // ← добавить!

struct Profile {
    QString name;
    QString displayName;
    QString folderPath;
    QDateTime created;
    QDateTime lastModified;
    QList<Vacancy> vacancies;
    QHash<QString, Point> vacancyCoords;

    bool isEmpty() const { return name.isEmpty(); }

    void saveMetadata() const {
        QFile file(folderPath + "/metadata.dat");
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out << name << displayName << created << lastModified << quint32(vacancies.size());
            file.close();
        }
    }

    void loadMetadata() {
        QFile file(folderPath + "/metadata.dat");
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            quint32 size;
            in >> name >> displayName >> created >> lastModified >> size;
            file.close();
        }
    }

    void saveVacancies() const {
        if (vacancies.isEmpty()) return;
        QFile file(folderPath + "/vacancies.dat");
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out << quint32(Vacancy::SERIALIZATION_VERSION);
            out << quint32(vacancies.size());
            for (const auto &v : vacancies) out << v;
            file.close();
        }
    }

    void loadVacancies() {
        QFile file(folderPath + "/vacancies.dat");
        if (!file.exists()) return;
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            quint32 version, size;
            in >> version >> size;
            vacancies.clear();
            vacancies.reserve(size);
            for (quint32 i = 0; i < size; ++i) {
                Vacancy v;
                in >> v;
                vacancies.append(v);
                if (v.hasCoords()) {
                    vacancyCoords[v.getId()] = v.getLocation();
                }
            }
            file.close();
        }
    }

    void saveCoords() const {
        if (vacancyCoords.isEmpty()) return;
        QFile file(folderPath + "/coords.dat");
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out << quint32(vacancyCoords.size());
            for (auto it = vacancyCoords.constBegin(); it != vacancyCoords.constEnd(); ++it) {
                out << it.key() << it.value().getX() << it.value().getY();
            }
            file.close();
        }
    }

    void loadCoords() {
        QFile file(folderPath + "/coords.dat");
        if (!file.exists()) return;
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            quint32 size;
            in >> size;
            vacancyCoords.clear();
            vacancyCoords.reserve(size);
            for (quint32 i = 0; i < size; ++i) {
                QString id;
                double lat, lon;
                in >> id >> lat >> lon;
                vacancyCoords[id] = Point(lat, lon);
            }
            file.close();
        }
    }

    void updateStats() {
        lastModified = QDateTime::currentDateTime();
        saveMetadata();
    }
};

class Controller : public QObject {
    Q_OBJECT

public:
    static Controller& instance() {
        static Controller controller;
        return controller;
    }

    // ===== ДОСТУП К ПРОФИЛЯМ =====
    QList<Profile> getAllProfiles() const { return m_profiles; }
    Profile* getCurrentProfile() { return m_currentProfile.isEmpty() ? nullptr : &m_currentProfile; }

    GeocoderWorker* geocoderWorker() { return m_geocoderWorker; }

    // ===== СКАНИРОВАНИЕ ПРОФИЛЕЙ =====
    void scanProfiles() {
        m_profiles.clear();
        QDir dir("profiles");
        if (!dir.exists()) {
            dir.mkpath(".");
            return;
        }

        auto folders = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto &folder : folders) {
            Profile p;
            p.name = folder;
            p.folderPath = "profiles/" + folder;
            p.loadMetadata();
            p.loadVacancies();
            p.loadCoords();
            if (p.created.isNull()) {
                p.created = QDateTime::currentDateTime();
                p.displayName = folder;
            }
            m_profiles.append(p);
        }
    }

    // ===== ЗАГРУЗКА ПРОФИЛЯ =====
    bool loadProfile(const QString &name) {
        for (const auto &p : m_profiles) {
            if (p.name == name) {
                m_currentProfile = p;
                m_currentProfile.loadVacancies();
                m_currentProfile.loadMetadata();
                m_currentProfile.loadCoords();
                emit profileLoaded(name, m_currentProfile.vacancies.size());
                return true;
            }
        }
        emit errorOccurred("Профиль не найден");
        return false;
    }

    // ===== РАБОТА С ВАКАНСИЯМИ =====
    void addVacancyToCurrent(const Vacancy &v) {
        if (m_currentProfile.name.isEmpty()) {
            emit errorOccurred("Нет активного профиля");
            return;
        }

        for (const auto &existing : m_currentProfile.vacancies) {
            if (existing.getId() == v.getId()) return;
        }

        m_currentProfile.vacancies.append(v);
        if (v.hasCoords()) {
            m_currentProfile.vacancyCoords[v.getId()] = v.getLocation();
        }
        m_currentProfile.updateStats();
        saveCurrentProfile();
        emit vacancyAdded(v);
        emit profileChanged();
    }

    void addVacanciesToCurrent(const QList<Vacancy> &vacancies) {
        if (m_currentProfile.name.isEmpty()) return;

        QSet<QString> existingIds;
        for (const auto &v : m_currentProfile.vacancies) existingIds.insert(v.getId());

        int added = 0;
        for (const auto &v : vacancies) {
            if (!existingIds.contains(v.getId())) {
                m_currentProfile.vacancies.append(v);
                if (v.hasCoords()) {
                    m_currentProfile.vacancyCoords[v.getId()] = v.getLocation();
                }
                existingIds.insert(v.getId());
                added++;
                emit vacancyAdded(v);
            }
        }

        if (added > 0) {
            m_currentProfile.updateStats();
            saveCurrentProfile();
            emit profileChanged();
        }
    }

    bool removeVacancyFromCurrent(const QString &vacancyId) {
        if (m_currentProfile.name.isEmpty()) return false;

        for (int i = 0; i < m_currentProfile.vacancies.size(); ++i) {
            if (m_currentProfile.vacancies[i].getId() == vacancyId) {
                m_currentProfile.vacancies.removeAt(i);
                m_currentProfile.vacancyCoords.remove(vacancyId);
                m_currentProfile.updateStats();
                saveCurrentProfile();
                emit vacancyRemoved(vacancyId);
                emit profileChanged();
                return true;
            }
        }
        return false;
    }

    Vacancy* findVacancyInCurrent(const QString &vacancyId) {
        if (m_currentProfile.name.isEmpty()) return nullptr;
        for (auto &v : m_currentProfile.vacancies) {
            if (v.getId() == vacancyId) return &v;
        }
        return nullptr;
    }

    void updateVacancyInCurrent(const Vacancy &v) {
        if (m_currentProfile.name.isEmpty()) return;
        for (auto &existing : m_currentProfile.vacancies) {
            if (existing.getId() == v.getId()) {
                existing = v;
                if (v.hasCoords()) {
                    m_currentProfile.vacancyCoords[v.getId()] = v.getLocation();
                } else {
                    m_currentProfile.vacancyCoords.remove(v.getId());
                }
                m_currentProfile.updateStats();
                saveCurrentProfile();
                emit vacancyUpdated(v.getId());
                emit profileChanged();
                return;
            }
        }
    }

    void clearCurrentVacancies() {
        if (m_currentProfile.name.isEmpty()) return;
        m_currentProfile.vacancies.clear();
        m_currentProfile.vacancyCoords.clear();
        m_currentProfile.updateStats();
        saveCurrentProfile();
        emit profileChanged();
    }

    // ===== КООРДИНАТЫ =====
    void setVacancyCoords(const QString &vacancyId, double lat, double lon) {
        if (m_currentProfile.name.isEmpty()) return;
        m_currentProfile.vacancyCoords[vacancyId] = Point(lat, lon);

        for (auto &v : m_currentProfile.vacancies) {
            if (v.getId() == vacancyId) {
                v.setLocation(lat, lon);
                break;
            }
        }

        m_currentProfile.saveCoords();
        saveCurrentProfile();
    }

    bool getVacancyCoords(const QString &vacancyId, double &lat, double &lon) const {
        if (m_currentProfile.name.isEmpty()) return false;
        auto it = m_currentProfile.vacancyCoords.find(vacancyId);
        if (it == m_currentProfile.vacancyCoords.end()) return false;
        lat = it->getX();
        lon = it->getY();
        return true;
    }

    QList<Vacancy> getVacanciesWithCoords() const {
        QList<Vacancy> result;
        if (m_currentProfile.name.isEmpty()) return result;
        for (const auto &v : m_currentProfile.vacancies) {
            if (v.hasCoords()) {
                result.append(v);
            }
        }
        return result;
    }

    // ===== СТАТИСТИКА =====
    double getAverageSalary() const {
        long long total = 0;
        int count = 0;
        for (const auto &v : m_currentProfile.vacancies) {
            if (v.hasSalary()) {
                total += v.getAverageSalary();
                count++;
            }
        }
        return count > 0 ? static_cast<double>(total) / count : 0.0;
    }

    int getVacanciesWithSalary() const {
        int count = 0;
        for (const auto &v : m_currentProfile.vacancies) {
            if (v.hasSalary()) count++;
        }
        return count;
    }

    // ===== ПОИСК =====
    QList<Vacancy> searchInCurrent(const QString &query) const {
        QList<Vacancy> results;
        QString lowerQuery = query.toLower();
        for (const auto &v : m_currentProfile.vacancies) {
            if (v.getName().toLower().contains(lowerQuery) ||
                v.getCompany().toLower().contains(lowerQuery) ||
                v.getDescription().toLower().contains(lowerQuery)) {
                results.append(v);
            }
        }
        return results;
    }

    QList<Vacancy> filterBySalary(int minSalary, int maxSalary) const {
        QList<Vacancy> results;
        for (const auto &v : m_currentProfile.vacancies) {
            if (!v.hasSalary()) continue;
            int avg = v.getAverageSalary();
            if (avg >= minSalary && avg <= maxSalary) results.append(v);
        }
        return results;
    }

    QList<Vacancy> filterByCompany(const QString &company) const {
        QList<Vacancy> results;
        QString lowerCompany = company.toLower();
        for (const auto &v : m_currentProfile.vacancies) {
            if (v.getCompany().toLower().contains(lowerCompany)) results.append(v);
        }
        return results;
    }

signals:
    void profileLoaded(const QString &name, int vacancyCount);
    void profileChanged();
    void vacancyAdded(const Vacancy &v);
    void vacancyRemoved(const QString &id);
    void vacancyUpdated(const QString &id);
    void errorOccurred(const QString &error);

private slots:
    void onVacancyGeocoded(const QString& id, double lat, double lon) {
        Vacancy* v = findVacancyInCurrent(id);
        if (v) {
            v->setLocation(lat, lon);
            m_currentProfile.vacancyCoords[id] = Point(lat, lon);
            saveCurrentProfile();
            emit vacancyUpdated(id);

            qDebug() << "✅ Координаты получены для" << id << lat << lon;
        }
    }

private:
    Controller() {
        QDir().mkpath("profiles");
        m_geocoderWorker = new GeocoderWorker(this, this);  // ← передаём Controller*
        connect(m_geocoderWorker, &GeocoderWorker::vacancyGeocoded,
                this, &Controller::onVacancyGeocoded);
    }
    ~Controller() = default;
    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;

    void saveCurrentProfile() {
        if (m_currentProfile.name.isEmpty()) return;
        m_currentProfile.saveVacancies();
        m_currentProfile.saveMetadata();
        m_currentProfile.saveCoords();
        for (auto &p : m_profiles) {
            if (p.name == m_currentProfile.name) {
                p = m_currentProfile;
                break;
            }
        }
    }

private:
    QList<Profile> m_profiles;
    Profile m_currentProfile;
    GeocoderWorker* m_geocoderWorker;  // ← добавить!
};
