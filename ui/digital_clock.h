#ifndef DIGITAL_CLOCK_H
#define DIGITAL_CLOCK_H

#include <QWidget>
#include <QDateTime>
#include <QTimeZone>
#include <QColor>

// DigitalClockWidget — draws a classic 7-segment LCD-style digital clock.
// Each digit is composed of 7 individually lit segments.
class DigitalClockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DigitalClockWidget(QWidget *parent = nullptr);

    void setTime(const QDateTime &dt);
    void setTimeZone(const QTimeZone &tz);

    // Colors: segment-on, segment-off, background
    void setOnColor(const QColor &c)    { m_onColor = c; }
    void setOffColor(const QColor &c)   { m_offColor = c; }
    void setBgColor(const QColor &c)    { m_bgColor = c; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    // Defines which of 7 segments are lit for digits 0-9.
    // Segments indexed: 0=top, 1=top-right, 2=bottom-right,
    // 3=bottom, 4=bottom-left, 5=top-left, 6=middle.
    static const bool s_segmentMap[10][7];
    static const bool s_colonMap[1][4]; // colon: two dots

    void drawSegment(QPainter &p, int segIndex, const QRectF &digitRect,
                     int segW, bool on);
    void drawDigit(QPainter &p, int digit, const QRectF &rect, int segW);
    void drawColon(QPainter &p, const QRectF &rect, int segW);

    QDateTime m_time;
    QTimeZone m_timeZone;

    QColor m_onColor   {40, 200, 80};   // bright green
    QColor m_offColor  {20, 50, 25};    // dim green
    QColor m_bgColor   {10, 15, 10};    // dark background
};

#endif // DIGITAL_CLOCK_H
