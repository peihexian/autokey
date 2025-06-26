#include "classprofile.h"
#include <QJsonDocument>

// KeyAction implementation
QJsonObject KeyAction::toJson() const {
    QJsonObject obj;
    obj["type"] = static_cast<int>(type);
    obj["key"] = key;
    obj["interval"] = interval;
    obj["enabled"] = enabled;
    obj["weight"] = weight;
    obj["minInterval"] = minInterval;
    obj["maxInterval"] = maxInterval;
    return obj;
}

void KeyAction::fromJson(const QJsonObject &json) {
    type = static_cast<InputType>(json["type"].toInt());
    key = json["key"].toInt();
    interval = json["interval"].toInt();
    enabled = json["enabled"].toBool();
    weight = json["weight"].toInt(50);  // Default to 50 if not present
    minInterval = json["minInterval"].toInt(50);  // Default to 50ms
    maxInterval = json["maxInterval"].toInt(1000);  // Default to 1000ms
}

// ClassProfile implementation
ClassProfile::ClassProfile() : m_name("New Profile"), m_enabled(true) {
}

ClassProfile::ClassProfile(const QString &name) : m_name(name), m_enabled(true) {
}

void ClassProfile::addAction(const KeyAction &action) {
    m_actions.append(action);
}

void ClassProfile::removeAction(int index) {
    if (index >= 0 && index < m_actions.size()) {
        m_actions.removeAt(index);
    }
}

void ClassProfile::updateAction(int index, const KeyAction &action) {
    if (index >= 0 && index < m_actions.size()) {
        m_actions[index] = action;
    }
}

void ClassProfile::clearActions() {
    m_actions.clear();
}

QJsonObject ClassProfile::toJson() const {
    QJsonObject obj;
    obj["name"] = m_name;
    obj["enabled"] = m_enabled;
    
    QJsonArray actionsArray;
    for (const auto &action : m_actions) {
        actionsArray.append(action.toJson());
    }
    obj["actions"] = actionsArray;
    
    return obj;
}

void ClassProfile::fromJson(const QJsonObject &json) {
    m_name = json["name"].toString();
    m_enabled = json["enabled"].toBool();
    
    m_actions.clear();
    QJsonArray actionsArray = json["actions"].toArray();
    for (const auto &value : actionsArray) {
        KeyAction action;
        action.fromJson(value.toObject());
        m_actions.append(action);
    }
}

// Default profiles
ClassProfile ClassProfile::createDefaultProfile1() {
    ClassProfile profile("Barbarian - Basic");
    profile.addAction(KeyAction(InputType::Keyboard, 0x32, 200)); // Key '2' every 200ms
    profile.addAction(KeyAction(InputType::Keyboard, 0x33, 300)); // Key '3' every 300ms
    profile.addAction(KeyAction(InputType::Keyboard, 0x34, 400)); // Key '4' every 400ms
    return profile;
}

ClassProfile ClassProfile::createDefaultProfile2() {
    ClassProfile profile("Wizard - Combo");
    profile.addAction(KeyAction(InputType::Keyboard, 0x31, 150)); // Key '1' every 150ms
    profile.addAction(KeyAction(InputType::Keyboard, 0x32, 200)); // Key '2' every 200ms
    profile.addAction(KeyAction(InputType::Keyboard, 0x33, 250)); // Key '3' every 250ms
    profile.addAction(KeyAction(InputType::Keyboard, 0x34, 300)); // Key '4' every 300ms
    profile.addAction(KeyAction(InputType::MouseRight, 0, 100));  // Right click every 100ms
    return profile;
}

ClassProfile ClassProfile::createDefaultProfile3() {
    ClassProfile profile("Demon Hunter - Rapid");
    profile.addAction(KeyAction(InputType::MouseLeft, 0, 500));    // Left click every 0.5 seconds
    profile.addAction(KeyAction(InputType::Keyboard, 0x32, 1800)); // Key '2' every 1.8 seconds
    profile.addAction(KeyAction(InputType::Keyboard, 0x33, 2200)); // Key '3' every 2.2 seconds
    profile.addAction(KeyAction(InputType::MouseRight, 0, 800));   // Right click every 0.8 seconds
    return profile;
}

ClassProfile ClassProfile::createDefaultProfile4() {
    ClassProfile profile("Monk - Balanced");
    profile.addAction(KeyAction(InputType::Keyboard, 0x31, 1200)); // Key '1' every 1.2 seconds
    profile.addAction(KeyAction(InputType::Keyboard, 0x32, 1800)); // Key '2' every 1.8 seconds
    profile.addAction(KeyAction(InputType::Keyboard, 0x33, 2400)); // Key '3' every 2.4 seconds
    profile.addAction(KeyAction(InputType::Keyboard, 0x34, 3600)); // Key '4' every 3.6 seconds
    return profile;
}

ClassProfile ClassProfile::createDefaultProfile5() {
    ClassProfile profile("Necromancer - Pet Build");
    profile.addAction(KeyAction(InputType::Keyboard, 0x31, 5000)); // Key '1' every 5 seconds
    profile.addAction(KeyAction(InputType::Keyboard, 0x32, 3000)); // Key '2' every 3 seconds
    profile.addAction(KeyAction(InputType::Keyboard, 0x33, 8000)); // Key '3' every 8 seconds
    profile.addAction(KeyAction(InputType::MouseRight, 0, 1500));  // Right click every 1.5 seconds
    return profile;
}
