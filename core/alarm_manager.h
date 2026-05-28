#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <QObject>
#include <QVector>
#include <QTime>
#include <QSet>
#include <QJsonObject>
#include <QJsonArray>

// Represents a single alarm definition.
struct Alarm {
    int id = 0;
    QTime time;               // trigger time
    QSet<int> repeatDays;     // empty = one-shot; 0=Sun..6=Sat
    QString soundFile;        // path to sound, empty = system beep
    int durationSec = 60;     // how long to ring
    int snoozeMin = 5;        // snooze interval
    bool enabled = true;

    QString repeatDescription() const;
    QJsonObject toJson() const;
    static Alarm fromJson(const QJsonObject &obj);
};

// AlarmManager — singleton managing alarm CRUD and trigger detection.
class AlarmManager : public QObject
{
    Q_OBJECT

public:
    static AlarmManager& instance();

    QVector<Alarm> alarms() const { return m_alarms; }
    void addAlarm(const Alarm &alarm);
    void removeAlarm(int id);
    void updateAlarm(const Alarm &alarm);
    void setEnabled(int id, bool enabled);

    void load();
    void save();

    // Called every second; emits alarmTriggered if any alarm fires.
    void checkAlarms(const QTime &currentTime, int dayOfWeek);

signals:
    void alarmTriggered(const Alarm &alarm);
    void alarmsChanged();

private:
    explicit AlarmManager(QObject *parent = nullptr);
    ~AlarmManager() override = default;
    AlarmManager(const AlarmManager&) = delete;
    AlarmManager& operator=(const AlarmManager&) = delete;

    int nextId();
    QVector<Alarm> m_alarms;
    int m_idCounter = 1;
    QSet<int> m_triggeredToday;  // prevent re-triggering same alarm in same minute
};

#endif // ALARM_MANAGER_H
