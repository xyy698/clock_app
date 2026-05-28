#ifndef STOPWATCH_PANEL_H
#define STOPWATCH_PANEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include "core/timer_manager.h"

// StopwatchPanel — stopwatch UI with lap timing.
class StopwatchPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StopwatchPanel(QWidget *parent = nullptr);

private slots:
    void onStartStop();
    void onLap();
    void onReset();
    void updateDisplay(qint64 millis);
    void updateButtons();

private:
    QString formatMillis(qint64 ms) const;

    TimerManager *m_manager;
    QLabel *m_display;
    QListWidget *m_lapList;
    QPushButton *m_startBtn, *m_lapBtn, *m_resetBtn;
};

#endif // STOPWATCH_PANEL_H
