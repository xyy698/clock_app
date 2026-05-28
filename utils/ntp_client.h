#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QDateTime>

// NtpClient — fetches current time from NTP servers (RFC 5905).
// Returns UTC time; callers convert to local timezone as needed.
class NtpClient : public QObject
{
    Q_OBJECT

public:
    explicit NtpClient(QObject *parent = nullptr);

    // Send a request to the default NTP pool.
    void requestTime(const QString &host = "pool.ntp.org", quint16 port = 123);

    // Last successfully received NTP time (UTC).
    QDateTime lastNtpTime() const { return m_lastTime; }
    bool isValid() const { return m_valid; }

signals:
    void timeReceived(const QDateTime &utcTime, qint64 offsetMs);

private slots:
    void onReadyRead();

private:
    QUdpSocket *m_socket;
    QDateTime m_lastTime;
    bool m_valid = false;
};

#endif // NTP_CLIENT_H
