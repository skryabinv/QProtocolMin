#include "SerialDevice.h"
#include <QSerialPort>
#include "ContextWrapper.h"

namespace min {

SerialDevice::SerialDevice(QObject *parent)
    : QObject{parent} {
    mContextWrapper = new ContextWrapper{this};
    mSerialPort = new QSerialPort{this};
    connect(mSerialPort, &QSerialPort::readyRead,
            this, &SerialDevice::onBytesAvailable);
    connect(mContextWrapper, &ContextWrapper::frameSended,
            this, &SerialDevice::onFrameReadyToSend);
    connect(mContextWrapper, &ContextWrapper::frameReceived,
            this, &SerialDevice::frameReceived);
}

bool SerialDevice::open(const QString& name, quint32 baudRate) {
    if(mSerialPort->isOpen()) {
        mSerialPort->close();
    }
    mSerialPort->setPortName(name);
    mSerialPort->setBaudRate(baudRate);
    mSerialPort->setDataBits(QSerialPort::Data8);
    mSerialPort->setStopBits(QSerialPort::OneStop);
    return mSerialPort->open(QIODevice::ReadWrite);
}

void SerialDevice::sendFrame(quint8 id, const QByteArray& payload) {
    mContextWrapper->send(id, payload);
}

void SerialDevice::onBytesAvailable() {
    mContextWrapper->poll(mSerialPort->readAll());
}

void SerialDevice::onFrameReadyToSend(const QByteArray& bytes) {
    mSerialPort->write(bytes);
}

} // namespace min
