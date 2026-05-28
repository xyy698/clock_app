#include "analog_clock.h"
#include <QPainter>
#include <QPaintEvent>
#include <QtMath>

AnalogClockWidget::AnalogClockWidget(QWidget *parent)
    : QWidget(parent)
    , m_time(QDateTime::currentDateTime())
    , m_timeZone(QTimeZone::systemTimeZone())
{
    setMinimumSize(60, 60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void AnalogClockWidget::setTime(const QDateTime &dt)
{
    m_time = dt;
    update();
}

void AnalogClockWidget::setTimeZone(const QTimeZone &tz)
{
    m_timeZone = tz;
    update();
}

void AnalogClockWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());
    int cx = width() / 2;
    int cy = height() / 2;
    int r = static_cast<int>(side / 2 * 0.90 * m_scale);
    QRect clockRect(cx - r, cy - r, r * 2, r * 2);

    drawFace(painter, clockRect);
    drawTickMarks(painter, clockRect);
    drawHands(painter, clockRect);
}

void AnalogClockWidget::drawFace(QPainter &p, const QRect &r)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(m_faceColor);
    p.drawEllipse(r);

    // Outer ring
    p.setBrush(Qt::NoBrush);
    QPen ringPen(QColor(80, 80, 80), 2.0 * m_scale);
    p.setPen(ringPen);
    p.drawEllipse(r.adjusted(1, 1, -1, -1));
    p.restore();
}

void AnalogClockWidget::drawTickMarks(QPainter &p, const QRect &r)
{
    int cx = r.center().x();
    int cy = r.center().y();
    int outerR = r.width() / 2;
    int innerR;

    p.save();
    for (int i = 0; i < 60; ++i) {
        double angle = (i * 6.0 - 90.0) * M_PI / 180.0;
        double cosA = cos(angle);
        double sinA = sin(angle);

        if (i % 5 == 0) {
            // Hour mark
            innerR = static_cast<int>(outerR * 0.82);
            QPen pen(QColor(40, 40, 40), 2.5 * m_scale);
            p.setPen(pen);
        } else {
            // Minute mark
            innerR = static_cast<int>(outerR * 0.90);
            QPen pen(QColor(160, 160, 160), 1.0 * m_scale);
            p.setPen(pen);
        }

        int x1 = cx + static_cast<int>(innerR * cosA);
        int y1 = cy + static_cast<int>(innerR * sinA);
        int x2 = cx + static_cast<int>(outerR * 0.95 * cosA);
        int y2 = cy + static_cast<int>(outerR * 0.95 * sinA);
        p.drawLine(x1, y1, x2, y2);
    }

    // Hour numerals (12, 3, 6, 9)
    p.setPen(QColor(40, 40, 40));
    QFont font = p.font();
    font.setPixelSize(static_cast<int>(outerR * 0.14));
    font.setBold(true);
    p.setFont(font);
    for (int h = 1; h <= 12; ++h) {
        double angle = (h * 30.0 - 90.0) * M_PI / 180.0;
        int numR = static_cast<int>(outerR * 0.70);
        int nx = cx + static_cast<int>(numR * cos(angle));
        int ny = cy + static_cast<int>(numR * sin(angle));
        QRect textRect(nx - 15, ny - 15, 30, 30);
        p.drawText(textRect, Qt::AlignCenter, QString::number(h));
    }
    p.restore();
}

void AnalogClockWidget::drawHands(QPainter &p, const QRect &r)
{
    int cx = r.center().x();
    int cy = r.center().y();

    QTime time = m_time.toTimeZone(m_timeZone).time();
    double hour = time.hour() % 12 + time.minute() / 60.0;
    double minute = time.minute() + time.second() / 60.0;
    double second = time.second() + time.msec() / 1000.0;

    double hAngle = (hour   * 30.0 - 90.0) * M_PI / 180.0;
    double mAngle = (minute *  6.0 - 90.0) * M_PI / 180.0;
    double sAngle = (second *  6.0 - 90.0) * M_PI / 180.0;

    int maxR = r.width() / 2;

    p.save();

    // Hour hand (short, thick)
    {
        int len = static_cast<int>(maxR * 0.48);
        QPen pen(m_hourColor, 5.0 * m_scale, Qt::SolidLine, Qt::RoundCap);
        p.setPen(pen);
        p.drawLine(cx, cy,
                   cx + static_cast<int>(len * cos(hAngle)),
                   cy + static_cast<int>(len * sin(hAngle)));
    }

    // Minute hand (longer, medium)
    {
        int len = static_cast<int>(maxR * 0.65);
        QPen pen(m_minuteColor, 3.0 * m_scale, Qt::SolidLine, Qt::RoundCap);
        p.setPen(pen);
        p.drawLine(cx, cy,
                   cx + static_cast<int>(len * cos(mAngle)),
                   cy + static_cast<int>(len * sin(mAngle)));
    }

    // Second hand (longest, thin, red)
    if (m_showSecond) {
        int len = static_cast<int>(maxR * 0.80);
        QPen pen(m_secondColor, 1.5 * m_scale);
        p.setPen(pen);
        p.drawLine(cx, cy,
                   cx + static_cast<int>(len * cos(sAngle)),
                   cy + static_cast<int>(len * sin(sAngle)));
        // Tail
        int tailLen = static_cast<int>(maxR * 0.15);
        p.drawLine(cx, cy,
                   cx - static_cast<int>(tailLen * cos(sAngle)),
                   cy - static_cast<int>(tailLen * sin(sAngle)));
    }

    // Center dot
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(40, 40, 40));
    p.drawEllipse(QPoint(cx, cy), static_cast<int>(4 * m_scale),
                  static_cast<int>(4 * m_scale));

    p.restore();
}
