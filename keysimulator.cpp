#include "keysimulator.h"
#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QtCore>
#include <climits>

KeySimulator::KeySimulator(QObject *parent)
    : QObject(parent), m_isRunning(false) {
    // Initialize timer for smart simulation
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &KeySimulator::executeAction);
}

KeySimulator::~KeySimulator() {
    stopSimulation();
    unregisterGlobalHotkeys();
}

void KeySimulator::startSimulation(const ClassProfile &profile) {
    if (m_isRunning) {
        stopSimulation();
    }

    m_currentProfile = profile;
    m_isRunning = true;

    // Clear previous key states
    m_keyStates.clear();

    // Start timer with 50ms interval for smart simulation
    m_timer->start(50);

    qDebug() << "Smart key simulation started with profile:" << profile.getName();
    emit simulationStarted();
}



void KeySimulator::stopSimulation() {
    if (!m_isRunning) {
        return;
    }

    // Clear key states and stop timer
    m_keyStates.clear();
    m_timer->stop();
    m_isRunning = false;

    qDebug() << "Simulation stopped";
    emit simulationStopped();
}

bool KeySimulator::registerGlobalHotkeys() {
    // Get the main window handle safely
    QWidget *mainWindow = QApplication::activeWindow();
    if (!mainWindow) {
        // If no active window, try to get the first top-level widget
        QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
        if (!topLevelWidgets.isEmpty()) {
            mainWindow = topLevelWidgets.first();
        }
    }

    if (!mainWindow) {
        qDebug() << "No main window available for hotkey registration";
        return false;
    }

    HWND hwnd = reinterpret_cast<HWND>(mainWindow->winId());
    qDebug() << "Registering hotkeys for window handle:" << hwnd;

    // Register F5 as start hotkey (VK_F5 = 0x74)
    bool f5Success = RegisterHotKey(
        hwnd,
        HOTKEY_START,
        0, // No modifiers - this makes it work globally across all applications
        0x74 // VK_F5
    );

    // Register F6 as stop hotkey (VK_F6 = 0x75)
    bool f6Success = RegisterHotKey(
        hwnd,
        HOTKEY_STOP,
        0, // No modifiers - this makes it work globally across all applications
        0x75 // VK_F6
    );

    if (f5Success && f6Success) {
        qDebug() << "Global hotkeys registered successfully - F5:" << f5Success << "F6:" << f6Success;
        return true;
    } else {
        qDebug() << "Failed to register global hotkeys - F5:" << f5Success << "F6:" << f6Success;
        // Try to get more specific error information
        DWORD error = GetLastError();
        qDebug() << "Windows error code:" << error;
        unregisterGlobalHotkeys();
        return false;
    }
}

void KeySimulator::unregisterGlobalHotkeys() {
    // Get the main window handle safely
    QWidget *mainWindow = QApplication::activeWindow();
    if (!mainWindow) {
        // If no active window, try to get the first top-level widget
        QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
        if (!topLevelWidgets.isEmpty()) {
            mainWindow = topLevelWidgets.first();
        }
    }

    if (mainWindow) {
        HWND hwnd = reinterpret_cast<HWND>(mainWindow->winId());
        UnregisterHotKey(hwnd, HOTKEY_START);
        UnregisterHotKey(hwnd, HOTKEY_STOP);
    }
}



void KeySimulator::simulateKeyPress(int virtualKey) {
    INPUT inputs[2] = {};
    
    // Key down
    inputs[0] = createKeyInput(virtualKey, false);
    // Key up
    inputs[1] = createKeyInput(virtualKey, true);
    
    SendInput(2, inputs, sizeof(INPUT));
}

void KeySimulator::simulateMouseClick(bool leftClick) {
    INPUT inputs[2] = {};
    
    // Mouse down
    inputs[0] = createMouseInput(0, leftClick, false);
    // Mouse up
    inputs[1] = createMouseInput(0, leftClick, true);
    
    SendInput(2, inputs, sizeof(INPUT));
}

INPUT KeySimulator::createKeyInput(WORD virtualKey, bool keyUp) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = keyUp ? KEYEVENTF_KEYUP : 0;
    return input;
}

INPUT KeySimulator::createMouseInput(DWORD mouseData, bool leftButton, bool buttonUp) {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    
    if (leftButton) {
        input.mi.dwFlags = buttonUp ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN;
    } else {
        input.mi.dwFlags = buttonUp ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_RIGHTDOWN;
    }
    
    return input;
}



void KeySimulator::executeAction() {
    if (!m_isRunning) {
        return;
    }

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    QVector<int> weightedPool;

    // Create weighted pool - higher weight = more entries
    for (const auto& action : m_currentProfile.getActions()) {
        if (action.enabled && action.type == InputType::Keyboard) {
            // Check if enough time has passed since last press
            bool canPress = true;
            if (m_keyStates.contains(action.key)) {
                const auto& state = m_keyStates[action.key];
                qint64 timeSinceLastPress = currentTime - state.lastPressed;
                canPress = (timeSinceLastPress >= action.minInterval);
            }

            if (canPress) {
                // Use squared weight to make differences more dramatic
                // Weight 95 -> 95*95 = 9025 entries
                // Weight 40 -> 40*40 = 1600 entries
                // Weight 20 -> 20*20 = 400 entries
                int entries = action.weight * action.weight / 100;  // Scale down to reasonable numbers
                entries = qMax(1, entries);  // At least 1 entry

                for (int i = 0; i < entries; ++i) {
                    weightedPool.append(action.key);
                }
            }
        }
    }

    int selectedKey = 0;
    if (!weightedPool.isEmpty()) {
        // Randomly select from weighted pool
        int index = QRandomGenerator::global()->bounded(weightedPool.size());
        selectedKey = weightedPool[index];
    } else {
        // If no keys available, find the highest weight key
        int highestWeight = 0;
        for (const auto& action : m_currentProfile.getActions()) {
            if (action.enabled && action.type == InputType::Keyboard && action.weight > highestWeight) {
                highestWeight = action.weight;
                selectedKey = action.key;
            }
        }
    }

    if (selectedKey > 0) {
        simulateKeyPress(selectedKey);

        // Update the pressed key's state
        if (!m_keyStates.contains(selectedKey)) {
            m_keyStates[selectedKey] = SmartKeyState();
        }

        auto& state = m_keyStates[selectedKey];
        state.lastPressed = currentTime;
        state.isActive = true;
    }
}





QString KeySimulator::generateSequencePreview(const ClassProfile &profile, int length) {
    if (profile.getActions().isEmpty()) {
        return "No actions configured";
    }

    QString sequence;
    QMap<int, int> lastUsed;  // Track when each key was last used

    // Initialize last used times
    for (const auto& action : profile.getActions()) {
        if (action.enabled && action.type == InputType::Keyboard) {
            lastUsed[action.key] = -1000;  // Initialize to allow immediate use
        }
    }

    for (int i = 0; i < length; ++i) {
        QVector<int> weightedPool;

        // Create weighted pool based on availability and weight
        for (const auto& action : profile.getActions()) {
            if (action.enabled && action.type == InputType::Keyboard) {
                // Check if enough time has passed since last use
                int timeSinceLastUse = i - lastUsed[action.key];
                int minGap = action.minInterval / 50;  // Convert to steps (50ms base interval)

                if (timeSinceLastUse >= minGap) {
                    // Use squared weight to make differences more dramatic
                    int entries = action.weight * action.weight / 100;  // Scale down
                    entries = qMax(1, entries);  // At least 1 entry

                    for (int j = 0; j < entries; ++j) {
                        weightedPool.append(action.key);
                    }
                }
            }
        }

        int selectedKey = 0;
        if (!weightedPool.isEmpty()) {
            // Randomly select from weighted pool
            int index = QRandomGenerator::global()->bounded(weightedPool.size());
            selectedKey = weightedPool[index];
        } else {
            // If no keys available, use highest weight key
            int highestWeight = 0;
            for (const auto& action : profile.getActions()) {
                if (action.enabled && action.type == InputType::Keyboard && action.weight > highestWeight) {
                    highestWeight = action.weight;
                    selectedKey = action.key;
                }
            }
        }

        if (selectedKey > 0) {
            // Update last used time
            lastUsed[selectedKey] = i;

            // Convert virtual key to character for display
            char keyChar = '?';
            if (selectedKey >= 49 && selectedKey <= 57) {  // 1-9 keys
                keyChar = '0' + (selectedKey - 48);
            } else if (selectedKey >= 65 && selectedKey <= 90) {  // A-Z keys
                keyChar = 'A' + (selectedKey - 65);
            }

            sequence += keyChar;
        }
    }

    return sequence;
}


