#include "mainwindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QCheckBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QProgressBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QShowEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QStandardPaths>
#include <QDebug>
#include <windows.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_configManager(new ConfigManager())
    , m_keySimulator(new KeySimulator(this))
    , m_isSimulationRunning(false)
    , m_statusUpdateTimer(new QTimer(this)) {
    
    setupUI();
    setupSystemTray();
    setupMenuBar();
    setupStatusBar();
    setupConnections();
    
    // Load configuration
    m_configManager->loadConfiguration();

    // DEBUG: Add breakpoint here to check if UI is properly initialized
    qDebug() << "About to call updateUI()";
    updateUI();
    qDebug() << "updateUI() completed";

    // Delay hotkey registration until after the window is shown
    // We'll register hotkeys in showEvent() instead
    
    // Setup status update timer
    m_statusUpdateTimer->setInterval(1000); // Update every second
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
    m_statusUpdateTimer->start();
    
    setWindowTitle("AutoKey for Diablo 3 v1.0");

    // Set window icon - try custom icon first, fallback to built-in
    QIcon windowIcon(":/icons/keyboard.ico");
    if (windowIcon.isNull()) {
        windowIcon = style()->standardIcon(QStyle::SP_ComputerIcon);
    }
    setWindowIcon(windowIcon);

    resize(800, 600);
}

MainWindow::~MainWindow() {
    m_keySimulator->stopSimulation();
    m_keySimulator->unregisterGlobalHotkeys();

    // Also unregister hotkeys from this window
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    UnregisterHotKey(hwnd, 1);
    UnregisterHotKey(hwnd, 2);

    m_configManager->saveConfiguration();
}

void MainWindow::setupUI() {
    m_tabWidget = new QTabWidget(this);
    setCentralWidget(m_tabWidget);
    
    createProfileTab();
    createActionsTab();
    createSettingsTab();
    createAboutTab();
}

void MainWindow::createProfileTab() {
    QWidget *profileWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(profileWidget);
    
    // Profile selection group
    QGroupBox *selectionGroup = new QGroupBox("Profile Selection");
    QGridLayout *selectionLayout = new QGridLayout(selectionGroup);
    
    selectionLayout->addWidget(new QLabel("Current Profile:"), 0, 0);
    m_profileComboBox = new QComboBox();
    selectionLayout->addWidget(m_profileComboBox, 0, 1, 1, 2);
    
    m_addProfileButton = new QPushButton("Add");
    m_removeProfileButton = new QPushButton("Remove");
    m_editProfileButton = new QPushButton("Edit");
    m_duplicateProfileButton = new QPushButton("Duplicate");
    
    selectionLayout->addWidget(m_addProfileButton, 1, 0);
    selectionLayout->addWidget(m_removeProfileButton, 1, 1);
    selectionLayout->addWidget(m_editProfileButton, 1, 2);
    selectionLayout->addWidget(m_duplicateProfileButton, 1, 3);
    
    layout->addWidget(selectionGroup);
    
    // Profile details group
    QGroupBox *detailsGroup = new QGroupBox("Profile Details");
    QGridLayout *detailsLayout = new QGridLayout(detailsGroup);
    
    detailsLayout->addWidget(new QLabel("Name:"), 0, 0);
    m_profileNameEdit = new QLineEdit();
    detailsLayout->addWidget(m_profileNameEdit, 0, 1);
    
    m_profileEnabledCheckBox = new QCheckBox("Enabled");
    detailsLayout->addWidget(m_profileEnabledCheckBox, 1, 0, 1, 2);
    
    layout->addWidget(detailsGroup);
    
    // Control buttons
    QGroupBox *controlGroup = new QGroupBox("Simulation Control");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    
    m_startButton = new QPushButton("Start (F5)");
    m_stopButton = new QPushButton("Stop (F6)");
    m_startButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    m_stopButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; }");
    
    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addStretch();
    
    layout->addWidget(controlGroup);
    
    layout->addStretch();
    m_tabWidget->addTab(profileWidget, "Profiles");
}

void MainWindow::createActionsTab() {
    QWidget *actionsWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(actionsWidget);
    
    // Actions table
    m_actionsTable = new QTableWidget();
    m_actionsTable->setColumnCount(4);
    QStringList headers = {"Type", "Key/Button", "Interval (ms)", "Enabled"};
    m_actionsTable->setHorizontalHeaderLabels(headers);
    m_actionsTable->horizontalHeader()->setStretchLastSection(true);
    m_actionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Make interval column editable
    connect(m_actionsTable, &QTableWidget::itemChanged, this, &MainWindow::onActionTableItemChanged);
    
    layout->addWidget(m_actionsTable);
    
    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_addActionButton = new QPushButton("Add Action");
    m_removeActionButton = new QPushButton("Remove Action");
    m_editActionButton = new QPushButton("Edit Action");
    
    buttonLayout->addWidget(m_addActionButton);
    buttonLayout->addWidget(m_removeActionButton);
    buttonLayout->addWidget(m_editActionButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    m_tabWidget->addTab(actionsWidget, "Actions");
}

void MainWindow::createSettingsTab() {
    QWidget *settingsWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(settingsWidget);
    
    // Hotkey settings
    QGroupBox *hotkeyGroup = new QGroupBox("Hotkey Settings");
    QGridLayout *hotkeyLayout = new QGridLayout(hotkeyGroup);
    
    hotkeyLayout->addWidget(new QLabel("Start Hotkey:"), 0, 0);
    m_startHotkeyEdit = new QLineEdit();
    m_startHotkeyEdit->setReadOnly(true);
    hotkeyLayout->addWidget(m_startHotkeyEdit, 0, 1);
    
    hotkeyLayout->addWidget(new QLabel("Stop Hotkey:"), 1, 0);
    m_stopHotkeyEdit = new QLineEdit();
    m_stopHotkeyEdit->setReadOnly(true);
    hotkeyLayout->addWidget(m_stopHotkeyEdit, 1, 1);
    
    layout->addWidget(hotkeyGroup);
    
    // Application settings
    QGroupBox *appGroup = new QGroupBox("Application Settings");
    QVBoxLayout *appLayout = new QVBoxLayout(appGroup);
    
    m_minimizeToTrayCheckBox = new QCheckBox("Minimize to system tray");
    m_startMinimizedCheckBox = new QCheckBox("Start minimized");
    
    appLayout->addWidget(m_minimizeToTrayCheckBox);
    appLayout->addWidget(m_startMinimizedCheckBox);
    
    layout->addWidget(appGroup);
    
    // Configuration buttons
    QGroupBox *configGroup = new QGroupBox("Configuration");
    QHBoxLayout *configLayout = new QHBoxLayout(configGroup);
    
    QPushButton *saveButton = new QPushButton("Save Configuration");
    QPushButton *loadButton = new QPushButton("Load Configuration");
    QPushButton *resetButton = new QPushButton("Reset to Defaults");
    
    configLayout->addWidget(saveButton);
    configLayout->addWidget(loadButton);
    configLayout->addWidget(resetButton);
    configLayout->addStretch();
    
    layout->addWidget(configGroup);
    
    layout->addStretch();
    m_tabWidget->addTab(settingsWidget, "Settings");
}

void MainWindow::createAboutTab() {
    QWidget *aboutWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(aboutWidget);
    
    QTextEdit *aboutText = new QTextEdit();
    aboutText->setReadOnly(true);
    aboutText->setHtml(
        "<h2>AutoKey for Diablo 3</h2>"
        "<p><b>Version:</b> 1.0.0</p>"
        "<p><b>Description:</b> An automated key-pressing application designed specifically for Diablo 3 gameplay.</p>"
        "<h3>Features:</h3>"
        "<ul>"
        "<li>Multiple character class profiles with customizable key sequences</li>"
        "<li>Configurable intervals for each key/mouse action</li>"
        "<li>Global hotkeys (F5 to start, F6 to stop)</li>"
        "<li>System tray integration</li>"
        "<li>JSON-based configuration storage</li>"
        "<li>5 pre-configured class profiles</li>"
        "</ul>"
        "<h3>Usage:</h3>"
        "<ol>"
        "<li>Select or create a character class profile</li>"
        "<li>Configure the keys and intervals for your abilities</li>"
        "<li>Press F5 to start auto-key simulation</li>"
        "<li>Press F6 to stop simulation</li>"
        "</ol>"
        "<p><b>Note:</b> This application is designed for legitimate gameplay enhancement. "
        "Please ensure compliance with game terms of service.</p>"
        "<p><b>System Requirements:</b> Windows 10/11, Qt5</p>"
    );
    
    layout->addWidget(aboutText);
    m_tabWidget->addTab(aboutWidget, "About");
}

void MainWindow::setupSystemTray() {
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(this, "System Tray",
            "System tray is not available on this system.");
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);

    // Try to load custom icon, fallback to built-in icon
    QIcon trayIcon(":/icons/keyboard.ico");
    if (trayIcon.isNull()) {
        // Use a built-in Qt icon as fallback
        trayIcon = style()->standardIcon(QStyle::SP_ComputerIcon);
    }
    m_trayIcon->setIcon(trayIcon);

    // Create tray menu
    m_trayMenu = new QMenu(this);

    m_showAction = new QAction("Show", this);
    m_hideAction = new QAction("Hide", this);
    m_startAction = new QAction("Start (F5)", this);
    m_stopAction = new QAction("Stop (F6)", this);
    m_quitAction = new QAction("Quit", this);

    m_trayMenu->addAction(m_showAction);
    m_trayMenu->addAction(m_hideAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_startAction);
    m_trayMenu->addAction(m_stopAction);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(m_quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();
}

void MainWindow::setupMenuBar() {
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *loadAction = new QAction("&Load Configuration", this);
    QAction *saveAction = new QAction("&Save Configuration", this);
    QAction *exitAction = new QAction("E&xit", this);

    fileMenu->addAction(loadAction);
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu *editMenu = menuBar()->addMenu("&Edit");
    QAction *addProfileAction = new QAction("&Add Profile", this);
    QAction *removeProfileAction = new QAction("&Remove Profile", this);

    editMenu->addAction(addProfileAction);
    editMenu->addAction(removeProfileAction);

    QMenu *controlMenu = menuBar()->addMenu("&Control");
    QAction *startAction = new QAction("&Start (F5)", this);
    QAction *stopAction = new QAction("S&top (F6)", this);

    controlMenu->addAction(startAction);
    controlMenu->addAction(stopAction);

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = new QAction("&About", this);
    helpMenu->addAction(aboutAction);

    // Connect menu actions
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoadConfiguration);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveConfiguration);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(addProfileAction, &QAction::triggered, this, &MainWindow::onAddProfile);
    connect(removeProfileAction, &QAction::triggered, this, &MainWindow::onRemoveProfile);
    connect(startAction, &QAction::triggered, this, &MainWindow::onStartSimulation);
    connect(stopAction, &QAction::triggered, this, &MainWindow::onStopSimulation);
    connect(aboutAction, &QAction::triggered, [this]() {
        m_tabWidget->setCurrentIndex(3); // Switch to About tab
    });
}

void MainWindow::setupStatusBar() {
    m_statusLabel = new QLabel("Ready");
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);

    statusBar()->addWidget(m_statusLabel);
    statusBar()->addPermanentWidget(m_progressBar);
}

void MainWindow::setupConnections() {
    // Profile management
    connect(m_profileComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onProfileChanged);
    connect(m_addProfileButton, &QPushButton::clicked, this, &MainWindow::onAddProfile);
    connect(m_removeProfileButton, &QPushButton::clicked, this, &MainWindow::onRemoveProfile);
    connect(m_editProfileButton, &QPushButton::clicked, this, &MainWindow::onEditProfile);
    connect(m_duplicateProfileButton, &QPushButton::clicked, this, &MainWindow::onDuplicateProfile);

    // Action management
    connect(m_addActionButton, &QPushButton::clicked, this, &MainWindow::onAddAction);
    connect(m_removeActionButton, &QPushButton::clicked, this, &MainWindow::onRemoveAction);
    connect(m_editActionButton, &QPushButton::clicked, this, &MainWindow::onEditAction);
    connect(m_actionsTable, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onActionSelectionChanged);

    // Simulation control
    connect(m_startButton, &QPushButton::clicked, this, &MainWindow::onStartSimulation);
    connect(m_stopButton, &QPushButton::clicked, this, &MainWindow::onStopSimulation);

    // Key simulator signals
    connect(m_keySimulator, &KeySimulator::simulationStarted,
            this, &MainWindow::onSimulationStarted);
    connect(m_keySimulator, &KeySimulator::simulationStopped,
            this, &MainWindow::onSimulationStopped);
    connect(m_keySimulator, &KeySimulator::hotkeyPressed,
            this, &MainWindow::onHotkeyPressed);

    // System tray
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayIconActivated);
    connect(m_showAction, &QAction::triggered, this, &MainWindow::showWindow);
    connect(m_hideAction, &QAction::triggered, this, &MainWindow::hideWindow);
    connect(m_startAction, &QAction::triggered, this, &MainWindow::onStartSimulation);
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::onStopSimulation);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
}

// Profile management slots
void MainWindow::onProfileChanged(int index) {
    if (index >= 0 && index < m_configManager->getProfiles().size()) {
        m_configManager->setCurrentProfileIndex(index);
        updateUI();
    }
}

void MainWindow::onAddProfile() {
    ClassProfile newProfile("New Profile");
    m_configManager->addProfile(newProfile);
    m_configManager->setCurrentProfileIndex(m_configManager->getProfiles().size() - 1);
    updateProfileList();
}

void MainWindow::onRemoveProfile() {
    int currentIndex = m_profileComboBox->currentIndex();
    if (currentIndex >= 0 && m_configManager->getProfiles().size() > 1) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
            "Remove Profile", "Are you sure you want to remove this profile?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            m_configManager->removeProfile(currentIndex);
            updateProfileList();
        }
    } else {
        QMessageBox::information(this, "Cannot Remove",
            "Cannot remove the last remaining profile.");
    }
}

void MainWindow::onEditProfile() {
    // For now, just focus on the name edit field
    m_profileNameEdit->setFocus();
    m_profileNameEdit->selectAll();
}

void MainWindow::onDuplicateProfile() {
    int currentIndex = m_profileComboBox->currentIndex();
    if (currentIndex >= 0) {
        ClassProfile currentProfile = m_configManager->getProfiles()[currentIndex];
        currentProfile.setName(currentProfile.getName() + " (Copy)");
        m_configManager->addProfile(currentProfile);
        m_configManager->setCurrentProfileIndex(m_configManager->getProfiles().size() - 1);
        updateProfileList();
    }
}

// Action management slots
void MainWindow::onAddAction() {
    int currentProfileIndex = m_profileComboBox->currentIndex();
    if (currentProfileIndex >= 0) {
        KeyAction newAction(InputType::Keyboard, 0x32, 200); // Default: Key '2', 200ms

        auto profiles = m_configManager->getProfiles();
        profiles[currentProfileIndex].addAction(newAction);
        m_configManager->setProfiles(profiles);

        updateActionList();
        autoSaveConfiguration();
    }
}

void MainWindow::onRemoveAction() {
    int currentRow = m_actionsTable->currentRow();
    int currentProfileIndex = m_profileComboBox->currentIndex();

    if (currentRow >= 0 && currentProfileIndex >= 0) {
        auto profiles = m_configManager->getProfiles();
        profiles[currentProfileIndex].removeAction(currentRow);
        m_configManager->setProfiles(profiles);

        updateActionList();
        autoSaveConfiguration();
    }
}

void MainWindow::onEditAction() {
    int currentRow = m_actionsTable->currentRow();
    int currentProfileIndex = m_profileComboBox->currentIndex();

    if (currentRow >= 0 && currentProfileIndex >= 0) {
        auto profiles = m_configManager->getProfiles();
        if (currentProfileIndex < profiles.size()) {
            const auto &actions = profiles[currentProfileIndex].getActions();
            if (currentRow < actions.size()) {
                KeyAction currentAction = actions[currentRow];

                ActionEditDialog dialog(currentAction, this);
                if (dialog.exec() == QDialog::Accepted) {
                    KeyAction editedAction = dialog.getAction();
                    profiles[currentProfileIndex].updateAction(currentRow, editedAction);
                    m_configManager->setProfiles(profiles);

                    updateActionList();
                    autoSaveConfiguration();
                }
            }
        }
    }
}

void MainWindow::onActionSelectionChanged() {
    if (!m_actionsTable || !m_removeActionButton || !m_editActionButton) {
        return;
    }

    bool hasSelection = m_actionsTable->currentRow() >= 0;
    m_removeActionButton->setEnabled(hasSelection);
    m_editActionButton->setEnabled(hasSelection);
}

void MainWindow::onActionTableItemChanged(QTableWidgetItem *item) {
    if (!item) return;

    int row = item->row();
    int column = item->column();
    int currentProfileIndex = m_profileComboBox->currentIndex();

    if (currentProfileIndex < 0) return;

    auto profiles = m_configManager->getProfiles();
    if (currentProfileIndex >= profiles.size()) return;

    auto actions = profiles[currentProfileIndex].getActions();
    if (row >= actions.size()) return;

    KeyAction action = actions[row];
    bool changed = false;

    // Handle different columns
    if (column == 2) { // Interval column
        bool ok;
        int newInterval = item->text().toInt(&ok);
        if (ok && newInterval >= 100 && newInterval <= 60000) {
            action.interval = newInterval;
            changed = true;
        } else {
            // Revert to original value if invalid
            item->setText(QString::number(action.interval));
            QMessageBox::warning(this, "Invalid Interval",
                "Interval must be between 100 and 60000 milliseconds.");
        }
    } else if (column == 3) { // Enabled column
        action.enabled = (item->checkState() == Qt::Checked);
        changed = true;
    }

    if (changed) {
        profiles[currentProfileIndex].updateAction(row, action);
        m_configManager->setProfiles(profiles);
        autoSaveConfiguration();
    }
}

// Simulation control slots
void MainWindow::onStartSimulation() {
    int currentProfileIndex = m_profileComboBox->currentIndex();
    if (currentProfileIndex >= 0) {
        const auto &profiles = m_configManager->getProfiles();
        if (currentProfileIndex < profiles.size()) {
            const ClassProfile &profile = profiles[currentProfileIndex];
            if (profile.isEnabled() && !profile.getActions().isEmpty()) {
                m_keySimulator->startSimulation(profile);
            } else {
                QMessageBox::warning(this, "Cannot Start",
                    "Profile is disabled or has no actions configured.");
            }
        }
    }
}

void MainWindow::onStopSimulation() {
    m_keySimulator->stopSimulation();
}

void MainWindow::onSimulationStarted() {
    m_isSimulationRunning = true;
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_statusLabel->setText("Simulation running...");

    if (m_trayIcon) {
        m_trayIcon->showMessage("AutoKey", "Simulation started",
                               QSystemTrayIcon::Information, 2000);
    }
}

void MainWindow::onSimulationStopped() {
    m_isSimulationRunning = false;
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_statusLabel->setText("Simulation stopped");

    if (m_trayIcon) {
        m_trayIcon->showMessage("AutoKey", "Simulation stopped",
                               QSystemTrayIcon::Information, 2000);
    }
}

// System tray slots
void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if (isVisible()) {
            hideWindow();
        } else {
            showWindow();
        }
        break;
    default:
        break;
    }
}

void MainWindow::showWindow() {
    show();
    raise();
    activateWindow();
}

void MainWindow::hideWindow() {
    hide();
}

// Configuration slots
void MainWindow::onSaveConfiguration() {
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Configuration", "config.json", "JSON Files (*.json)");

    if (!fileName.isEmpty()) {
        if (m_configManager->saveConfiguration(fileName)) {
            QMessageBox::information(this, "Success", "Configuration saved successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to save configuration.");
        }
    }
}

void MainWindow::onLoadConfiguration() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "Load Configuration", "config.json", "JSON Files (*.json)");

    if (!fileName.isEmpty()) {
        if (m_configManager->loadConfiguration(fileName)) {
            updateUI();
            QMessageBox::information(this, "Success", "Configuration loaded successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to load configuration.");
        }
    }
}

void MainWindow::onResetToDefaults() {
    QMessageBox::StandardButton reply = QMessageBox::question(this,
        "Reset to Defaults", "Are you sure you want to reset all settings to defaults?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_configManager->resetToDefaults();
        updateUI();
        QMessageBox::information(this, "Reset Complete", "Settings have been reset to defaults.");
    }
}

// Hotkey handling
void MainWindow::onHotkeyPressed(int hotkeyId) {
    qDebug() << "Processing hotkey ID:" << hotkeyId;
    switch (hotkeyId) {
    case 1: // Start hotkey
        qDebug() << "F5 pressed - starting simulation";
        onStartSimulation();
        break;
    case 2: // Stop hotkey
        qDebug() << "F6 pressed - stopping simulation";
        onStopSimulation();
        break;
    default:
        qDebug() << "Unknown hotkey ID:" << hotkeyId;
        break;
    }
}

// UI update methods
void MainWindow::updateProfileList() {
    if (!m_profileComboBox) {
        return;
    }

    // Temporarily disconnect the signal to avoid infinite recursion
    disconnect(m_profileComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
               this, &MainWindow::onProfileChanged);

    m_profileComboBox->clear();
    const auto &profiles = m_configManager->getProfiles();
    for (const auto &profile : profiles) {
        m_profileComboBox->addItem(profile.getName());
    }

    int currentIndex = m_configManager->getCurrentProfileIndex();
    if (currentIndex >= 0 && currentIndex < profiles.size()) {
        m_profileComboBox->setCurrentIndex(currentIndex);
    }

    // Reconnect the signal
    connect(m_profileComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onProfileChanged);
}

void MainWindow::updateActionList() {
    if (!m_actionsTable || !m_profileComboBox) {
        return;
    }

    m_actionsTable->setRowCount(0);

    int currentProfileIndex = m_profileComboBox->currentIndex();
    if (currentProfileIndex >= 0) {
        const auto &profiles = m_configManager->getProfiles();
        if (currentProfileIndex < profiles.size()) {
            const auto &actions = profiles[currentProfileIndex].getActions();

            m_actionsTable->setRowCount(actions.size());
            for (int i = 0; i < actions.size(); ++i) {
                const auto &action = actions[i];

                // Type column
                QString typeStr;
                switch (action.type) {
                case InputType::Keyboard:
                    typeStr = "Keyboard";
                    break;
                case InputType::MouseLeft:
                    typeStr = "Mouse Left";
                    break;
                case InputType::MouseRight:
                    typeStr = "Mouse Right";
                    break;
                }
                m_actionsTable->setItem(i, 0, new QTableWidgetItem(typeStr));

                // Key/Button column
                QString keyStr;
                if (action.type == InputType::Keyboard) {
                    keyStr = QString("Key %1").arg(QChar(action.key));
                } else {
                    keyStr = "Click";
                }
                m_actionsTable->setItem(i, 1, new QTableWidgetItem(keyStr));

                // Interval column - make it editable
                QTableWidgetItem *intervalItem = new QTableWidgetItem(QString::number(action.interval));
                intervalItem->setFlags(intervalItem->flags() | Qt::ItemIsEditable);
                m_actionsTable->setItem(i, 2, intervalItem);

                // Enabled column
                QTableWidgetItem *enabledItem = new QTableWidgetItem();
                enabledItem->setCheckState(action.enabled ? Qt::Checked : Qt::Unchecked);
                enabledItem->setFlags(enabledItem->flags() | Qt::ItemIsUserCheckable);
                m_actionsTable->setItem(i, 3, enabledItem);
            }
        }
    }
}

void MainWindow::updateUI() {
    qDebug() << "updateUI() called";
    qDebug() << "m_profileComboBox:" << (void*)m_profileComboBox;
    qDebug() << "m_actionsTable:" << (void*)m_actionsTable;

    if (!m_profileComboBox || !m_actionsTable) {
        // UI not fully initialized yet
        qDebug() << "UI not fully initialized, returning early";
        return;
    }

    updateProfileList();
    updateActionList();

    // Update profile details
    int currentIndex = m_profileComboBox->currentIndex();
    if (currentIndex >= 0) {
        const auto &profiles = m_configManager->getProfiles();
        if (currentIndex < profiles.size()) {
            const auto &profile = profiles[currentIndex];
            if (m_profileNameEdit) {
                m_profileNameEdit->setText(profile.getName());
            }
            if (m_profileEnabledCheckBox) {
                m_profileEnabledCheckBox->setChecked(profile.isEnabled());
            }
        }
    }

    // Update settings
    if (m_startHotkeyEdit) {
        m_startHotkeyEdit->setText(m_configManager->getStartHotkey());
    }
    if (m_stopHotkeyEdit) {
        m_stopHotkeyEdit->setText(m_configManager->getStopHotkey());
    }
    if (m_minimizeToTrayCheckBox) {
        m_minimizeToTrayCheckBox->setChecked(m_configManager->getMinimizeToTray());
    }
    if (m_startMinimizedCheckBox) {
        m_startMinimizedCheckBox->setChecked(m_configManager->getStartMinimized());
    }

    // Update button states
    if (m_startButton) {
        m_startButton->setEnabled(!m_isSimulationRunning);
    }
    if (m_stopButton) {
        m_stopButton->setEnabled(m_isSimulationRunning);
    }

    onActionSelectionChanged(); // Update action button states
}

void MainWindow::updateStatusBar() {
    if (m_isSimulationRunning) {
        int currentIndex = m_profileComboBox->currentIndex();
        if (currentIndex >= 0) {
            const auto &profiles = m_configManager->getProfiles();
            if (currentIndex < profiles.size()) {
                const auto &profile = profiles[currentIndex];
                m_statusLabel->setText(QString("Running: %1").arg(profile.getName()));
            }
        }
    } else {
        m_statusLabel->setText("Ready");
    }
}

void MainWindow::autoSaveConfiguration() {
    // Auto-save to the default configuration file
    m_configManager->saveConfiguration(); // Uses default path
    qDebug() << "Configuration auto-saved";
}

// Event handlers
void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    // Register global hotkeys after the window is shown
    static bool hotkeysRegistered = false;
    if (!hotkeysRegistered) {
        // Use this window's handle directly for hotkey registration
        HWND hwnd = reinterpret_cast<HWND>(this->winId());

        // Register F5 as start hotkey (VK_F5 = 0x74)
        bool f5Success = RegisterHotKey(hwnd, 1, 0, 0x74);

        // Register F6 as stop hotkey (VK_F6 = 0x75)
        bool f6Success = RegisterHotKey(hwnd, 2, 0, 0x75);

        if (f5Success && f6Success) {
            hotkeysRegistered = true;
            qDebug() << "Global hotkeys registered successfully";
        } else {
            QMessageBox::warning(this, "Warning",
                "Failed to register global hotkeys F5/F6. "
                "You may need to run the application as administrator.");
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (m_trayIcon && m_trayIcon->isVisible() && m_configManager->getMinimizeToTray()) {
        QMessageBox::information(this, "AutoKey",
            "The application will keep running in the system tray. "
            "To terminate the application, choose Quit from the context menu.");
        hide();
        event->ignore();
    } else {
        m_keySimulator->stopSimulation();
        m_configManager->saveConfiguration();
        event->accept();
    }
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result) {
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            int hotkeyId = msg->wParam;
            qDebug() << "Hotkey received! ID:" << hotkeyId;
            onHotkeyPressed(hotkeyId);
            return true;
        }
    }

    return QMainWindow::nativeEvent(eventType, message, result);
}
