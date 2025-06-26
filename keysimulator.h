#ifndef KEYSIMULATOR_H
#define KEYSIMULATOR_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QDateTime>
#include <QRandomGenerator>
#include <QVector>
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

    // Sequence preview
    QString generateSequencePreview(const ClassProfile &profile, int length = 50);
    
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
    // Smart key state tracking
    struct SmartKeyState {
        qint64 lastPressed;
        bool isActive;

        SmartKeyState() : lastPressed(0), isActive(true) {}
    };

    bool m_isRunning;
    ClassProfile m_currentProfile;
    QMap<int, SmartKeyState> m_keyStates;
    QTimer *m_timer;
    
    // Global hotkey IDs
    static const int HOTKEY_START = 1;
    static const int HOTKEY_STOP = 2;
    
    // Helper methods
    
    // Windows API helpers
    static INPUT createKeyInput(WORD virtualKey, bool keyUp = false);
    static INPUT createMouseInput(DWORD mouseData, bool leftButton, bool buttonUp = false);
};

#endif // KEYSIMULATOR_H
