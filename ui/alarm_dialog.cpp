#include "alarm_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>

AlarmDialog::AlarmDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QString::fromUtf8("闹钟管理"));
    setMinimumSize(450, 400);

    auto *mainLayout = new QVBoxLayout(this);

    // ---- Alarm list ----
    m_list = new QListWidget(this);
    mainLayout->addWidget(new QLabel(QString::fromUtf8("已有闹钟："), this));
    mainLayout->addWidget(m_list);

    auto *btnRow = new QHBoxLayout();
    auto *delBtn = new QPushButton(QString::fromUtf8("删除"), this);
    auto *editBtn = new QPushButton(QString::fromUtf8("编辑"), this);
    btnRow->addWidget(delBtn);
    btnRow->addWidget(editBtn);
    btnRow->addStretch();
    mainLayout->addLayout(btnRow);

    // ---- New alarm form ----
    auto *formGroup = new QGroupBox(QString::fromUtf8("添加闹钟"), this);
    auto *form = new QVBoxLayout(formGroup);

    auto *timeRow = new QHBoxLayout();
    timeRow->addWidget(new QLabel(QString::fromUtf8("时间："), this));
    m_timeEdit = new QTimeEdit(QTime::currentTime(), this);
    m_timeEdit->setDisplayFormat("HH:mm");
    timeRow->addWidget(m_timeEdit);
    timeRow->addStretch();
    form->addLayout(timeRow);

    auto *repeatRow = new QHBoxLayout();
    repeatRow->addWidget(new QLabel(QString::fromUtf8("重复："), this));
    m_chkMon = new QCheckBox(QString::fromUtf8("一"), this);
    m_chkTue = new QCheckBox(QString::fromUtf8("二"), this);
    m_chkWed = new QCheckBox(QString::fromUtf8("三"), this);
    m_chkThu = new QCheckBox(QString::fromUtf8("四"), this);
    m_chkFri = new QCheckBox(QString::fromUtf8("五"), this);
    m_chkSat = new QCheckBox(QString::fromUtf8("六"), this);
    m_chkSun = new QCheckBox(QString::fromUtf8("日"), this);
    repeatRow->addWidget(m_chkMon);
    repeatRow->addWidget(m_chkTue);
    repeatRow->addWidget(m_chkWed);
    repeatRow->addWidget(m_chkThu);
    repeatRow->addWidget(m_chkFri);
    repeatRow->addWidget(m_chkSat);
    repeatRow->addWidget(m_chkSun);
    repeatRow->addStretch();
    form->addLayout(repeatRow);

    auto *optRow = new QHBoxLayout();
    optRow->addWidget(new QLabel(QString::fromUtf8("响铃时长(秒)："), this));
    m_durationSpin = new QSpinBox(this);
    m_durationSpin->setRange(10, 300);
    m_durationSpin->setValue(60);
    optRow->addWidget(m_durationSpin);
    optRow->addSpacing(10);
    optRow->addWidget(new QLabel(QString::fromUtf8("再响间隔(分)："), this));
    m_snoozeSpin = new QSpinBox(this);
    m_snoozeSpin->setRange(1, 30);
    m_snoozeSpin->setValue(5);
    optRow->addWidget(m_snoozeSpin);
    optRow->addStretch();
    form->addLayout(optRow);

    auto *soundRow = new QHBoxLayout();
    soundRow->addWidget(new QLabel(QString::fromUtf8("铃声文件："), this));
    m_soundEdit = new QLineEdit(this);
    m_soundEdit->setPlaceholderText(QString::fromUtf8("留空使用系统提示音"));
    soundRow->addWidget(m_soundEdit);
    form->addLayout(soundRow);

    auto *addBtn = new QPushButton(QString::fromUtf8("添加闹钟"), this);
    addBtn->setStyleSheet("QPushButton{background:#4CAF50;color:white;padding:6px 16px;}");
    form->addWidget(addBtn);

    mainLayout->addWidget(formGroup);

    // Connections
    connect(addBtn, &QPushButton::clicked, this, &AlarmDialog::onAdd);
    connect(delBtn, &QPushButton::clicked, this, &AlarmDialog::onDelete);
    connect(editBtn, &QPushButton::clicked, this, &AlarmDialog::onEdit);
    connect(&AlarmManager::instance(), &AlarmManager::alarmsChanged,
            this, &AlarmDialog::refreshList);

    refreshList();
}

void AlarmDialog::refreshList()
{
    m_list->clear();
    for (const auto &a : AlarmManager::instance().alarms()) {
        QString text = QString("%1  %2  [%3]  %4")
            .arg(a.time.toString("HH:mm"))
            .arg(a.repeatDescription())
            .arg(a.enabled ? QString::fromUtf8("启用") : QString::fromUtf8("禁用"))
            .arg(a.soundFile.isEmpty() ? QString::fromUtf8("系统音") : a.soundFile);
        m_list->addItem(text);
    }
}

void AlarmDialog::onAdd()
{
    Alarm a;
    a.time = m_timeEdit->time();
    QList<QCheckBox*> chks = {m_chkMon, m_chkTue, m_chkWed, m_chkThu,
                              m_chkFri, m_chkSat, m_chkSun};
    for (int i = 0; i < 7; ++i) {
        if (chks[i]->isChecked()) a.repeatDays.insert((i + 1) % 7);
        // Monday = Qt day 1 → index 0 in our list
        // We need 0=Sun, 1=Mon, ..., 6=Sat
    }
    // Fix: chks[0]=Mon → day 1, chks[6]=Sun → day 0
    // Our repeatDays uses 0=Sun,1=Mon...
    // chks index: 0=Mon(1), 1=Tue(2), ..., 5=Sat(6), 6=Sun(0)
    // Let's redo this:
    a.repeatDays.clear();
    if (m_chkSun->isChecked()) a.repeatDays.insert(0);
    if (m_chkMon->isChecked()) a.repeatDays.insert(1);
    if (m_chkTue->isChecked()) a.repeatDays.insert(2);
    if (m_chkWed->isChecked()) a.repeatDays.insert(3);
    if (m_chkThu->isChecked()) a.repeatDays.insert(4);
    if (m_chkFri->isChecked()) a.repeatDays.insert(5);
    if (m_chkSat->isChecked()) a.repeatDays.insert(6);

    a.durationSec = m_durationSpin->value();
    a.snoozeMin = m_snoozeSpin->value();
    a.soundFile = m_soundEdit->text().trimmed();
    a.enabled = true;

    AlarmManager::instance().addAlarm(a);
}

void AlarmDialog::onDelete()
{
    int row = m_list->currentRow();
    if (row < 0) return;
    const auto &alarms = AlarmManager::instance().alarms();
    if (row < alarms.size()) {
        AlarmManager::instance().removeAlarm(alarms[row].id);
    }
}

void AlarmDialog::onEdit()
{
    int row = m_list->currentRow();
    if (row < 0) return;
    const auto &alarms = AlarmManager::instance().alarms();
    if (row >= alarms.size()) return;

    Alarm a = alarms[row];
    m_timeEdit->setTime(a.time);
    m_chkSun->setChecked(a.repeatDays.contains(0));
    m_chkMon->setChecked(a.repeatDays.contains(1));
    m_chkTue->setChecked(a.repeatDays.contains(2));
    m_chkWed->setChecked(a.repeatDays.contains(3));
    m_chkThu->setChecked(a.repeatDays.contains(4));
    m_chkFri->setChecked(a.repeatDays.contains(5));
    m_chkSat->setChecked(a.repeatDays.contains(6));
    m_durationSpin->setValue(a.durationSec);
    m_snoozeSpin->setValue(a.snoozeMin);
    m_soundEdit->setText(a.soundFile);

    // Remove old, will re-add on next "Add" click
    AlarmManager::instance().removeAlarm(a.id);
}

// ---- AlarmNotifyDialog ----

AlarmNotifyDialog::AlarmNotifyDialog(const Alarm &alarm, QWidget *parent)
    : QDialog(parent), m_snoozeMin(alarm.snoozeMin)
{
    setWindowTitle(QString::fromUtf8("闹钟"));
    setFixedSize(320, 180);
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog);

    auto *layout = new QVBoxLayout(this);
    auto *msg = new QLabel(
        QString::fromUtf8("⏰ 闹钟时间到了！\n\n") +
        alarm.time.toString("HH:mm"), this);
    msg->setAlignment(Qt::AlignCenter);
    msg->setStyleSheet("font-size:18px; font-weight:bold;");
    layout->addWidget(msg);

    auto *btnRow = new QHBoxLayout();
    auto *closeBtn = new QPushButton(QString::fromUtf8("关闭"), this);
    auto *snoozeBtn = new QPushButton(
        QString::fromUtf8("再响(%1分钟)").arg(alarm.snoozeMin), this);
    btnRow->addWidget(closeBtn);
    btnRow->addWidget(snoozeBtn);
    layout->addLayout(btnRow);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(snoozeBtn, &QPushButton::clicked, this, [this, &alarm]() {
        m_snoozeMin = alarm.snoozeMin;
        // Re-add a one-shot alarm for snooze
        Alarm snoozeAlarm;
        snoozeAlarm.time = QTime::currentTime().addSecs(alarm.snoozeMin * 60);
        snoozeAlarm.durationSec = alarm.durationSec;
        snoozeAlarm.snoozeMin = alarm.snoozeMin;
        snoozeAlarm.soundFile = alarm.soundFile;
        AlarmManager::instance().addAlarm(snoozeAlarm);
        accept();
    });

    // Auto-close after duration
    QTimer::singleShot(alarm.durationSec * 1000, this, &QDialog::accept);

    // Beep if no custom sound
    if (alarm.soundFile.isEmpty()) {
        QApplication::beep();
    }
}
