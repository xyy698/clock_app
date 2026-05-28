#include "clock_engine.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

ClockEngine& ClockEngine::instance()
{
    static ClockEngine inst;
    return inst;
}

ClockEngine::ClockEngine(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_timeZone(QTimeZone::systemTimeZone())
{
    m_timer->setTimerType(Qt::PreciseTimer);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        emit tick(currentTime());
    });
}

void ClockEngine::start()
{
    m_timer->start(1000);
}

void ClockEngine::stop()
{
    m_timer->stop();
}

bool ClockEngine::isRunning() const
{
    return m_timer->isActive();
}

QDateTime ClockEngine::currentTime() const
{
    return QDateTime::currentDateTime().toTimeZone(m_timeZone);
}

QTimeZone ClockEngine::timeZone() const
{
    return m_timeZone;
}

void ClockEngine::setTimeZone(const QTimeZone &tz)
{
    m_timeZone = tz;
    emit tick(currentTime());
}

bool ClockEngine::setSystemTime(const QDateTime &dt)
{
#ifdef Q_OS_WIN
    QDate date = dt.date();
    QTime time = dt.time();
    SYSTEMTIME st;
    st.wYear   = date.year();
    st.wMonth  = date.month();
    st.wDay    = date.day();
    st.wHour   = time.hour();
    st.wMinute = time.minute();
    st.wSecond = time.second();
    st.wMilliseconds = time.msec();
    st.wDayOfWeek = date.dayOfWeek() % 7;

    return SetSystemTime(&st);
#else
    Q_UNUSED(dt)
    return false;
#endif
}
