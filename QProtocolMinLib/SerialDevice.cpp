#include "SerialDevice.h"
#include <QSerialPort>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include "ContextWrapper.h"

namespace min {

SerialDevice::SerialDevice(QObject *parent)
    : QObject{parent} {
    mContextWrapper = new ContextWrapper{this};
    mSerialPort = new QSerialPort{this};
    connect(mSerialPort, &QSerialPort::readyRead,
            this, &SerialDevice::onSerialPortReadyRead);
    connect(mSerialPort, &QSerialPort::errorOccurred,
            this, &SerialDevice::onSerialPortError);
    connect(mContextWrapper, &ContextWrapper::frameSended,
            this, &SerialDevice::onFrameReadyWrite);
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
    qDebug() << __FUNCTION__ << id << payload;
    mContextWrapper->send(id, payload);
}

bool SerialDevice::waitFrame(quint8& idOut, QByteArray& payloadOut, quint32 timeoutMs) {
    auto loop = QEventLoop{};
    auto timer = QTimer{};
    timer.setInterval(timeoutMs);
    connect(&timer, &QTimer::timeout,
            &loop, &QEventLoop::quit);
    connect(mContextWrapper, &ContextWrapper::frameReceived, &loop,
            [&idOut, &payloadOut, &loop](quint8 id, const QByteArray& bytes){
        idOut = id;
        payloadOut = bytes;
        loop.exit(1);
    });
    timer.start();
    return loop.exec() == 1;
}

void SerialDevice::onSerialPortError() {
    emit error(mSerialPort->errorString());
}

void SerialDevice::onSerialPortReadyRead() {
    auto bytes = mSerialPort->readAll();
    qDebug() << __FUNCTION__ << bytes;
    mContextWrapper->poll(bytes);
}

void SerialDevice::onFrameReadyWrite(const QByteArray& frameBytes) {
    qDebug() << __FUNCTION__ << frameBytes;
    mSerialPort->write(frameBytes);
}

} // namespace min
