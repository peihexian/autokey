#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QProgressBar>
#include <QTimer>
#include "configmanager.h"
#include "keysimulator.h"
#include "actioneditdialog.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QSystemTrayIcon;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

private slots:
    // Profile management
    void onProfileChanged(int index);
    void onAddProfile();
    void onRemoveProfile();
    void onEditProfile();
    void onDuplicateProfile();
    
    // Action management
    void onAddAction();
    void onRemoveAction();
    void onEditAction();
    void onActionSelectionChanged();
    void onActionTableItemChanged(QTableWidgetItem *item);
    
    // Simulation control
    void onStartSimulation();
    void onStopSimulation();
    void onSimulationStarted();
    void onSimulationStopped();
    
    // System tray
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void showWindow();
    void hideWindow();
    
    // Configuration
    void onSaveConfiguration();
    void onLoadConfiguration();
    void onResetToDefaults();
    
    // Hotkey handling
    void onHotkeyPressed(int hotkeyId);
    
    // UI updates
    void updateProfileList();
    void updateActionList();
    void updateUI();
    void updateStatusBar();
    void autoSaveConfiguration();

private:
    void setupUI();
    void setupSystemTray();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();
    
    void createProfileTab();
    void createActionsTab();
    void createSettingsTab();
    void createAboutTab();
    
    // UI Components
    QTabWidget *m_tabWidget;
    
    // Profile tab
    QComboBox *m_profileComboBox;
    QPushButton *m_addProfileButton;
    QPushButton *m_removeProfileButton;
    QPushButton *m_editProfileButton;
    QPushButton *m_duplicateProfileButton;
    QLineEdit *m_profileNameEdit;
    QCheckBox *m_profileEnabledCheckBox;
    
    // Actions tab
    QTableWidget *m_actionsTable;
    QPushButton *m_addActionButton;
    QPushButton *m_removeActionButton;
    QPushButton *m_editActionButton;
    
    // Control buttons
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
    
    // Settings tab
    QLineEdit *m_startHotkeyEdit;
    QLineEdit *m_stopHotkeyEdit;
    QCheckBox *m_minimizeToTrayCheckBox;
    QCheckBox *m_startMinimizedCheckBox;
    
    // Status and info
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QTextEdit *m_logTextEdit;
    
    // System tray
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QAction *m_showAction;
    QAction *m_hideAction;
    QAction *m_quitAction;
    QAction *m_startAction;
    QAction *m_stopAction;
    
    // Core components
    ConfigManager *m_configManager;
    KeySimulator *m_keySimulator;
    
    // State
    bool m_isSimulationRunning;
    QTimer *m_statusUpdateTimer;
};

#endif // MAINWINDOW_H
