#ifndef KEYSIMULATOR_H
#define KEYSIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <windows.h>
#include "classprofile.h"

class KeySimulator : public QObject {
    Q_OBJECT

public:
    explicit KeySimulator(QObject *parent = nullptr);
    ~KeySimulator();
    
    // Main control methods
    void startSimulation(const ClassProfile &profile);
    void stopSimulation();
    bool isRunning() const { return m_isRunning; }
    
    // Global hotkey management
    bool registerGlobalHotkeys();
    void unregisterGlobalHotkeys();
    
    // Static key simulation methods
    static void simulateKeyPress(int virtualKey);
    static void simulateMouseClick(bool leftClick);
    
signals:
    void simulationStarted();
    void simulationStopped();
    void hotkeyPressed(int hotkeyId);
    
protected:
    // Windows message handling for global hotkeys
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result);
    
private slots:
    void executeAction();
    
private:
    struct ActionTimer {
        QTimer *timer;
        KeyAction action;
        int actionIndex;
    };
    
    bool m_isRunning;
    ClassProfile m_currentProfile;
    QList<ActionTimer> m_actionTimers;
    
    // Global hotkey IDs
    static const int HOTKEY_START = 1;
    static const int HOTKEY_STOP = 2;
    
    // Helper methods
    void setupActionTimers();
    void clearActionTimers();
    void executeKeyAction(const KeyAction &action);
    
    // Windows API helpers
    static INPUT createKeyInput(WORD virtualKey, bool keyUp = false);
    static INPUT createMouseInput(DWORD mouseData, bool leftButton, bool buttonUp = false);
};

#endif // KEYSIMULATOR_H
