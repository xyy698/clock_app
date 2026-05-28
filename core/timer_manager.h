#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QTime>

// TimerManager handles both countdown-timer and stopwatch logic.
// Uses QElapsedTimer internally for drift-free timing.
class TimerManager : public QObject
{
    Q_OBJECT

public:
    enum State { Stopped, Running, Paused };

    explicit TimerManager(QObject *parent = nullptr);

    // ---- Countdown timer ----
    void setDuration(int hours, int minutes, int seconds);
    void startTimer();
    void pauseTimer();
    void stopTimer();
    QTime remainingTime() const;   // in H:M:S format
    State timerState() const { return m_timerState; }

    // ---- Stopwatch ----
    void startStopwatch();
    void pauseStopwatch();
    void stopStopwatch();
    qint64 elapsedMillis() const;
    qint64 recordLap();            // returns lap time, records lap
    State stopwatchState() const { return m_swState; }
    QVector<qint64> laps() const { return m_laps; }

signals:
    void timerTick(const QTime &remaining);
    void timerFinished();
    void stopwatchTick(qint64 millis);

private:
    QTimer *m_tickTimer;           // fires every 80ms
    QElapsedTimer m_ref;           // reference timer for both modes

    // Countdown
    State m_timerState = Stopped;
    int m_totalMs = 0;
    int m_remainingMs = 0;
    qint64 m_timerAccum = 0;       // accumulated elapsed before pause

    // Stopwatch
    State m_swState = Stopped;
    qint64 m_swAccum = 0;          // accumulated before pause
    qint64 m_lastLapMark = 0;
    QVector<qint64> m_laps;
};

#endif // TIMER_MANAGER_H
