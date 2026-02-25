// ProfileManager.h
#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QObject>
#include <QString>
#include "../core/Controller.h"

class ProfileManager : public QObject {
    Q_OBJECT

public:
    explicit ProfileManager(Controller* controller, QObject* parent = nullptr);

    // Управление профилями
    bool createProfile(const QString &name, const QString &displayName = "");
    bool deleteProfile(const QString &name);
    bool renameProfile(const QString &oldName, const QString &newName);
    bool duplicateProfile(const QString &sourceName, const QString &newName);
    bool loadProfile(const QString &name);
    void scanProfiles();

    signals:
        void profileCreated(const QString &name);
    void profileDeleted(const QString &name);
    void profileRenamed(const QString &oldName, const QString &newName);
    void errorOccurred(const QString &error);

private:
    bool ensureProfilesDir();

private:
    Controller* m_controller;
};

#endif // PROFILEMANAGER_H