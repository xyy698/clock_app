#include "digital_clock.h"
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QtMath>

// Segment map: [digit][segment] = lit?
// Segments: 0=top, 1=top-right, 2=bottom-right, 3=bottom,
//           4=bottom-left, 5=top-left, 6=middle
const bool DigitalClockWidget::s_segmentMap[10][7] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1}, // 9
};

DigitalClockWidget::DigitalClockWidget(QWidget *parent)
    : QWidget(parent)
    , m_time(QDateTime::currentDateTime())
    , m_timeZone(QTimeZone::systemTimeZone())
{
    setMinimumSize(200, 60);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void DigitalClockWidget::setTime(const QDateTime &dt)
{
    m_time = dt;
    update();
}

void DigitalClockWidget::setTimeZone(const QTimeZone &tz)
{
    m_timeZone = tz;
    update();
}

void DigitalClockWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    painter.fillRect(rect(), m_bgColor);

    QTime time = m_time.toTimeZone(m_timeZone).time();
    int h = time.hour();
    int m = time.minute();
    int s = time.second();

    // Layout: HH:MM:SS — 8 "characters" (6 digits + 2 colons)
    int totalDigits = 8;
    double availW = width() * 0.92;
    double availH = height() * 0.80;
    double unitW = availW / static_cast<double>(totalDigits + 1); // +1 for spacing

    double startX = (width() - availW) / 2.0 + unitW * 0.3;
    double startY = (height() - availH) / 2.0;

    // Segment width proportional to digit height
    int segW = qMax(2, static_cast<int>(availH * 0.12));

    // Hour tens
    QRectF r0(startX, startY, unitW * 0.85, availH);
    drawDigit(painter, h / 10, r0, segW);

    // Hour ones
    QRectF r1(startX + unitW * 1.0, startY, unitW * 0.85, availH);
    drawDigit(painter, h % 10, r1, segW);

    // Colon 1
    QRectF rc1(startX + unitW * 2.0, startY, unitW * 0.6, availH);
    drawColon(painter, rc1, segW);

    // Minute tens
    QRectF r2(startX + unitW * 2.8, startY, unitW * 0.85, availH);
    drawDigit(painter, m / 10, r2, segW);

    // Minute ones
    QRectF r3(startX + unitW * 3.8, startY, unitW * 0.85, availH);
    drawDigit(painter, m % 10, r3, segW);

    // Colon 2
    QRectF rc2(startX + unitW * 4.8, startY, unitW * 0.6, availH);
    drawColon(painter, rc2, segW);

    // Second tens
    QRectF r4(startX + unitW * 5.6, startY, unitW * 0.85, availH);
    drawDigit(painter, s / 10, r4, segW);

    // Second ones
    QRectF r5(startX + unitW * 6.6, startY, unitW * 0.85, availH);
    drawDigit(painter, s % 10, r5, segW);
}

void DigitalClockWidget::drawDigit(QPainter &p, int digit, const QRectF &r, int segW)
{
    if (digit < 0 || digit > 9) return;
    for (int i = 0; i < 7; ++i) {
        drawSegment(p, i, r, segW, s_segmentMap[digit][i]);
    }
}

void DigitalClockWidget::drawSegment(QPainter &p, int idx, const QRectF &r,
                                      int segW, bool on)
{
    p.setPen(Qt::NoPen);
    p.setBrush(on ? m_onColor : m_offColor);

    double x = r.x(), y = r.y(), w = r.width(), h = r.height();
    double t = segW; // thickness
    double m = t * 0.3; // miter

    QPainterPath path;
    switch (idx) {
    case 0: // top horizontal
        path.moveTo(x + m, y);
        path.lineTo(x + w - m, y);
        path.lineTo(x + w - m - t, y + t);
        path.lineTo(x + m + t, y + t);
        path.closeSubpath();
        break;
    case 1: // top-right vertical
        path.moveTo(x + w, y + m);
        path.lineTo(x + w, y + h / 2 - m / 2);
        path.lineTo(x + w - t, y + h / 2 - m / 2 - t);
        path.lineTo(x + w - t, y + m + t);
        path.closeSubpath();
        break;
    case 2: // bottom-right vertical
        path.moveTo(x + w, y + h / 2 + m / 2);
        path.lineTo(x + w, y + h - m);
        path.lineTo(x + w - t, y + h - m - t);
        path.lineTo(x + w - t, y + h / 2 + m / 2 - t);
        path.closeSubpath();
        break;
    case 3: // bottom horizontal
        path.moveTo(x + m + t, y + h - t);
        path.lineTo(x + w - m - t, y + h - t);
        path.lineTo(x + w - m, y + h);
        path.lineTo(x + m, y + h);
        path.closeSubpath();
        break;
    case 4: // bottom-left vertical
        path.moveTo(x, y + h / 2 + m / 2);
        path.lineTo(x, y + h - m);
        path.lineTo(x + t, y + h - m - t);
        path.lineTo(x + t, y + h / 2 + m / 2 - t);
        path.closeSubpath();
        break;
    case 5: // top-left vertical
        path.moveTo(x, y + m);
        path.lineTo(x, y + h / 2 - m / 2);
        path.lineTo(x + t, y + h / 2 - m / 2 - t);
        path.lineTo(x + t, y + m + t);
        path.closeSubpath();
        break;
    case 6: // middle horizontal
        path.moveTo(x + m + t, y + h / 2 - t / 2);
        path.lineTo(x + w - m - t, y + h / 2 - t / 2);
        path.lineTo(x + w - m, y + h / 2 + t / 2);
        path.lineTo(x + m, y + h / 2 + t / 2);
        path.closeSubpath();
        break;
    }
    p.drawPath(path);
}

void DigitalClockWidget::drawColon(QPainter &p, const QRectF &r, int segW)
{
    double cx = r.center().x();
    double cy = r.center().y();
    double dotR = segW * 0.8;

    p.setPen(Qt::NoPen);
    bool blink = (m_time.toTimeZone(m_timeZone).time().second() % 2 == 0);
    p.setBrush(blink ? m_onColor : m_offColor);

    p.drawEllipse(QPointF(cx, cy - r.height() * 0.15), dotR, dotR);
    p.drawEllipse(QPointF(cx, cy + r.height() * 0.15), dotR, dotR);
}
