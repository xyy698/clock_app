#ifndef ALARM_DIALOG_H
#define ALARM_DIALOG_H

#include <QDialog>
#include <QTimeEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QListWidget>
#include "core/alarm_manager.h"

// AlarmDialog — dialog for adding/editing alarms and listing existing ones.
class AlarmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmDialog(QWidget *parent = nullptr);

private slots:
    void onAdd();
    void onDelete();
    void onEdit();
    void refreshList();

private:
    QListWidget *m_list;
    QTimeEdit *m_timeEdit;
    QCheckBox *m_chkMon, *m_chkTue, *m_chkWed, *m_chkThu, *m_chkFri, *m_chkSat, *m_chkSun;
    QSpinBox *m_durationSpin;
    QSpinBox *m_snoozeSpin;
    QLineEdit *m_soundEdit;
};

// AlarmNotifyDialog — shown when an alarm fires.
class AlarmNotifyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmNotifyDialog(const Alarm &alarm, QWidget *parent = nullptr);

    int snoozeMinutes() const { return m_snoozeMin; }

private:
    int m_snoozeMin;
};

#endif // ALARM_DIALOG_H
