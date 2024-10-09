#pragma once

#include <QObject>
#include <QPointer>

class QIODevice;

namespace min {

class ContextWrapper;

class SerialDevice : public QObject {
    Q_OBJECT
public:
    explicit SerialDevice(QObject *parent = nullptr);
    bool openSerialPort(const QString& name, quint32 baudRate = 115200);
    void sendFrame(quint8 id, const QByteArray& payload);
    bool waitForFrame(quint8& idOut, QByteArray& payloadOut, quint32 timeoutMs);
    bool isValid() const;
signals:
    void error(const QString& message);
    void frameReceived(quint8 id, const QByteArray& payload);
    void frameSended(quint8 id, const QByteArray& payload);
private:
    void onIODeviceError();
    void onIODeviceReadyRead();
    void onFrameReadyWrite(const QByteArray& frameBytes);
    QPointer<QIODevice> mIODevice{};
    ContextWrapper* mContextWrapper{};
};

} // namespace min

