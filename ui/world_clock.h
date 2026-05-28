#ifndef WORLD_CLOCK_H
#define WORLD_CLOCK_H

#include <QWidget>
#include <QVector>
#include <QTimeZone>

class AnalogClockWidget;
class QLabel;

// WorldClockPanel — displays multiple small analog clocks for
// major world cities with their respective timezones.
class WorldClockPanel : public QWidget
{
    Q_OBJECT

public:
    explicit WorldClockPanel(QWidget *parent = nullptr);

public slots:
    void updateClocks(const QDateTime &utcTime);

private:
    struct CityClock {
        AnalogClockWidget *clock;
        QLabel *nameLabel;
        QLabel *timeLabel;
        QTimeZone timeZone;
        QString cityName;
    };

    void addCity(const QString &name, const QByteArray &tzId);
    QVector<CityClock> m_clocks;
};

#endif // WORLD_CLOCK_H
