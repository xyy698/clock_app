#include "settings_dialog.h"
#include "core/preference_manager.h"
#include "core/clock_engine.h"
#include "utils/time_utils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QTimeZone>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QString::fromUtf8("设置"));
    setMinimumWidth(420);

    auto *mainLayout = new QVBoxLayout(this);
    auto &prefs = PreferenceManager::instance();

    // ---- Display group ----
    auto *dispGroup = new QGroupBox(QString::fromUtf8("显示"), this);
    auto *dispForm = new QFormLayout(dispGroup);

    m_displayModeCombo = new QComboBox(this);
    m_displayModeCombo->addItem(QString::fromUtf8("指针式"), "analog");
    m_displayModeCombo->addItem(QString::fromUtf8("数字式"), "digital");
    int idx = m_displayModeCombo->findData(prefs.displayMode());
    if (idx >= 0) m_displayModeCombo->setCurrentIndex(idx);
    dispForm->addRow(QString::fromUtf8("显示模式："), m_displayModeCombo);
    mainLayout->addWidget(dispGroup);

    // ---- Window group ----
    auto *winGroup = new QGroupBox(QString::fromUtf8("窗口"), this);
    auto *winForm = new QFormLayout(winGroup);

    m_alwaysOnTopChk = new QCheckBox(QString::fromUtf8("总在最前面"), this);
    m_alwaysOnTopChk->setChecked(prefs.alwaysOnTop());
    winForm->addRow(m_alwaysOnTopChk);

    m_irregularChk = new QCheckBox(QString::fromUtf8("不规则窗口（圆形）"), this);
    m_irregularChk->setChecked(prefs.irregularWindow());
    winForm->addRow(m_irregularChk);

    m_autoStartChk = new QCheckBox(QString::fromUtf8("随系统启动"), this);
    m_autoStartChk->setChecked(prefs.autoStart());
    winForm->addRow(m_autoStartChk);

    mainLayout->addWidget(winGroup);

    // ---- Timezone group ----
    auto *tzGroup = new QGroupBox(QString::fromUtf8("时区"), this);
    auto *tzForm = new QFormLayout(tzGroup);

    m_timezoneCombo = new QComboBox(this);
    for (const auto &tz : TimeUtils::commonTimezones()) {
        m_timezoneCombo->addItem(tz.first, QString(tz.second));
    }
    QString curTz = prefs.timeZoneId();
    int tzIdx = m_timezoneCombo->findData(curTz);
    if (tzIdx >= 0) m_timezoneCombo->setCurrentIndex(tzIdx);
    tzForm->addRow(QString::fromUtf8("时区："), m_timezoneCombo);
    mainLayout->addWidget(tzGroup);

    // ---- System time group ----
    auto *sysGroup = new QGroupBox(QString::fromUtf8("系统时间"), this);
    auto *sysForm = new QFormLayout(sysGroup);
    m_dateTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    m_dateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    sysForm->addRow(QString::fromUtf8("修改系统时间："), m_dateTimeEdit);
    mainLayout->addWidget(sysGroup);

    // ---- Eye protection group ----
    auto *eyeGroup = new QGroupBox(QString::fromUtf8("视力保护"), this);
    auto *eyeForm = new QFormLayout(eyeGroup);
    m_workSpin = new QSpinBox(this);
    m_workSpin->setRange(10, 180);
    m_workSpin->setSuffix(QString::fromUtf8(" 分钟"));
    m_workSpin->setValue(prefs.workInterval());
    eyeForm->addRow(QString::fromUtf8("工作间隔："), m_workSpin);

    m_restSpin = new QSpinBox(this);
    m_restSpin->setRange(1, 30);
    m_restSpin->setSuffix(QString::fromUtf8(" 分钟"));
    m_restSpin->setValue(prefs.restDuration());
    eyeForm->addRow(QString::fromUtf8("休息时长："), m_restSpin);
    mainLayout->addWidget(eyeGroup);

    // ---- Alarm defaults ----
    auto *alarmGroup = new QGroupBox(QString::fromUtf8("闹钟默认值"), this);
    auto *alarmForm = new QFormLayout(alarmGroup);
    m_snoozeSpin = new QSpinBox(this);
    m_snoozeSpin->setRange(1, 30);
    m_snoozeSpin->setSuffix(QString::fromUtf8(" 分钟"));
    m_snoozeSpin->setValue(prefs.snoozeMinutes());
    alarmForm->addRow(QString::fromUtf8("再响间隔："), m_snoozeSpin);
    mainLayout->addWidget(alarmGroup);

    // ---- Buttons ----
    auto *btnRow = new QHBoxLayout();
    auto *okBtn = new QPushButton(QString::fromUtf8("确定"), this);
    auto *applyBtn = new QPushButton(QString::fromUtf8("应用"), this);
    auto *cancelBtn = new QPushButton(QString::fromUtf8("取消"), this);
    btnRow->addStretch();
    btnRow->addWidget(okBtn);
    btnRow->addWidget(applyBtn);
    btnRow->addWidget(cancelBtn);
    mainLayout->addLayout(btnRow);

    connect(applyBtn, &QPushButton::clicked, this, &SettingsDialog::onApply);
    connect(okBtn, &QPushButton::clicked, this, &SettingsDialog::onOk);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void SettingsDialog::onApply()
{
    auto &prefs = PreferenceManager::instance();

    prefs.setDisplayMode(m_displayModeCombo->currentData().toString());
    prefs.setAlwaysOnTop(m_alwaysOnTopChk->isChecked());
    prefs.setIrregularWindow(m_irregularChk->isChecked());
    prefs.setAutoStart(m_autoStartChk->isChecked());
    prefs.setWorkInterval(m_workSpin->value());
    prefs.setRestDuration(m_restSpin->value());
    prefs.setSnoozeMinutes(m_snoozeSpin->value());

    QString tzId = m_timezoneCombo->currentData().toString();
    prefs.setTimeZoneId(tzId);
    QTimeZone tz(tzId.toUtf8());
    if (tz.isValid()) {
        ClockEngine::instance().setTimeZone(tz);
    }

    // System time
    QDateTime newTime = m_dateTimeEdit->dateTime();
    if (newTime != QDateTime::currentDateTime()) {
        bool ok = ClockEngine::instance().setSystemTime(newTime);
        if (!ok) {
            QMessageBox::warning(this, QString::fromUtf8("错误"),
                QString::fromUtf8("无法设置系统时间（需要管理员权限）。"));
        }
    }

    prefs.save();
}

void SettingsDialog::onOk()
{
    onApply();
    accept();
}
