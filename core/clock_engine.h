#ifndef CLOCK_ENGINE_H
#define CLOCK_ENGINE_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QTimeZone>

// ClockEngine — singleton core time engine.
// Emits tick() every second to drive all clock displays.
class ClockEngine : public QObject
{
    Q_OBJECT

public:
    static ClockEngine& instance();

    void start();
    void stop();
    bool isRunning() const;

    QDateTime currentTime() const;
    QTimeZone timeZone() const;
    void setTimeZone(const QTimeZone &tz);

    // Attempt to set system time (requires admin on Windows).
    bool setSystemTime(const QDateTime &dt);

signals:
    void tick(const QDateTime &dateTime);

private:
    explicit ClockEngine(QObject *parent = nullptr);
    ~ClockEngine() override = default;
    ClockEngine(const ClockEngine&) = delete;
    ClockEngine& operator=(const ClockEngine&) = delete;

    QTimer *m_timer;
    QTimeZone m_timeZone;
};

#endif // CLOCK_ENGINE_H
