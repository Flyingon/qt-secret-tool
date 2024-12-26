//
// Created by 袁兆祎 on 2024/12/26.
//

#include "settingmanager.h"

SettingsManager::SettingsManager() : settings("MyOrg", "MyApp") {}

SettingsManager& SettingsManager::getInstance()
{
    static SettingsManager instance;
    return instance;
}

void SettingsManager::setValue(const QString& key, const QVariant& value)
{
    settings.setValue(key, value);
}

QVariant SettingsManager::value(const QString& key) const
{
    return settings.value(key);
}

void SettingsManager::remove(const QString& key)
{
    settings.remove(key);
}
