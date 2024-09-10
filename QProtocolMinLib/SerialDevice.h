#pragma once

#include <QObject>

class QSerialPort;

namespace min {

class ContextWrapper;

class SerialDevice : public QObject {
    Q_OBJECT
public:
    explicit SerialDevice(QObject *parent = nullptr);
    bool open(const QString& name, quint32 baudRate = 115200);
    void sendFrame(quint8 id, const QByteArray& payload);
    bool waitFrame(quint8& id, QByteArray& payload, quint32 timeoutMs);
signals:
    void frameReceived(quint8 id, const QByteArray& payload);
private:
    void onSerialPortReadyRead();
    void onFrameReadyWrite(const QByteArray& frameBytes);
    QSerialPort* mSerialPort{};
    ContextWrapper* mContextWrapper{};
};

} // namespace min

