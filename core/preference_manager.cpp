#include "preference_manager.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

PreferenceManager& PreferenceManager::instance()
{
    static PreferenceManager inst;
    return inst;
}

PreferenceManager::PreferenceManager(QObject *parent)
    : QObject(parent)
    , m_settings("ClockApp", "ClockApp")
{
}

void PreferenceManager::load()
{
    // All values read on demand via getters; nothing to preload.
}

void PreferenceManager::save()
{
    m_settings.sync();
}

QString PreferenceManager::displayMode() const
{
    return m_settings.value("display/mode", "analog").toString();
}

void PreferenceManager::setDisplayMode(const QString &mode)
{
    m_settings.setValue("display/mode", mode);
}

bool PreferenceManager::alwaysOnTop() const
{
    return m_settings.value("window/alwaysOnTop", false).toBool();
}

void PreferenceManager::setAlwaysOnTop(bool on)
{
    m_settings.setValue("window/alwaysOnTop", on);
}

bool PreferenceManager::irregularWindow() const
{
    return m_settings.value("window/irregular", false).toBool();
}

void PreferenceManager::setIrregularWindow(bool irr)
{
    m_settings.setValue("window/irregular", irr);
}

bool PreferenceManager::autoStart() const
{
    return m_settings.value("system/autoStart", false).toBool();
}

void PreferenceManager::setAutoStart(bool start)
{
    m_settings.setValue("system/autoStart", start);
#ifdef Q_OS_WIN
    QSettings reg("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                  QSettings::NativeFormat);
    if (start) {
        QString appPath = QDir::toNativeSeparators(QApplication::applicationFilePath());
        reg.setValue("ClockApp", appPath);
    } else {
        reg.remove("ClockApp");
    }
#endif
}

QString PreferenceManager::timeZoneId() const
{
    return m_settings.value("time/timezone",
        QTimeZone::systemTimeZone().id()).toString();
}

void PreferenceManager::setTimeZoneId(const QString &id)
{
    m_settings.setValue("time/timezone", id);
}

QPoint PreferenceManager::windowPos() const
{
    return m_settings.value("window/pos", QPoint(200, 200)).toPoint();
}

void PreferenceManager::setWindowPos(const QPoint &pos)
{
    m_settings.setValue("window/pos", pos);
}

QSize PreferenceManager::windowSize() const
{
    return m_settings.value("window/size", QSize(420, 580)).toSize();
}

void PreferenceManager::setWindowSize(const QSize &size)
{
    m_settings.setValue("window/size", size);
}

int PreferenceManager::workInterval() const
{
    return m_settings.value("health/workInterval", 45).toInt();
}

void PreferenceManager::setWorkInterval(int min)
{
    m_settings.setValue("health/workInterval", min);
}

int PreferenceManager::restDuration() const
{
    return m_settings.value("health/restDuration", 5).toInt();
}

void PreferenceManager::setRestDuration(int min)
{
    m_settings.setValue("health/restDuration", min);
}

int PreferenceManager::snoozeMinutes() const
{
    return m_settings.value("alarm/snoozeMinutes", 5).toInt();
}

void PreferenceManager::setSnoozeMinutes(int min)
{
    m_settings.setValue("alarm/snoozeMinutes", min);
}
