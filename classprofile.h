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
    
    KeyAction() : type(InputType::Keyboard), key(0), interval(1000), enabled(true) {}
    KeyAction(InputType t, int k, int i, bool e = true) 
        : type(t), key(k), interval(i), enabled(e) {}
    
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
