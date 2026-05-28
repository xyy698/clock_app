#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <QString>
#include <QDateTime>
#include <QTimeZone>

// Utility functions for timezone and date operations.
namespace TimeUtils {

// Get a human-readable list of common timezone IDs.
QList<QPair<QString, QByteArray>> commonTimezones();

// Format a QDateTime with weekday in Chinese locale style.
QString formatWithWeekday(const QDateTime &dt);

// Convert UTC datetime to a target timezone.
QDateTime toTimezone(const QDateTime &utc, const QTimeZone &tz);

} // namespace TimeUtils

#endif // TIME_UTILS_H
