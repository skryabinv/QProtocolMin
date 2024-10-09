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
    connect(mContextWrapper, &ContextWrapper::frameSended,
            this, &SerialDevice::onFrameReadyWrite);
    connect(mContextWrapper, &ContextWrapper::frameReceived,
            this, &SerialDevice::frameReceived);
}

bool SerialDevice::openSerialPort(const QString& name, quint32 baudRate) {
    if(!mContextWrapper->isValid()) {
        qCritical() << "Trying to open serial port with invalid MIN context";
        return false;
    }
    delete mIODevice;
    auto serialPort = new QSerialPort{this};
    serialPort->setPortName(name);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    auto opened =  serialPort->open(QIODevice::ReadWrite);
    if(opened) {
        connect(serialPort, &QSerialPort::errorOccurred,
                this, &SerialDevice::onIODeviceError);
        connect(serialPort, &QSerialPort::readyRead,
                this, &SerialDevice::onIODeviceReadyRead);
        mIODevice = serialPort;
    }
    return opened;
}

void SerialDevice::sendFrame(quint8 id, const QByteArray& payload) {
    if(!isValid()) {
        qCritical() << "Trying to use invalid serial device";
        return;
    }
    mContextWrapper->send(id, payload);
    emit frameSended(id, payload);
}

bool SerialDevice::waitForFrame(quint8& idOut, QByteArray& payloadOut, quint32 timeoutMs) {
    if(!isValid()) {
        qCritical() << "Trying to use invalid serial device";
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

bool SerialDevice::isValid() const {
    return mContextWrapper->isValid() && mIODevice != nullptr;
}

void SerialDevice::onIODeviceError() {
    assert(mIODevice != nullptr);
    auto errorString = mIODevice->errorString();
    delete mIODevice;
    emit error(errorString);
}

void SerialDevice::onIODeviceReadyRead() {
    auto bytes = mIODevice->readAll();
    mContextWrapper->poll(bytes);
}

void SerialDevice::onFrameReadyWrite(const QByteArray& frameBytes) {
    assert(mIODevice != nullptr);
    mIODevice->write(frameBytes);
}

} // namespace min
