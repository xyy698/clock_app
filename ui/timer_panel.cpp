#include "timer_panel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>

TimerPanel::TimerPanel(QWidget *parent)
    : QWidget(parent)
{
    m_manager = new TimerManager(this);

    auto *layout = new QVBoxLayout(this);

    // Display
    m_display = new QLabel("00:00:00", this);
    m_display->setAlignment(Qt::AlignCenter);
    m_display->setStyleSheet(
        "font-size:36px; font-weight:bold; font-family:'Consolas','Courier New';"
        "color:#333; background:#f0f0f0; border-radius:8px; padding:12px;");
    layout->addWidget(m_display);

    // Time setting
    auto *setRow = new QHBoxLayout();
    m_hourSpin = new QSpinBox(this);
    m_hourSpin->setRange(0, 99);
    m_hourSpin->setPrefix(QString::fromUtf8("时 "));
    m_minSpin = new QSpinBox(this);
    m_minSpin->setRange(0, 59);
    m_minSpin->setPrefix(QString::fromUtf8("分 "));
    m_secSpin = new QSpinBox(this);
    m_secSpin->setRange(0, 59);
    m_secSpin->setPrefix(QString::fromUtf8("秒 "));
    setRow->addWidget(m_hourSpin);
    setRow->addWidget(m_minSpin);
    setRow->addWidget(m_secSpin);
    layout->addLayout(setRow);

    // Buttons
    auto *btnRow = new QHBoxLayout();
    m_startBtn = new QPushButton(QString::fromUtf8("开始"), this);
    m_startBtn->setStyleSheet(
        "QPushButton{background:#2196F3;color:white;padding:8px 24px;border-radius:4px;}"
        "QPushButton:hover{background:#1976D2;}");
    m_resetBtn = new QPushButton(QString::fromUtf8("重置"), this);
    btnRow->addWidget(m_startBtn);
    btnRow->addWidget(m_resetBtn);
    layout->addLayout(btnRow);

    connect(m_startBtn, &QPushButton::clicked, this, &TimerPanel::onStartStop);
    connect(m_resetBtn, &QPushButton::clicked, this, &TimerPanel::onReset);
    connect(m_manager, &TimerManager::timerTick, this, &TimerPanel::updateDisplay);
    connect(m_manager, &TimerManager::timerFinished, this, &TimerPanel::onFinished);
    connect(m_hourSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int) {
        if (m_manager->timerState() == TimerManager::Stopped) {
            m_manager->setDuration(m_hourSpin->value(), m_minSpin->value(), m_secSpin->value());
        }
    });
    connect(m_minSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int) {
        if (m_manager->timerState() == TimerManager::Stopped) {
            m_manager->setDuration(m_hourSpin->value(), m_minSpin->value(), m_secSpin->value());
        }
    });
    connect(m_secSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int) {
        if (m_manager->timerState() == TimerManager::Stopped) {
            m_manager->setDuration(m_hourSpin->value(), m_minSpin->value(), m_secSpin->value());
        }
    });
}

void TimerPanel::updateButtons()
{
    auto state = m_manager->timerState();
    if (state == TimerManager::Running) {
        m_startBtn->setText(QString::fromUtf8("暂停"));
    } else {
        m_startBtn->setText(QString::fromUtf8("开始"));
    }
}

void TimerPanel::onStartStop()
{
    auto state = m_manager->timerState();
    if (state == TimerManager::Running) {
        m_manager->pauseTimer();
    } else {
        if (state == TimerManager::Stopped) {
            m_manager->setDuration(m_hourSpin->value(), m_minSpin->value(), m_secSpin->value());
        }
        m_manager->startTimer();
    }
    updateButtons();
}

void TimerPanel::onReset()
{
    m_manager->stopTimer();
    updateButtons();
}

void TimerPanel::updateDisplay(const QTime &remaining)
{
    m_display->setText(remaining.toString("HH:mm:ss"));
    if (m_manager->timerState() == TimerManager::Running) {
        m_startBtn->setText(QString::fromUtf8("暂停"));
    }
}

void TimerPanel::onFinished()
{
    m_display->setText("00:00:00");
    updateButtons();
    QApplication::beep();
    QApplication::alert(window(), 3000);
}
