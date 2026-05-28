#include "world_clock.h"
#include "analog_clock.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDateTime>
#include <QTimeZone>

WorldClockPanel::WorldClockPanel(QWidget *parent)
    : QWidget(parent)
{
    auto *grid = new QGridLayout(this);
    grid->setSpacing(10);

    // Predefined cities with timezone IDs
    struct CityInfo { QString name; QByteArray tzId; };
    const QVector<CityInfo> cities = {
        {"北京",   "Asia/Shanghai"},
        {"伦敦",   "Europe/London"},
        {"莫斯科", "Europe/Moscow"},
        {"纽约",   "America/New_York"},
        {"东京",   "Asia/Tokyo"},
        {"悉尼",   "Australia/Sydney"},
    };

    int col = 0;
    int row = 0;
    for (const auto &ci : cities) {
        QTimeZone tz(ci.tzId);
        if (!tz.isValid()) tz = QTimeZone::utc();

        auto *clock = new AnalogClockWidget(this);
        clock->setMinimumSize(80, 80);
        clock->setMaximumSize(140, 140);
        clock->setScale(0.85f);
        clock->setShowSecondHand(false);
        clock->setFaceColor(QColor(250, 250, 250));

        auto *nameLabel = new QLabel(ci.name, this);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet("font-weight:bold; font-size:11px;");

        auto *timeLabel = new QLabel(this);
        timeLabel->setAlignment(Qt::AlignCenter);
        timeLabel->setStyleSheet("font-size:10px; color:#555;");

        auto *wrap = new QVBoxLayout();
        wrap->setSpacing(2);
        wrap->addWidget(clock);
        wrap->addWidget(nameLabel);
        wrap->addWidget(timeLabel);
        grid->addLayout(wrap, row, col);

        CityClock cc;
        cc.clock = clock;
        cc.nameLabel = nameLabel;
        cc.timeLabel = timeLabel;
        cc.timeZone = tz;
        cc.cityName = ci.name;
        m_clocks.append(cc);

        col++;
        if (col >= 3) { col = 0; row++; }
    }
}

void WorldClockPanel::updateClocks(const QDateTime &utcTime)
{
    for (auto &cc : m_clocks) {
        QDateTime local = utcTime.toTimeZone(cc.timeZone);
        cc.clock->setTime(local);
        cc.timeLabel->setText(local.toString("HH:mm:ss"));
    }
}
