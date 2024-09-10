#pragma once

#include <QObject>

class QSerialPort;

namespace min {

class ContextWrapper;

class SerialDevice : public QObject {
    Q_OBJECT
public:
    explicit SerialDevice(QObject *parent = nullptr);
    bool open(const QString& name, quint32 baudRate);
    void sendFrame(quint8 id, const QByteArray& payload);
signals:
    void frameReceived(quint8 id, const QByteArray& payload);
private:
    void onBytesAvailable();
    void onFrameReadyToSend(const QByteArray& bytes);
    QSerialPort* mSerialPort{};
    ContextWrapper* mContextWrapper{};
};

} // namespace min

