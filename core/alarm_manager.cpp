#include "alarm_manager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>

QString Alarm::repeatDescription() const
{
    if (repeatDays.isEmpty()) return QString::fromUtf8("单次");
    if (repeatDays.size() == 7) return QString::fromUtf8("每天");
    QStringList names = {"日","一","二","三","四","五","六"};
    QStringList days;
    for (int d : repeatDays) {
        if (d >= 0 && d < 7) days << names[d];
    }
    return "周" + days.join(",");
}

QJsonObject Alarm::toJson() const
{
    QJsonObject obj;
    obj["id"] = id;
    obj["time"] = time.toString("HH:mm");
    QJsonArray daysArr;
    for (int d : repeatDays) daysArr.append(d);
    obj["repeatDays"] = daysArr;
    obj["soundFile"] = soundFile;
    obj["durationSec"] = durationSec;
    obj["snoozeMin"] = snoozeMin;
    obj["enabled"] = enabled;
    return obj;
}

Alarm Alarm::fromJson(const QJsonObject &obj)
{
    Alarm a;
    a.id = obj["id"].toInt();
    a.time = QTime::fromString(obj["time"].toString(), "HH:mm");
    for (auto v : obj["repeatDays"].toArray())
        a.repeatDays.insert(v.toInt());
    a.soundFile = obj["soundFile"].toString();
    a.durationSec = obj["durationSec"].toInt(60);
    a.snoozeMin = obj["snoozeMin"].toInt(5);
    a.enabled = obj["enabled"].toBool(true);
    return a;
}

// ---- AlarmManager ----

AlarmManager& AlarmManager::instance()
{
    static AlarmManager inst;
    return inst;
}

AlarmManager::AlarmManager(QObject *parent) : QObject(parent) {}

int AlarmManager::nextId()
{
    return m_idCounter++;
}

void AlarmManager::addAlarm(const Alarm &alarm)
{
    Alarm a = alarm;
    a.id = nextId();
    m_alarms.append(a);
    save();
    emit alarmsChanged();
}

void AlarmManager::removeAlarm(int id)
{
    m_alarms.erase(
        std::remove_if(m_alarms.begin(), m_alarms.end(),
                       [id](const Alarm &a) { return a.id == id; }),
        m_alarms.end());
    save();
    emit alarmsChanged();
}

void AlarmManager::updateAlarm(const Alarm &alarm)
{
    for (auto &a : m_alarms) {
        if (a.id == alarm.id) {
            a = alarm;
            save();
            emit alarmsChanged();
            return;
        }
    }
}

void AlarmManager::setEnabled(int id, bool enabled)
{
    for (auto &a : m_alarms) {
        if (a.id == id) {
            a.enabled = enabled;
            save();
            emit alarmsChanged();
            return;
        }
    }
}

void AlarmManager::checkAlarms(const QTime &currentTime, int dayOfWeek)
{
    for (auto &alarm : m_alarms) {
        if (!alarm.enabled) continue;

        // Check if already triggered this minute (prevent re-fire)
        int triggerKey = alarm.id * 10000 + currentTime.hour() * 60 + currentTime.minute();
        if (m_triggeredToday.contains(triggerKey)) continue;

        // Time match (hour:minute, ignore seconds)
        if (alarm.time.hour() != currentTime.hour()) continue;
        if (alarm.time.minute() != currentTime.minute()) continue;

        // Day-of-week check
        if (!alarm.repeatDays.isEmpty() && !alarm.repeatDays.contains(dayOfWeek))
            continue;

        m_triggeredToday.insert(triggerKey);
        emit alarmTriggered(alarm);
    }

    // Clear trigger cache at midnight
    if (currentTime.hour() == 0 && currentTime.minute() == 0) {
        m_triggeredToday.clear();
    }
}

void AlarmManager::load()
{
    QSettings settings("ClockApp", "ClockApp");
    QJsonDocument doc = QJsonDocument::fromJson(
        settings.value("alarms/list", "[]").toByteArray());
    m_alarms.clear();
    m_idCounter = 1;
    for (auto v : doc.array()) {
        Alarm a = Alarm::fromJson(v.toObject());
        m_alarms.append(a);
        if (a.id >= m_idCounter) m_idCounter = a.id + 1;
    }
}

void AlarmManager::save()
{
    QSettings settings("ClockApp", "ClockApp");
    QJsonArray arr;
    for (const auto &a : m_alarms)
        arr.append(a.toJson());
    settings.setValue("alarms/list", QJsonDocument(arr).toJson(QJsonDocument::Compact));
}
