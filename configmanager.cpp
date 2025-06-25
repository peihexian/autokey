#include "configmanager.h"
#include <QFile>
#include <QJsonArray>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

ConfigManager::ConfigManager()
    : m_startHotkey("F5")
    , m_stopHotkey("F6")
    , m_currentProfileIndex(0)
    , m_minimizeToTray(true)
    , m_startMinimized(false) {
}

QString ConfigManager::getDefaultConfigPath() const {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configDir); // Ensure directory exists
    return configDir + "/config.json";
}

bool ConfigManager::loadConfiguration(const QString &filePath) {
    QString actualPath = filePath.isEmpty() ? getDefaultConfigPath() : filePath;

    QFile file(actualPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open config file for reading:" << actualPath;
        createDefaultConfiguration();
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Invalid JSON in config file";
        createDefaultConfiguration();
        return false;
    }
    
    fromJson(doc.object());
    qDebug() << "Configuration loaded successfully from:" << actualPath;
    return true;
}

bool ConfigManager::saveConfiguration(const QString &filePath) {
    QString actualPath = filePath.isEmpty() ? getDefaultConfigPath() : filePath;

    QJsonDocument doc(toJson());

    QFile file(actualPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not open config file for writing:" << actualPath;
        return false;
    }

    file.write(doc.toJson());
    qDebug() << "Configuration saved successfully to:" << actualPath;
    return true;
}

void ConfigManager::addProfile(const ClassProfile &profile) {
    m_profiles.append(profile);
}

void ConfigManager::removeProfile(int index) {
    if (index >= 0 && index < m_profiles.size()) {
        m_profiles.removeAt(index);
        
        // Adjust current profile index if necessary
        if (m_currentProfileIndex >= m_profiles.size()) {
            m_currentProfileIndex = qMax(0, m_profiles.size() - 1);
        }
    }
}

void ConfigManager::updateProfile(int index, const ClassProfile &profile) {
    if (index >= 0 && index < m_profiles.size()) {
        m_profiles[index] = profile;
    }
}

void ConfigManager::createDefaultConfiguration() {
    m_profiles.clear();
    
    // Add default profiles
    m_profiles.append(ClassProfile::createDefaultProfile1());
    m_profiles.append(ClassProfile::createDefaultProfile2());
    m_profiles.append(ClassProfile::createDefaultProfile3());
    m_profiles.append(ClassProfile::createDefaultProfile4());
    m_profiles.append(ClassProfile::createDefaultProfile5());
    
    m_startHotkey = "F5";
    m_stopHotkey = "F6";
    m_currentProfileIndex = 0;
    m_minimizeToTray = true;
    m_startMinimized = false;
    
    qDebug() << "Default configuration created";
}

void ConfigManager::resetToDefaults() {
    createDefaultConfiguration();
}

QJsonObject ConfigManager::toJson() const {
    QJsonObject obj;
    
    // Settings
    obj["startHotkey"] = m_startHotkey;
    obj["stopHotkey"] = m_stopHotkey;
    obj["currentProfileIndex"] = m_currentProfileIndex;
    obj["minimizeToTray"] = m_minimizeToTray;
    obj["startMinimized"] = m_startMinimized;
    
    // Profiles
    QJsonArray profilesArray;
    for (const auto &profile : m_profiles) {
        profilesArray.append(profile.toJson());
    }
    obj["profiles"] = profilesArray;
    
    // Metadata
    obj["version"] = "1.0.0";
    obj["appName"] = "AutoKey for Diablo 3";
    
    return obj;
}

void ConfigManager::fromJson(const QJsonObject &json) {
    // Settings
    m_startHotkey = json["startHotkey"].toString("F5");
    m_stopHotkey = json["stopHotkey"].toString("F6");
    m_currentProfileIndex = json["currentProfileIndex"].toInt(0);
    m_minimizeToTray = json["minimizeToTray"].toBool(true);
    m_startMinimized = json["startMinimized"].toBool(false);
    
    // Profiles
    m_profiles.clear();
    QJsonArray profilesArray = json["profiles"].toArray();
    for (const auto &value : profilesArray) {
        ClassProfile profile;
        profile.fromJson(value.toObject());
        m_profiles.append(profile);
    }
    
    // Ensure we have at least one profile
    if (m_profiles.isEmpty()) {
        createDefaultConfiguration();
    }
    
    // Validate current profile index
    if (m_currentProfileIndex >= m_profiles.size()) {
        m_currentProfileIndex = 0;
    }
}
