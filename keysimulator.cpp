#include "keysimulator.h"
#include <QDebug>
#include <QApplication>
#include <QWidget>

KeySimulator::KeySimulator(QObject *parent) 
    : QObject(parent), m_isRunning(false) {
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
    
    setupActionTimers();
    
    qDebug() << "Simulation started for profile:" << profile.getName();
    emit simulationStarted();
}

void KeySimulator::stopSimulation() {
    if (!m_isRunning) {
        return;
    }
    
    m_isRunning = false;
    clearActionTimers();
    
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

void KeySimulator::setupActionTimers() {
    clearActionTimers();
    
    const auto &actions = m_currentProfile.getActions();
    for (int i = 0; i < actions.size(); ++i) {
        const auto &action = actions[i];
        if (!action.enabled) {
            continue;
        }
        
        ActionTimer actionTimer;
        actionTimer.timer = new QTimer(this);
        actionTimer.action = action;
        actionTimer.actionIndex = i;
        
        connect(actionTimer.timer, &QTimer::timeout, this, &KeySimulator::executeAction);
        
        actionTimer.timer->start(action.interval);
        m_actionTimers.append(actionTimer);
    }
}

void KeySimulator::clearActionTimers() {
    for (auto &actionTimer : m_actionTimers) {
        actionTimer.timer->stop();
        actionTimer.timer->deleteLater();
    }
    m_actionTimers.clear();
}

void KeySimulator::executeAction() {
    QTimer *senderTimer = qobject_cast<QTimer*>(sender());
    if (!senderTimer || !m_isRunning) {
        return;
    }
    
    // Find the corresponding action
    for (const auto &actionTimer : m_actionTimers) {
        if (actionTimer.timer == senderTimer) {
            executeKeyAction(actionTimer.action);
            break;
        }
    }
}

void KeySimulator::executeKeyAction(const KeyAction &action) {
    switch (action.type) {
        case InputType::Keyboard:
            simulateKeyPress(action.key);
            break;
        case InputType::MouseLeft:
            simulateMouseClick(true);
            break;
        case InputType::MouseRight:
            simulateMouseClick(false);
            break;
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
