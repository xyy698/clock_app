#include "ntp_client.h"
#include <QNetworkDatagram>
#include <QtEndian>

// NTP epoch: Jan 1, 1900. Unix epoch: Jan 1, 1970.
// Difference: 2208988800 seconds.
static const qint64 NTP_TO_UNIX_EPOCH = 2208988800LL;

NtpClient::NtpClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QUdpSocket(this))
{
    connect(m_socket, &QUdpSocket::readyRead, this, &NtpClient::onReadyRead);
}

void NtpClient::requestTime(const QString &host, quint16 port)
{
    m_socket->disconnectFromHost();

    // Build NTP request packet (48 bytes, first byte = 0x1B for client mode 3)
    QByteArray packet(48, '\0');
    packet[0] = 0x1B; // LI=0, VN=3, Mode=3 (client)

    m_socket->connectToHost(host, port);
    m_socket->write(packet);
    m_socket->waitForConnected(3000);
}

void NtpClient::onReadyRead()
{
    while (m_socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        QByteArray data = datagram.data();

        if (data.size() < 48) continue;

        // Extract transmit timestamp (bytes 40-47): 32-bit integer + 32-bit fraction
        quint32 seconds;
        quint32 fraction;
        // Use memcpy to avoid alignment issues
        const uchar *ptr = reinterpret_cast<const uchar*>(data.constData());

        seconds  = qFromBigEndian<quint32>(ptr + 40);
        fraction = qFromBigEndian<quint32>(ptr + 44);

        // Convert NTP seconds to Unix epoch milliseconds
        qint64 ntpSec = static_cast<qint64>(seconds) - NTP_TO_UNIX_EPOCH;
        qint64 ntpMs = ntpSec * 1000 + (static_cast<qint64>(fraction) * 1000) / 0x100000000ULL;

        m_lastTime = QDateTime::fromMSecsSinceEpoch(ntpMs, Qt::UTC);
        m_valid = true;

        qint64 localMs = QDateTime::currentMSecsSinceEpoch();
        qint64 offset = ntpMs - localMs;

        emit timeReceived(m_lastTime, offset);
        m_socket->disconnectFromHost();
    }
}
