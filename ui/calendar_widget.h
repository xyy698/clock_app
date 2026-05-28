#ifndef CALENDAR_WIDGET_H
#define CALENDAR_WIDGET_H

#include <QWidget>
#include <QDate>

// CalendarWidget — custom-drawn monthly calendar grid.
// Highlights today, dims non-month days, allows month navigation.
class CalendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarWidget(QWidget *parent = nullptr);

    QDate currentMonth() const { return m_month; }

public slots:
    void goToToday();
    void nextMonth();
    void prevMonth();
    void setDate(const QDate &date);

signals:
    void dateClicked(const QDate &date);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int cellAtPos(const QPoint &pos) const;
    QRect cellRect(int row, int col) const;

    QDate m_month;    // first day of displayed month
    QDate m_selected;
    int m_rows = 6;
    int m_cols = 7;
};

#endif // CALENDAR_WIDGET_H
