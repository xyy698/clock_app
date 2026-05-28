#include "stopwatch_panel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

StopwatchPanel::StopwatchPanel(QWidget *parent)
    : QWidget(parent)
{
    m_manager = new TimerManager(this);

    auto *layout = new QVBoxLayout(this);

    // Display
    m_display = new QLabel("00:00:00.000", this);
    m_display->setAlignment(Qt::AlignCenter);
    m_display->setStyleSheet(
        "font-size:34px; font-weight:bold; font-family:'Consolas','Courier New';"
        "color:#222; background:#f5f5f5; border-radius:8px; padding:10px;");
    layout->addWidget(m_display);

    // Buttons
    auto *btnRow = new QHBoxLayout();
    m_startBtn = new QPushButton(QString::fromUtf8("开始"), this);
    m_startBtn->setStyleSheet(
        "QPushButton{background:#FF5722;color:white;padding:8px 20px;border-radius:4px;}"
        "QPushButton:hover{background:#E64A19;}");
    m_lapBtn = new QPushButton(QString::fromUtf8("计次"), this);
    m_lapBtn->setEnabled(false);
    m_resetBtn = new QPushButton(QString::fromUtf8("重置"), this);
    btnRow->addWidget(m_startBtn);
    btnRow->addWidget(m_lapBtn);
    btnRow->addWidget(m_resetBtn);
    layout->addLayout(btnRow);

    // Lap list
    m_lapList = new QListWidget(this);
    layout->addWidget(m_lapList);

    connect(m_startBtn, &QPushButton::clicked, this, &StopwatchPanel::onStartStop);
    connect(m_lapBtn, &QPushButton::clicked, this, &StopwatchPanel::onLap);
    connect(m_resetBtn, &QPushButton::clicked, this, &StopwatchPanel::onReset);
    connect(m_manager, &TimerManager::stopwatchTick, this, &StopwatchPanel::updateDisplay);
}

void StopwatchPanel::updateButtons()
{
    auto state = m_manager->stopwatchState();
    if (state == TimerManager::Running) {
        m_startBtn->setText(QString::fromUtf8("暂停"));
        m_lapBtn->setEnabled(true);
    } else {
        m_startBtn->setText(QString::fromUtf8("开始"));
        m_lapBtn->setEnabled(false);
    }
}

QString StopwatchPanel::formatMillis(qint64 ms) const
{
    int h = ms / 3600000;
    int m = (ms % 3600000) / 60000;
    int s = (ms % 60000) / 1000;
    int mi = ms % 1000;
    return QString("%1:%2:%3.%4")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'))
        .arg(mi, 3, 10, QChar('0'));
}

void StopwatchPanel::onStartStop()
{
    auto state = m_manager->stopwatchState();
    if (state == TimerManager::Running) {
        m_manager->pauseStopwatch();
    } else {
        m_manager->startStopwatch();
    }
    updateButtons();
}

void StopwatchPanel::onLap()
{
    qint64 lap = m_manager->recordLap();
    const auto &laps = m_manager->laps();
    m_lapList->insertItem(0,
        QString::fromUtf8("计次 %1: %2")
            .arg(laps.size())
            .arg(formatMillis(lap)));
}

void StopwatchPanel::onReset()
{
    m_manager->stopStopwatch();
    m_lapList->clear();
    updateButtons();
    m_display->setText("00:00:00.000");
}

void StopwatchPanel::updateDisplay(qint64 millis)
{
    m_display->setText(formatMillis(millis));
    if (m_manager->stopwatchState() == TimerManager::Running) {
        m_startBtn->setText(QString::fromUtf8("暂停"));
        m_lapBtn->setEnabled(true);
    }
}
