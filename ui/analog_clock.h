#ifndef ANALOG_CLOCK_H
#define ANALOG_CLOCK_H

#include <QWidget>
#include <QDateTime>
#include <QTimeZone>
#include <QColor>

// AnalogClockWidget — custom-painted analog clock with hour, minute,
// and second hands. Supports configurable timezone for world-clock reuse.
class AnalogClockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnalogClockWidget(QWidget *parent = nullptr);

    void setTime(const QDateTime &dt);
    void setTimeZone(const QTimeZone &tz);
    QTimeZone timeZone() const { return m_timeZone; }

    // Appearance
    void setFaceColor(const QColor &c)   { m_faceColor = c; }
    void setHourHandColor(const QColor &c)  { m_hourColor = c; }
    void setMinuteHandColor(const QColor &c) { m_minuteColor = c; }
    void setSecondHandColor(const QColor &c) { m_secondColor = c; }
    void setShowSecondHand(bool show) { m_showSecond = show; update(); }
    void setScale(float scale) { m_scale = scale; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawFace(QPainter &p, const QRect &r);
    void drawHands(QPainter &p, const QRect &r);
    void drawTickMarks(QPainter &p, const QRect &r);

    QDateTime m_time;
    QTimeZone m_timeZone;

    QColor m_faceColor    {240, 240, 240};
    QColor m_hourColor    {40, 40, 40};
    QColor m_minuteColor  {60, 60, 60};
    QColor m_secondColor  {220, 40, 40};
    bool m_showSecond = true;
    float m_scale = 1.0f;
};

#endif // ANALOG_CLOCK_H
