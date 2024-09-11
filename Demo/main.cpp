#include <QCoreApplication>
#include "SerialDevice.h"
#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    min::SerialDevice device;
    qDebug() << "Open device" << device.openSerialPort("COM25");
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &timer, [&device]{
        device.sendFrame(11, QByteArray("Hello World"));
    });
    timer.setInterval(2000);
    QTimer::singleShot(0, &timer, QOverload<>::of(&QTimer::start));
    QObject::connect(&device, &min::SerialDevice::frameReceived, &a,
                     [](quint8 id, const QByteArray& payload){
        qDebug() << id << payload;
    });
    QObject::connect(&device, &min::SerialDevice::error, &a,
                     [](const QString& message){
        qDebug() << message;
    });

    QTimer::singleShot(600000, &a, &QCoreApplication::quit);
    return a.exec();
}
