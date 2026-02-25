// ProfileManager.cpp
#include "ProfileManager.h"
#include <QDir>
#include <QFile>
#include <QDateTime>

ProfileManager::ProfileManager(Controller* controller, QObject* parent)
    : QObject(parent), m_controller(controller) {}

bool ProfileManager::createProfile(const QString &name, const QString &displayName) {
    if (name.isEmpty()) {
        emit errorOccurred("Имя профиля не может быть пустым");
        return false;
    }

    // Проверяем, нет ли уже такого профиля
    for (const auto &p : m_controller->getAllProfiles()) {
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

    // Обновляем список профилей в контроллере
    m_controller->scanProfiles();
    emit profileCreated(name);
    return true;
}

bool ProfileManager::deleteProfile(const QString &name) {
    auto* current = m_controller->getCurrentProfile();
    if (current && current->name == name) {
        emit errorOccurred("Нельзя удалить текущий профиль");
        return false;
    }

    QDir dir("profiles/" + name);
    if (!dir.removeRecursively()) {
        emit errorOccurred("Не удалось удалить папку профиля");
        return false;
    }

    m_controller->scanProfiles();
    emit profileDeleted(name);
    return true;
}

bool ProfileManager::renameProfile(const QString &oldName, const QString &newName) {
    if (oldName == newName) return true;

    // Проверяем, нет ли уже профиля с новым именем
    for (const auto &p : m_controller->getAllProfiles()) {
        if (p.name == newName) {
            emit errorOccurred("Профиль с таким именем уже существует");
            return false;
        }
    }

    QDir dir;
    if (!dir.rename("profiles/" + oldName, "profiles/" + newName)) {
        emit errorOccurred("Не удалось переименовать папку");
        return false;
    }

    m_controller->scanProfiles();
    emit profileRenamed(oldName, newName);
    return true;
}

bool ProfileManager::duplicateProfile(const QString &sourceName, const QString &newName) {
    // Находим исходный профиль
    Profile source;
    bool found = false;
    for (const auto &p : m_controller->getAllProfiles()) {
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

    // Копируем вакансии
    QString sourceFile = source.folderPath + "/vacancies.dat";
    QString destFile = "profiles/" + newName + "/vacancies.dat";
    if (QFile::exists(sourceFile)) {
        QFile::copy(sourceFile, destFile);
    }

    m_controller->scanProfiles();
    return true;
}

bool ProfileManager::loadProfile(const QString &name) {
    return m_controller->loadProfile(name);
}

void ProfileManager::scanProfiles() {
    m_controller->scanProfiles();
}

bool ProfileManager::ensureProfilesDir() {
    QDir dir;
    if (!dir.exists("profiles")) return dir.mkdir("profiles");
    return true;
}