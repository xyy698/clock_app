#ifndef PREFERENCE_MANAGER_H
#define PREFERENCE_MANAGER_H

#include <QObject>
#include <QSettings>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QTimeZone>

// PreferenceManager — singleton wrapper around QSettings.
// Persists user preferences across sessions.
class PreferenceManager : public QObject
{
    Q_OBJECT

public:
    static PreferenceManager& instance();

    void load();
    void save();

    // Display mode: "analog" or "digital"
    QString displayMode() const;
    void setDisplayMode(const QString &mode);

    bool alwaysOnTop() const;
    void setAlwaysOnTop(bool on);

    bool irregularWindow() const;
    void setIrregularWindow(bool irr);

    bool autoStart() const;
    void setAutoStart(bool start);

    QString timeZoneId() const;
    void setTimeZoneId(const QString &id);

    // Window geometry
    QPoint windowPos() const;
    void setWindowPos(const QPoint &pos);
    QSize windowSize() const;
    void setWindowSize(const QSize &size);

    // Eye protection
    int workInterval() const;     // minutes
    void setWorkInterval(int min);
    int restDuration() const;     // minutes
    void setRestDuration(int min);

    // Snooze
    int snoozeMinutes() const;
    void setSnoozeMinutes(int min);

private:
    explicit PreferenceManager(QObject *parent = nullptr);
    ~PreferenceManager() override = default;
    PreferenceManager(const PreferenceManager&) = delete;
    PreferenceManager& operator=(const PreferenceManager&) = delete;

    QSettings m_settings;
};

#endif // PREFERENCE_MANAGER_H
