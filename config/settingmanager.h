//
// Created by 袁兆祎 on 2024/12/26.
//

#ifndef QT_SECRET_TOOL_SETTINGMANAGER_H
#define QT_SECRET_TOOL_SETTINGMANAGER_H

#include <QString>
#include <QSettings>

const QString dbPathKey = "databasePath";

class SettingsManager {
public:
    static SettingsManager &getInstance();

    void setValue(const QString &key, const QVariant &value);

    QVariant value(const QString &key) const;

    void remove(const QString &key);

private:
    QSettings settings;

    SettingsManager();

    SettingsManager(const SettingsManager &) = delete;

    SettingsManager &operator=(const SettingsManager &) = delete;
};


#endif //QT_SECRET_TOOL_SETTINGMANAGER_H
