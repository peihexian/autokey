#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>
#include "classprofile.h"

class ConfigManager {
public:
    ConfigManager();
    
    // Configuration file management
    bool loadConfiguration(const QString &filePath = "");
    bool saveConfiguration(const QString &filePath = "");
    QString getDefaultConfigPath() const;
    
    // Profile management
    QList<ClassProfile> getProfiles() const { return m_profiles; }
    void setProfiles(const QList<ClassProfile> &profiles) { m_profiles = profiles; }
    
    void addProfile(const ClassProfile &profile);
    void removeProfile(int index);
    void updateProfile(int index, const ClassProfile &profile);
    
    // Settings management
    QString getStartHotkey() const { return m_startHotkey; }
    QString getStopHotkey() const { return m_stopHotkey; }
    int getCurrentProfileIndex() const { return m_currentProfileIndex; }
    bool getMinimizeToTray() const { return m_minimizeToTray; }
    bool getStartMinimized() const { return m_startMinimized; }
    
    void setStartHotkey(const QString &hotkey) { m_startHotkey = hotkey; }
    void setStopHotkey(const QString &hotkey) { m_stopHotkey = hotkey; }
    void setCurrentProfileIndex(int index) { m_currentProfileIndex = index; }
    void setMinimizeToTray(bool minimize) { m_minimizeToTray = minimize; }
    void setStartMinimized(bool startMin) { m_startMinimized = startMin; }
    
    // Utility methods
    void createDefaultConfiguration();
    void resetToDefaults();
    
private:
    QList<ClassProfile> m_profiles;
    QString m_startHotkey;
    QString m_stopHotkey;
    int m_currentProfileIndex;
    bool m_minimizeToTray;
    bool m_startMinimized;
    
    // JSON conversion helpers
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
};

#endif // CONFIGMANAGER_H
