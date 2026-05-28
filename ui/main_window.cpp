#include "main_window.h"
#include "analog_clock.h"
#include "digital_clock.h"
#include "calendar_widget.h"
#include "world_clock.h"
#include "alarm_dialog.h"
#include "timer_panel.h"
#include "stopwatch_panel.h"
#include "settings_dialog.h"
#include "core/clock_engine.h"
#include "core/preference_manager.h"
#include "core/alarm_manager.h"
#include "utils/ntp_client.h"
#include "utils/time_utils.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <QScreen>
#include <QMessageBox>
#include <QDialog>
#include <QDockWidget>
#include <QScrollArea>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setupTray();
    setupMenus();
    applyPreferences();

    // Connect to clock engine
    connect(&ClockEngine::instance(), &ClockEngine::tick,
            this, &MainWindow::onTick);

    // Connect alarm manager
    connect(&AlarmManager::instance(), &AlarmManager::alarmTriggered,
            this, [this](const Alarm &alarm) {
        auto *dlg = new AlarmNotifyDialog(alarm, this);
        dlg->show();
    });

    // Eye protection timer
    m_eyeTimer = new QTimer(this);
    m_eyeTimer->setInterval(60000); // every minute
    connect(m_eyeTimer, &QTimer::timeout, this, &MainWindow::checkEyeProtection);
    m_eyeTimer->start();
}

MainWindow::~MainWindow()
{
    PreferenceManager::instance().setWindowPos(pos());
    PreferenceManager::instance().setWindowSize(size());
    PreferenceManager::instance().save();
}

void MainWindow::setupUi()
{
    setWindowTitle("多功能时钟");
    resize(PreferenceManager::instance().windowSize());
    move(PreferenceManager::instance().windowPos());

    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(6);

    // Date label
    m_dateLabel = new QLabel(this);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_dateLabel->setStyleSheet("font-size:13px; color:#555; padding:2px;");
    mainLayout->addWidget(m_dateLabel);

    // Stacked clock display
    m_stack = new QStackedWidget(this);

    m_analogClock = new AnalogClockWidget(this);
    m_analogClock->setMinimumSize(280, 280);
    m_stack->addWidget(m_analogClock);

    m_digitalClock = new DigitalClockWidget(this);
    m_digitalClock->setMinimumSize(280, 80);
    m_stack->addWidget(m_digitalClock);

    mainLayout->addWidget(m_stack, 1);

    // Switch tool buttons
    auto *switchRow = new QHBoxLayout();
    auto *analogBtn = new QPushButton(QString::fromUtf8("指针式"), this);
    auto *digitalBtn = new QPushButton(QString::fromUtf8("数字式"), this);
    analogBtn->setStyleSheet("QPushButton{padding:4px 12px;}");
    digitalBtn->setStyleSheet("QPushButton{padding:4px 12px;}");
    switchRow->addStretch();
    switchRow->addWidget(analogBtn);
    switchRow->addWidget(digitalBtn);
    switchRow->addStretch();
    mainLayout->addLayout(switchRow);

    connect(analogBtn, &QPushButton::clicked, this, &MainWindow::switchToAnalog);
    connect(digitalBtn, &QPushButton::clicked, this, &MainWindow::switchToDigital);

    // Status bar
    statusBar()->showMessage(QString::fromUtf8("就绪"));

    // Set initial display mode
    QString mode = PreferenceManager::instance().displayMode();
    if (mode == "digital") {
        m_stack->setCurrentWidget(m_digitalClock);
    } else {
        m_stack->setCurrentWidget(m_analogClock);
    }
}

void MainWindow::setupTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qDebug() << "System tray not available, skipping.";
        m_trayIcon = nullptr;
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    m_trayIcon->setToolTip(QString::fromUtf8("多功能时钟"));

    m_trayMenu = new QMenu(this);
    m_trayMenu->addAction(QString::fromUtf8("显示主窗口"), this, &MainWindow::showMainWindow);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(QString::fromUtf8("退出"), qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &MainWindow::onTrayActivated);
}

void MainWindow::setupMenus()
{
    // ---- File menu ----
    auto *fileMenu = menuBar()->addMenu(QString::fromUtf8("文件"));

    fileMenu->addAction(QString::fromUtf8("显示月历"), this, [this]() {
        if (!m_calendar) {
            m_calendar = new CalendarWidget(nullptr);
            m_calendar->setWindowTitle(QString::fromUtf8("月历"));
            m_calendar->resize(320, 280);
        }
        m_calendar->goToToday();
        m_calendar->show();
        m_calendar->raise();
    });

    fileMenu->addAction(QString::fromUtf8("设置"), this, [this]() {
        if (!m_settingsDlg) {
            m_settingsDlg = new SettingsDialog(this);
        }
        if (m_settingsDlg->exec() == QDialog::Accepted) {
            applyPreferences();
        }
    });

    fileMenu->addSeparator();
    fileMenu->addAction(QString::fromUtf8("退出"), qApp, &QApplication::quit);

    // ---- View menu ----
    auto *viewMenu = menuBar()->addMenu(QString::fromUtf8("视图"));

    viewMenu->addAction(QString::fromUtf8("指针式"), this, &MainWindow::switchToAnalog);
    viewMenu->addAction(QString::fromUtf8("数字式"), this, &MainWindow::switchToDigital);

    auto *topAction = viewMenu->addAction(QString::fromUtf8("总在最前"));
    topAction->setCheckable(true);
    topAction->setChecked(PreferenceManager::instance().alwaysOnTop());
    connect(topAction, &QAction::toggled, this, &MainWindow::toggleAlwaysOnTop);

    // ---- Tools menu ----
    auto *toolsMenu = menuBar()->addMenu(QString::fromUtf8("工具"));

    toolsMenu->addAction(QString::fromUtf8("世界时钟"), this, [this]() {
        if (!m_worldClock) {
            m_worldClock = new WorldClockPanel(nullptr);
            m_worldClock->setWindowTitle(QString::fromUtf8("世界时钟"));
            m_worldClock->resize(500, 320);
        }
        m_worldClock->show();
        m_worldClock->raise();
    });

    toolsMenu->addAction(QString::fromUtf8("闹钟"), this, [this]() {
        if (!m_alarmDlg) {
            m_alarmDlg = new AlarmDialog(this);
        }
        m_alarmDlg->show();
        m_alarmDlg->raise();
    });

    toolsMenu->addAction(QString::fromUtf8("计时器"), this, [this]() {
        if (!m_timerPanel) {
            m_timerPanel = new TimerPanel(nullptr);
            m_timerPanel->setWindowTitle(QString::fromUtf8("计时器"));
            m_timerPanel->resize(320, 220);
        }
        m_timerPanel->show();
        m_timerPanel->raise();
    });

    toolsMenu->addAction(QString::fromUtf8("秒表"), this, [this]() {
        if (!m_stopwatchPanel) {
            m_stopwatchPanel = new StopwatchPanel(nullptr);
            m_stopwatchPanel->setWindowTitle(QString::fromUtf8("秒表"));
            m_stopwatchPanel->resize(360, 350);
        }
        m_stopwatchPanel->show();
        m_stopwatchPanel->raise();
    });

    toolsMenu->addAction(QString::fromUtf8("同步网络时间"), this, &MainWindow::syncNtpTime);

    // ---- Help menu ----
    auto *helpMenu = menuBar()->addMenu(QString::fromUtf8("帮助"));
    helpMenu->addAction(QString::fromUtf8("关于"), this, [this]() {
        QMessageBox::about(this, QString::fromUtf8("关于多功能时钟"),
            QString::fromUtf8("多功能桌面时钟 v1.0\n\n"
                              "核心功能：\n"
                              "• 数字式显示时分秒\n"
                              "• 指针式显示时分秒\n"
                              "• 自由切换显示模式\n"
                              "• 显示星期和日期\n"
                              "• 更改日期和时间\n\n"
                              "扩展功能：\n"
                              "• 月历、世界时钟、NTP授时\n"
                              "• 闹钟、计时器、秒表\n"
                              "• 视力保护、外观特效\n"
                              "• 使用Qt6/C++17实现"));
    });
}

void MainWindow::applyPreferences()
{
    auto &prefs = PreferenceManager::instance();

    // Display mode
    if (prefs.displayMode() == "digital") {
        m_stack->setCurrentWidget(m_digitalClock);
    } else {
        m_stack->setCurrentWidget(m_analogClock);
    }

    // Always on top — only toggle if different from current state
    bool wantTop = prefs.alwaysOnTop();
    bool isTop = windowFlags() & Qt::WindowStaysOnTopHint;
    if (wantTop != isTop) {
        bool wasVisible = isVisible();
        if (wantTop) {
            setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        } else {
            setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        }
        if (wasVisible) show();
    }

    // Timezone
    QString tzId = prefs.timeZoneId();
    QTimeZone tz(tzId.toUtf8());
    if (tz.isValid()) {
        ClockEngine::instance().setTimeZone(tz);
    }

    updateWindowMask();
}

void MainWindow::updateWindowMask()
{
    if (PreferenceManager::instance().irregularWindow()) {
        // Circular mask
        int side = qMin(width(), height());
        QRegion region((width() - side) / 2, (height() - side) / 2, side, side,
                       QRegion::Ellipse);
        setMask(region);
    } else {
        setMask(QRegion()); // clear mask
    }
}

void MainWindow::onTick(const QDateTime &dt)
{
    // Update clock widgets
    m_analogClock->setTime(dt);
    m_digitalClock->setTime(dt);

    // Update date label
    QString tzName = ClockEngine::instance().timeZone().id().mid(0, 20);
    m_dateLabel->setText(TimeUtils::formatWithWeekday(dt) +
                         "  [" + tzName + "]");

    // Update tray tooltip
    m_trayIcon->setToolTip(dt.toString("HH:mm:ss  yyyy-MM-dd"));

    // Update world clock if visible
    if (m_worldClock && m_worldClock->isVisible()) {
        m_worldClock->updateClocks(dt.toUTC());
    }

    // Check alarms
    AlarmManager::instance().checkAlarms(dt.time(), dt.date().dayOfWeek());
}

void MainWindow::switchToAnalog()
{
    m_stack->setCurrentWidget(m_analogClock);
    PreferenceManager::instance().setDisplayMode("analog");
}

void MainWindow::switchToDigital()
{
    m_stack->setCurrentWidget(m_digitalClock);
    PreferenceManager::instance().setDisplayMode("digital");
}

void MainWindow::toggleAlwaysOnTop()
{
    bool on = !(windowFlags() & Qt::WindowStaysOnTopHint);
    if (on) {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    }
    PreferenceManager::instance().setAlwaysOnTop(on);
    show();
}

void MainWindow::toggleIrregular()
{
    bool irr = !PreferenceManager::instance().irregularWindow();
    PreferenceManager::instance().setIrregularWindow(irr);
    updateWindowMask();
}

void MainWindow::syncNtpTime()
{
    if (!m_ntpClient) {
        m_ntpClient = new NtpClient(this);
        connect(m_ntpClient, &NtpClient::timeReceived, this,
                [this](const QDateTime &utc, qint64 offsetMs) {
            statusBar()->showMessage(
                QString::fromUtf8("NTP同步成功！偏差: %1ms").arg(offsetMs), 5000);
        });
    }
    statusBar()->showMessage(QString::fromUtf8("正在同步NTP时间..."));
    m_ntpClient->requestTime();
}

void MainWindow::checkEyeProtection()
{
    if (m_resting) return;

    m_eyeMinutesElapsed++;
    int workInterval = PreferenceManager::instance().workInterval();
    if (m_eyeMinutesElapsed >= workInterval) {
        startEyeRest();
    }
}

void MainWindow::startEyeRest()
{
    m_resting = true;
    m_eyeMinutesElapsed = 0;

    int restDuration = PreferenceManager::instance().restDuration();

    // Full-screen semi-transparent overlay
    m_restOverlay = new QWidget(nullptr);
    m_restOverlay->setWindowFlags(
        Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    m_restOverlay->setStyleSheet("background:rgba(0,0,0,220);");
    m_restOverlay->setCursor(Qt::BlankCursor);

    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        m_restOverlay->setGeometry(screen->geometry());
    }

    auto *msgLabel = new QLabel(m_restOverlay);
    msgLabel->setAlignment(Qt::AlignCenter);
    msgLabel->setStyleSheet(
        "color:white; font-size:28px; font-weight:bold; background:transparent;");
    msgLabel->setText(QString::fromUtf8("休息一下~\n保护视力"));
    msgLabel->setGeometry(m_restOverlay->rect());

    // Countdown label
    auto *countLabel = new QLabel(m_restOverlay);
    countLabel->setAlignment(Qt::AlignCenter);
    countLabel->setStyleSheet(
        "color:#aaa; font-size:48px; background:transparent;");
    countLabel->setGeometry(0, m_restOverlay->height() / 2 + 40,
                            m_restOverlay->width(), 60);

    m_restOverlay->showFullScreen();

    // Countdown timer
    int remaining = restDuration * 60;
    auto *restTimer = new QTimer(this);
    restTimer->setInterval(1000);
    connect(restTimer, &QTimer::timeout, this, [=]() mutable {
        remaining--;
        int mins = remaining / 60;
        int secs = remaining % 60;
        countLabel->setText(QString("%1:%2")
            .arg(mins, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0')));
        if (remaining <= 0) {
            restTimer->stop();
            restTimer->deleteLater();
            m_restOverlay->deleteLater();
            m_restOverlay = nullptr;
            m_resting = false;
        }
    });
    restTimer->start();
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        showMainWindow();
    }
}

void MainWindow::showMainWindow()
{
    showNormal();
    raise();
    activateWindow();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Minimize to tray instead of closing
    if (m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        m_trayIcon->showMessage(
            QString::fromUtf8("多功能时钟"),
            QString::fromUtf8("程序已最小化到托盘。"),
            QSystemTrayIcon::Information, 2000);
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPos = event->globalPos() - frameGeometry().topLeft();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragPos);
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_dragging = false;
    QMainWindow::mouseReleaseEvent(event);
}
