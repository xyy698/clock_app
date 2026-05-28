#include "timer_manager.h"

TimerManager::TimerManager(QObject *parent)
    : QObject(parent)
    , m_tickTimer(new QTimer(this))
{
    m_tickTimer->setInterval(80);
    m_tickTimer->setTimerType(Qt::PreciseTimer);
    connect(m_tickTimer, &QTimer::timeout, this, [this]() {
        if (m_timerState == Running) {
            int elapsed = static_cast<int>(m_timerAccum + m_ref.elapsed());
            int remain = qMax(0, m_totalMs - elapsed);
            // Round down to whole seconds for display
            emit timerTick(QTime(0, 0, 0).addMSecs(remain));
            if (remain <= 0) {
                m_timerState = Stopped;
                m_tickTimer->stop();
                emit timerFinished();
            }
        }
        if (m_swState == Running) {
            emit stopwatchTick(elapsedMillis());
        }
    });
}

// ---- Countdown ----

void TimerManager::setDuration(int hours, int minutes, int seconds)
{
    m_totalMs = (hours * 3600 + minutes * 60 + seconds) * 1000;
    m_remainingMs = m_totalMs;
    emit timerTick(QTime(0, 0, 0).addMSecs(m_totalMs));
}

void TimerManager::startTimer()
{
    if (m_timerState != Paused) {
        m_timerAccum = 0;
    }
    m_timerState = Running;
    m_ref.start();
    m_tickTimer->start();
}

void TimerManager::pauseTimer()
{
    if (m_timerState == Running) {
        m_timerAccum += m_ref.elapsed();
    }
    m_timerState = Paused;
    m_tickTimer->stop();
}

void TimerManager::stopTimer()
{
    m_timerState = Stopped;
    m_tickTimer->stop();
    m_timerAccum = 0;
    emit timerTick(QTime(0, 0, 0).addMSecs(m_totalMs));
}

QTime TimerManager::remainingTime() const
{
    int remain = m_totalMs;
    if (m_timerState == Running) {
        remain = qMax(0, m_totalMs - static_cast<int>(m_timerAccum + m_ref.elapsed()));
    } else if (m_timerState == Paused) {
        remain = qMax(0, m_totalMs - static_cast<int>(m_timerAccum));
    }
    return QTime(0, 0, 0).addMSecs(remain);
}

// ---- Stopwatch ----

void TimerManager::startStopwatch()
{
    if (m_swState != Paused) {
        m_swAccum = 0;
        m_laps.clear();
        m_lastLapMark = 0;
    }
    m_swState = Running;
    m_ref.start();
    m_tickTimer->start();
}

void TimerManager::pauseStopwatch()
{
    if (m_swState == Running) {
        m_swAccum += m_ref.elapsed();
    }
    m_swState = Paused;
    m_tickTimer->stop();
}

void TimerManager::stopStopwatch()
{
    m_swState = Stopped;
    m_tickTimer->stop();
    m_swAccum = 0;
    m_lastLapMark = 0;
    emit stopwatchTick(0);
}

qint64 TimerManager::elapsedMillis() const
{
    if (m_swState == Running) {
        return m_swAccum + m_ref.elapsed();
    }
    return m_swAccum;
}

qint64 TimerManager::recordLap()
{
    qint64 total = elapsedMillis();
    qint64 lap = total - m_lastLapMark;
    m_lastLapMark = total;
    m_laps.append(lap);
    return lap;
}
