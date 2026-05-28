#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QStackedWidget>
#include <QLabel>
#include <QTimer>

class AnalogClockWidget;
class DigitalClockWidget;
class CalendarWidget;
class WorldClockPanel;
class AlarmDialog;
class TimerPanel;
class StopwatchPanel;
class SettingsDialog;
class NtpClient;
class QVBoxLayout;

// MainWindow — top-level window aggregating all sub-widgets.
// Manages tray icon, menus, display-mode switching, and eye protection.
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onTick(const QDateTime &dt);
    void switchToAnalog();
    void switchToDigital();
    void toggleAlwaysOnTop();
    void toggleIrregular();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void showMainWindow();
    void syncNtpTime();
    void checkEyeProtection();
    void startEyeRest();

private:
    void setupUi();
    void setupTray();
    void setupMenus();
    void applyPreferences();
    void updateWindowMask();

    // Core widgets
    QStackedWidget *m_stack;
    AnalogClockWidget *m_analogClock;
    DigitalClockWidget *m_digitalClock;
    QLabel *m_dateLabel;

    // Tray
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;

    // Eye protection
    QTimer *m_eyeTimer;
    int m_eyeMinutesElapsed = 0;
    bool m_resting = false;
    QWidget *m_restOverlay = nullptr;

    // Sub-panels (created on demand)
    CalendarWidget *m_calendar = nullptr;
    WorldClockPanel *m_worldClock = nullptr;
    AlarmDialog *m_alarmDlg = nullptr;
    TimerPanel *m_timerPanel = nullptr;
    StopwatchPanel *m_stopwatchPanel = nullptr;
    SettingsDialog *m_settingsDlg = nullptr;
    NtpClient *m_ntpClient = nullptr;

    // Drag
    QPoint m_dragPos;
    bool m_dragging = false;
};

#endif // MAIN_WINDOW_H
