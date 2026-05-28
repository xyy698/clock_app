#include "time_utils.h"
#include <QLocale>

QList<QPair<QString, QByteArray>> TimeUtils::commonTimezones()
{
    return {
        {"UTC",           "UTC"},
        {"北京 (UTC+8)",   "Asia/Shanghai"},
        {"东京 (UTC+9)",   "Asia/Tokyo"},
        {"伦敦 (UTC+0)",   "Europe/London"},
        {"莫斯科 (UTC+3)", "Europe/Moscow"},
        {"纽约 (UTC-5)",   "America/New_York"},
        {"洛杉矶 (UTC-8)", "America/Los_Angeles"},
        {"悉尼 (UTC+10)",  "Australia/Sydney"},
        {"迪拜 (UTC+4)",   "Asia/Dubai"},
        {"新加坡 (UTC+8)", "Asia/Singapore"},
    };
}

QString TimeUtils::formatWithWeekday(const QDateTime &dt)
{
    QLocale loc = QLocale::system();
    QString dow = loc.dayName(dt.date().dayOfWeek(), QLocale::LongFormat);
    return QString("%1 %2")
        .arg(dt.toString("yyyy年M月d日"))
        .arg(dow);
}

QDateTime TimeUtils::toTimezone(const QDateTime &utc, const QTimeZone &tz)
{
    return utc.toTimeZone(tz);
}
