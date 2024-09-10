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
    if(!mContextWrapper->isValid()) {
        return;
    }
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
    if(!mContextWrapper->isValid()) {
        qCritical() << "Can't open serial device without valid MIN context";
        return false;
    }
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
    if(!mContextWrapper->isValid()) {
        qCritical() << "Can't send frame over serial device without valid MIN context";
        return;
    }
    mContextWrapper->send(id, payload);
}

bool SerialDevice::waitFrame(quint8& idOut, QByteArray& payloadOut, quint32 timeoutMs) {
    if(!mContextWrapper->isValid()) {
        return false;
    }
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
    mContextWrapper->poll(bytes);
}

void SerialDevice::onFrameReadyWrite(const QByteArray& frameBytes) {    
    mSerialPort->write(frameBytes);
}

} // namespace min
