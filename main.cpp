#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include "mainwindow.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#endif

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef _WIN32
#ifdef _DEBUG
    // Allocate a console for this GUI application in debug mode
    AllocConsole();

    // Redirect stdout, stdin, stderr to console
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    // Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
    // point to console as well
    std::ios::sync_with_stdio(true);

    // Set console title
    SetConsoleTitleA("AutoKey Debug Console");

    qDebug() << "Debug console initialized";
#endif
#endif
    
    // Set application properties
    app.setApplicationName("AutoKey for Diablo 3");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("AutoKey");
    app.setOrganizationDomain("autokey.local");
    
    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "System Tray",
            "System tray is not available on this system. "
            "The application will run without tray functionality.");
    }
    
    // Ensure config directory exists
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(configDir);
    
    // Create and show main window
    MainWindow window;
    
    // Check command line arguments for minimized start
    QStringList args = app.arguments();
    bool startMinimized = args.contains("--minimized") || args.contains("-m");
    
    if (!startMinimized) {
        window.show();
    }
    
    return app.exec();
}
