#ifndef TIMER_PANEL_H
#define TIMER_PANEL_H

#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include "core/timer_manager.h"

// TimerPanel — countdown timer UI.
class TimerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TimerPanel(QWidget *parent = nullptr);

private slots:
    void onStartStop();
    void onReset();
    void updateDisplay(const QTime &remaining);
    void onFinished();
    void updateButtons();

private:
    TimerManager *m_manager;
    QLabel *m_display;
    QSpinBox *m_hourSpin, *m_minSpin, *m_secSpin;
    QPushButton *m_startBtn, *m_resetBtn;
};

#endif // TIMER_PANEL_H
