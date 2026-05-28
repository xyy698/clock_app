#include <QApplication>
#include <QStyle>
#include <QDebug>
#include "ui/main_window.h"
#include "core/clock_engine.h"
#include "core/preference_manager.h"
#include "core/alarm_manager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("ClockApp");
    app.setOrganizationName("ClockApp");
    app.setWindowIcon(app.style()->standardIcon(QStyle::SP_ComputerIcon));

    qDebug() << "Starting ClockApp...";

    // Initialize singleton managers
    PreferenceManager::instance().load();
    qDebug() << "Preferences loaded.";

    ClockEngine::instance().start();
    qDebug() << "Clock engine started.";

    AlarmManager::instance().load();
    qDebug() << "Alarm manager loaded.";

    MainWindow window;
    qDebug() << "MainWindow created, showing...";
    window.show();

    int result = app.exec();

    // Cleanup
    PreferenceManager::instance().setWindowPos(window.pos());
    PreferenceManager::instance().setWindowSize(window.size());
    PreferenceManager::instance().save();
    AlarmManager::instance().save();
    ClockEngine::instance().stop();

    qDebug() << "ClockApp exiting.";
    return result;
}
