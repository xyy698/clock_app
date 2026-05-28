#include "calendar_widget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QLocale>

CalendarWidget::CalendarWidget(QWidget *parent)
    : QWidget(parent)
    , m_month(QDate::currentDate().year(), QDate::currentDate().month(), 1)
    , m_selected(QDate::currentDate())
{
    setMinimumSize(280, 200);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void CalendarWidget::goToToday()
{
    m_month = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    m_selected = QDate::currentDate();
    update();
}

void CalendarWidget::nextMonth()
{
    m_month = m_month.addMonths(1);
    update();
}

void CalendarWidget::prevMonth()
{
    m_month = m_month.addMonths(-1);
    update();
}

void CalendarWidget::setDate(const QDate &date)
{
    m_selected = date;
    update();
}

QRect CalendarWidget::cellRect(int row, int col) const
{
    double cw = static_cast<double>(width()) / m_cols;
    // Row 0 = header (day names)
    double headerH = height() * 0.12;
    double bodyH = height() - headerH;
    double rh = bodyH / m_rows;
    int x = static_cast<int>(col * cw);
    int y = static_cast<int>(headerH + row * rh);
    return QRect(x + 1, y + 1, static_cast<int>(cw) - 2, static_cast<int>(rh) - 2);
}

int CalendarWidget::cellAtPos(const QPoint &pos) const
{
    double headerH = height() * 0.12;
    if (pos.y() < headerH) return -1;
    double cw = static_cast<double>(width()) / m_cols;
    double bodyH = height() - headerH;
    double rh = bodyH / m_rows;
    int col = static_cast<int>(pos.x() / cw);
    int row = static_cast<int>((pos.y() - headerH) / rh);
    if (col < 0 || col >= m_cols || row < 0 || row >= m_rows) return -1;
    return row * m_cols + col;
}

void CalendarWidget::mousePressEvent(QMouseEvent *event)
{
    int idx = cellAtPos(event->pos());
    if (idx < 0) return;
    int row = idx / m_cols;
    int col = idx % m_cols;

    // Calculate the date for this cell
    int startDow = m_month.dayOfWeek() % 7; // Mon=1 ... Sun=7 → 0..6
    int dayNum = row * 7 + col - startDow + 1;

    QDate cellDate(m_month.year(), m_month.month(), 1);
    cellDate = cellDate.addDays(dayNum - 1);

    if (cellDate.month() != m_month.month()) {
        // Clicked a day from adjacent month — navigate
        m_month = QDate(cellDate.year(), cellDate.month(), 1);
    }
    m_selected = cellDate;
    emit dateClicked(m_selected);
    update();
}

void CalendarWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(250, 250, 250));

    QFont boldFont = font();
    boldFont.setBold(true);

    // Header — month/year title
    QRect titleRect(0, 0, width(), static_cast<int>(height() * 0.12));
    p.setFont(boldFont);
    p.setPen(QColor(40, 40, 40));
    p.drawText(titleRect, Qt::AlignCenter,
               m_month.toString("yyyy年 M月"));

    // Day-of-week headers
    QStringList dowNames;
    QLocale loc = QLocale::system();
    for (int d = 1; d <= 7; ++d) {
        dowNames << loc.dayName(d, QLocale::ShortFormat);
    }

    double cw = static_cast<double>(width()) / 7;
    double headerH = height() * 0.12;
    double bodyH = height() - headerH;
    double rh = bodyH / m_rows;

    QFont smallFont = font();
    smallFont.setPixelSize(static_cast<int>(rh * 0.35));
    p.setFont(smallFont);

    // Qt dayOfWeek: Mon=1..Sat=6, Sun=7.
    // firstCol = column of month's 1st day (0=Sun, 1=Mon..6=Sat)
    int firstCol = m_month.dayOfWeek() % 7; // Mon→1, Sun→0

    QDate today = QDate::currentDate();

    // Draw day-name headers
    for (int col = 0; col < 7; ++col) {
        QRect hdr(col * cw, 0, cw, headerH * 0.8);
        p.setPen(QColor(120, 120, 120));
        p.drawText(hdr, Qt::AlignBottom | Qt::AlignHCenter,
                   dowNames[(col + 6) % 7]); // start with Sunday
    }

    QFont dayFont = font();
    dayFont.setPixelSize(static_cast<int>(rh * 0.40));
    QFont todayFont = dayFont;
    todayFont.setBold(true);

    for (int row = 0; row < m_rows; ++row) {
        for (int col = 0; col < m_cols; ++col) {
            QRect cr = cellRect(row, col);
            int dayIdx = row * 7 + col - firstCol;
            QDate cellDate = m_month.addDays(dayIdx);

            bool isCurrentMonth = (cellDate.month() == m_month.month());
            bool isToday = (cellDate == today);
            bool isSel = (cellDate == m_selected);

            // Cell background
            if (isSel) {
                p.fillRect(cr, QColor(70, 130, 220));
                p.setPen(Qt::white);
            } else if (isToday) {
                p.fillRect(cr, QColor(255, 235, 200));
                p.setPen(QColor(40, 40, 40));
            } else if (!isCurrentMonth) {
                p.setPen(QColor(190, 190, 190));
            } else {
                p.setPen(QColor(40, 40, 40));
            }

            p.setFont(isToday ? todayFont : dayFont);
            p.drawText(cr, Qt::AlignCenter, QString::number(cellDate.day()));
        }
    }

    // Grid lines
    p.setPen(QPen(QColor(220, 220, 220), 0.5));
    for (int col = 1; col < m_cols; ++col) {
        int x = static_cast<int>(col * cw);
        p.drawLine(x, static_cast<int>(headerH), x, height());
    }
    for (int row = 1; row < m_rows; ++row) {
        int y = static_cast<int>(headerH + row * rh);
        p.drawLine(0, y, width(), y);
    }
}
