// controller.cpp
#include "controller.h"

// ==================== Profile методы ====================

void Profile::saveMetadata() const {
    QFile file(folderPath + "/metadata.dat");
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << name << displayName << created << lastModified << quint32(vacancies.size());
        file.close();
    }
}

void Profile::loadMetadata() {
    QFile file(folderPath + "/metadata.dat");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        quint32 size;
        in >> name >> displayName >> created >> lastModified >> size;
        file.close();
    }
}

void Profile::saveVacancies() const {
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

void Profile::loadVacancies() {
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
        }
        file.close();
    }
}

void Profile::updateStats() {
    lastModified = QDateTime::currentDateTime();
    saveMetadata();
}

// ==================== Controller методы ====================

void Controller::scanProfiles() {
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
        if (p.created.isNull()) {
            p.created = QDateTime::currentDateTime();
            p.displayName = folder;
        }
        m_profiles.append(p);
    }
}

bool Controller::createProfile(const QString &name, const QString &displayName) {
    if (name.isEmpty()) {
        emit errorOccurred("Имя профиля не может быть пустым");
        return false;
    }

    for (const auto &p : m_profiles) {
        if (p.name == name) {
            emit errorOccurred("Профиль с таким именем уже существует");
            return false;
        }
    }

    if (!ensureProfilesDir()) return false;

    Profile newProfile;
    newProfile.name = name;
    newProfile.displayName = displayName.isEmpty() ? name : displayName;
    newProfile.folderPath = "profiles/" + name;
    newProfile.created = QDateTime::currentDateTime();
    newProfile.lastModified = newProfile.created;

    QDir dir;
    if (!dir.mkpath(newProfile.folderPath)) {
        emit errorOccurred("Не удалось создать папку профиля");
        return false;
    }

    newProfile.saveMetadata();
    m_profiles.append(newProfile);
    emit profileCreated(name);
    return true;
}

bool Controller::deleteProfile(const QString &name) {
    if (m_currentProfile.name == name) {
        emit errorOccurred("Нельзя удалить текущий профиль");
        return false;
    }

    for (int i = 0; i < m_profiles.size(); ++i) {
        if (m_profiles[i].name == name) {
            QDir dir(m_profiles[i].folderPath);
            if (!dir.removeRecursively()) {
                emit errorOccurred("Не удалось удалить папку профиля");
                return false;
            }
            m_profiles.removeAt(i);
            emit profileDeleted(name);
            return true;
        }
    }
    emit errorOccurred("Профиль не найден");
    return false;
}

bool Controller::renameProfile(const QString &oldName, const QString &newName) {
    if (oldName == newName) return true;

    for (const auto &p : m_profiles) {
        if (p.name == newName) {
            emit errorOccurred("Профиль с таким именем уже существует");
            return false;
        }
    }

    for (auto &p : m_profiles) {
        if (p.name == oldName) {
            QString oldPath = p.folderPath;
            QString newPath = "profiles/" + newName;

            QDir dir;
            if (!dir.rename(oldPath, newPath)) {
                emit errorOccurred("Не удалось переименовать папку");
                return false;
            }

            p.name = newName;
            p.displayName = newName;
            p.folderPath = newPath;
            p.updateStats();

            if (m_currentProfile.name == oldName) m_currentProfile = p;
            emit profileRenamed(oldName, newName);
            return true;
        }
    }
    emit errorOccurred("Профиль не найден");
    return false;
}

bool Controller::duplicateProfile(const QString &sourceName, const QString &newName) {
    Profile source;
    bool found = false;
    for (const auto &p : m_profiles) {
        if (p.name == sourceName) {
            source = p;
            found = true;
            break;
        }
    }
    if (!found) {
        emit errorOccurred("Исходный профиль не найден");
        return false;
    }

    if (!createProfile(newName, source.displayName + " (копия)")) return false;

    for (auto &p : m_profiles) {
        if (p.name == newName) {
            QString sourceFile = source.folderPath + "/vacancies.dat";
            QString destFile = p.folderPath + "/vacancies.dat";
            if (QFile::exists(sourceFile)) {
                QFile::copy(sourceFile, destFile);
                p.loadVacancies();
            }
            p.updateStats();
            break;
        }
    }
    return true;
}

bool Controller::loadProfile(const QString &name) {
    for (const auto &p : m_profiles) {
        if (p.name == name) {
            m_currentProfile = p;
            m_currentProfile.loadVacancies();
            m_currentProfile.loadMetadata();
            emit profileLoaded(name, m_currentProfile.vacancies.size());
            return true;
        }
    }
    emit errorOccurred("Профиль не найден");
    return false;
}

void Controller::addVacancyToCurrent(const Vacancy &v) {
    if (m_currentProfile.name.isEmpty()) {
        emit errorOccurred("Нет активного профиля");
        return;
    }

    for (const auto &existing : m_currentProfile.vacancies) {
        if (existing.getId() == v.getId()) return;
    }

    m_currentProfile.vacancies.append(v);
    m_currentProfile.updateStats();
    saveCurrentProfile();
    emit vacancyAdded(v);
    emit profileChanged();
}

void Controller::addVacanciesToCurrent(const QList<Vacancy> &vacancies) {
    if (m_currentProfile.name.isEmpty()) return;

    QSet<QString> existingIds;
    for (const auto &v : m_currentProfile.vacancies) existingIds.insert(v.getId());

    int added = 0;
    for (const auto &v : vacancies) {
        if (!existingIds.contains(v.getId())) {
            m_currentProfile.vacancies.append(v);
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

bool Controller::removeVacancyFromCurrent(const QString &vacancyId) {
    if (m_currentProfile.name.isEmpty()) return false;

    for (int i = 0; i < m_currentProfile.vacancies.size(); ++i) {
        if (m_currentProfile.vacancies[i].getId() == vacancyId) {
            m_currentProfile.vacancies.removeAt(i);
            m_currentProfile.updateStats();
            saveCurrentProfile();
            emit vacancyRemoved(vacancyId);
            emit profileChanged();
            return true;
        }
    }
    return false;
}

Vacancy* Controller::findVacancyInCurrent(const QString &vacancyId) {
    if (m_currentProfile.name.isEmpty()) return nullptr;
    for (auto &v : m_currentProfile.vacancies) {
        if (v.getId() == vacancyId) return &v;
    }
    return nullptr;
}

void Controller::updateVacancyInCurrent(const Vacancy &v) {
    if (m_currentProfile.name.isEmpty()) return;
    for (auto &existing : m_currentProfile.vacancies) {
        if (existing.getId() == v.getId()) {
            existing = v;
            m_currentProfile.updateStats();
            saveCurrentProfile();
            emit vacancyUpdated(v.getId());
            emit profileChanged();
            return;
        }
    }
}

void Controller::clearCurrentVacancies() {
    if (m_currentProfile.name.isEmpty()) return;
    m_currentProfile.vacancies.clear();
    m_currentProfile.updateStats();
    saveCurrentProfile();
    emit profileChanged();
}

QMap<QString, int> Controller::getSkillsStats() const {
    QMap<QString, int> stats;
    if (m_currentProfile.name.isEmpty()) return stats;

    QStringList commonSkills = {"C++", "Python", "Java", "JavaScript", "SQL",
                                "Git", "Linux", "Docker", "Qt", "C#", "PHP"};

    for (const auto &v : m_currentProfile.vacancies) {
        if (!v.hasFullData()) continue;
        QString desc = v.getDescription().toLower();
        QString name = v.getName().toLower();
        for (const auto &skill : commonSkills) {
            if (desc.contains(skill.toLower()) || name.contains(skill.toLower())) {
                stats[skill]++;
            }
        }
    }
    return stats;
}

QMap<QString, int> Controller::getCompaniesStats() const {
    QMap<QString, int> stats;
    for (const auto &v : m_currentProfile.vacancies) {
        QString company = v.getCompany();
        if (!company.isEmpty()) stats[company]++;
    }
    return stats;
}

QList<int> Controller::getSalaryDistribution(int bins) const {
    QList<int> salaries;
    for (const auto &v : m_currentProfile.vacancies) {
        if (v.hasSalary()) salaries.append(v.getAverageSalary());
    }
    if (salaries.isEmpty()) return QList<int>();

    std::sort(salaries.begin(), salaries.end());
    int minSalary = salaries.first();
    int maxSalary = salaries.last();
    int binSize = (maxSalary - minSalary) / bins + 1;

    QList<int> distribution(bins, 0);
    for (int salary : salaries) {
        int bin = (salary - minSalary) / binSize;
        if (bin >= 0 && bin < bins) distribution[bin]++;
    }
    return distribution;
}

double Controller::getAverageSalary() const {
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

int Controller::getVacanciesWithSalary() const {
    int count = 0;
    for (const auto &v : m_currentProfile.vacancies) {
        if (v.hasSalary()) count++;
    }
    return count;
}

bool Controller::exportToJson(const QString &filename) const {
    if (m_currentProfile.name.isEmpty()) return false;

    QJsonArray vacanciesArray;
    for (const auto &v : m_currentProfile.vacancies) {
        vacanciesArray.append(v.toJson());
    }

    QJsonObject root;
    root["profile_name"] = m_currentProfile.name;
    root["created"] = m_currentProfile.created.toString(Qt::ISODate);
    root["vacancies"] = vacanciesArray;
    root["count"] = static_cast<int>(m_currentProfile.vacancies.size());

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool Controller::exportToCsv(const QString &filename) const {
    if (m_currentProfile.name.isEmpty()) return false;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << "ID;Название;Компания;Зарплата от;Зарплата до;Валюта;URL;Дата;Описание;Адрес;Опыт\n";

    for (const auto &v : m_currentProfile.vacancies) {
        stream << v.getId() << ";"
               << v.getName() << ";"
               << v.getCompany() << ";"
               << v.getSalaryFrom() << ";"
               << v.getSalaryTo() << ";"
               << v.getSalaryCurrency() << ";"
               << v.getUrl() << ";"
               << v.getPublishedAt() << ";"
               << v.getDescription().left(100).replace(";", ",") << ";"
               << v.getAddress() << ";"
               << v.getExperience() << "\n";
    }
    file.close();
    return true;
}

bool Controller::importFromJson(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return false;

    QJsonObject root = doc.object();
    QJsonArray vacanciesArray = root["vacancies"].toArray();

    QList<Vacancy> imported;
    for (const auto &item : vacanciesArray) {
        if (item.isObject()) {
            imported.append(Vacancy::fromJson(item.toObject()));
        }
    }

    addVacanciesToCurrent(imported);
    return true;
}

QList<Vacancy> Controller::searchInCurrent(const QString &query) const {
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

QList<Vacancy> Controller::filterBySalary(int minSalary, int maxSalary) const {
    QList<Vacancy> results;
    for (const auto &v : m_currentProfile.vacancies) {
        if (!v.hasSalary()) continue;
        int avg = v.getAverageSalary();
        if (avg >= minSalary && avg <= maxSalary) results.append(v);
    }
    return results;
}

QList<Vacancy> Controller::filterByCompany(const QString &company) const {
    QList<Vacancy> results;
    QString lowerCompany = company.toLower();
    for (const auto &v : m_currentProfile.vacancies) {
        if (v.getCompany().toLower().contains(lowerCompany)) results.append(v);
    }
    return results;
}

bool Controller::ensureProfilesDir() {
    QDir dir;
    if (!dir.exists("profiles")) return dir.mkdir("profiles");
    return true;
}

void Controller::saveCurrentProfile() {
    if (m_currentProfile.name.isEmpty()) return;
    m_currentProfile.saveVacancies();
    m_currentProfile.saveMetadata();
    for (auto &p : m_profiles) {
        if (p.name == m_currentProfile.name) {
            p = m_currentProfile;
            break;
        }
    }
}