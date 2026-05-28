#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDateTimeEdit>

// SettingsDialog — user preferences configuration.
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void onApply();
    void onOk();

private:
    QComboBox *m_displayModeCombo;
    QCheckBox *m_alwaysOnTopChk;
    QCheckBox *m_irregularChk;
    QCheckBox *m_autoStartChk;
    QComboBox *m_timezoneCombo;
    QSpinBox *m_workSpin;
    QSpinBox *m_restSpin;
    QSpinBox *m_snoozeSpin;
    QDateTimeEdit *m_dateTimeEdit;
};

#endif // SETTINGS_DIALOG_H
