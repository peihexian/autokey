#ifndef CLASSPROFILE_H
#define CLASSPROFILE_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

enum class InputType {
    Keyboard,
    MouseLeft,
    MouseRight
};

struct KeyAction {
    InputType type;
    int key;           // Virtual key code for keyboard, 0 for mouse
    int interval;      // Interval in milliseconds
    bool enabled;      // Whether this action is enabled
    int weight;        // Weight for smart simulation (1-100)
    int minInterval;   // Minimum interval for smart simulation
    int maxInterval;   // Maximum interval for smart simulation

    KeyAction() : type(InputType::Keyboard), key(0), interval(1000), enabled(true),
                  weight(50), minInterval(50), maxInterval(1000) {}
    KeyAction(InputType t, int k, int i, bool e = true, int w = 50, int minInt = 50, int maxInt = 1000)
        : type(t), key(k), interval(i), enabled(e), weight(w), minInterval(minInt), maxInterval(maxInt) {}

    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
};

class ClassProfile {
public:
    ClassProfile();
    ClassProfile(const QString &name);
    
    // Getters
    QString getName() const { return m_name; }
    QList<KeyAction> getActions() const { return m_actions; }
    bool isEnabled() const { return m_enabled; }
    
    // Setters
    void setName(const QString &name) { m_name = name; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    void setActions(const QList<KeyAction> &actions) { m_actions = actions; }
    
    // Action management
    void addAction(const KeyAction &action);
    void removeAction(int index);
    void updateAction(int index, const KeyAction &action);
    void clearActions();
    
    // JSON serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
    
    // Static methods for default profiles
    static ClassProfile createDefaultProfile1();
    static ClassProfile createDefaultProfile2();
    static ClassProfile createDefaultProfile3();
    static ClassProfile createDefaultProfile4();
    static ClassProfile createDefaultProfile5();
    
private:
    QString m_name;
    QList<KeyAction> m_actions;
    bool m_enabled;
};

#endif // CLASSPROFILE_H
