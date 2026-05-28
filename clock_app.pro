QT       += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = ClockApp
TEMPLATE = app

SOURCES += \
    main.cpp \
    core/clock_engine.cpp \
    core/alarm_manager.cpp \
    core/timer_manager.cpp \
    core/preference_manager.cpp \
    ui/main_window.cpp \
    ui/analog_clock.cpp \
    ui/digital_clock.cpp \
    ui/calendar_widget.cpp \
    ui/world_clock.cpp \
    ui/alarm_dialog.cpp \
    ui/timer_panel.cpp \
    ui/stopwatch_panel.cpp \
    ui/settings_dialog.cpp \
    utils/time_utils.cpp \
    utils/ntp_client.cpp

HEADERS += \
    core/clock_engine.h \
    core/alarm_manager.h \
    core/timer_manager.h \
    core/preference_manager.h \
    ui/main_window.h \
    ui/analog_clock.h \
    ui/digital_clock.h \
    ui/calendar_widget.h \
    ui/world_clock.h \
    ui/alarm_dialog.h \
    ui/timer_panel.h \
    ui/stopwatch_panel.h \
    ui/settings_dialog.h \
    utils/time_utils.h \
    utils/ntp_client.h
