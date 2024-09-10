#include <QCoreApplication>
#include "SerialDevice.h"
#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    min::SerialDevice device;
    qDebug() << "Open device" << device.open("COM25");
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &timer, [&device]{
        device.sendFrame(123, QByteArray("Hello World"));
    });
    timer.setInterval(2000);
    QTimer::singleShot(0, &timer, QOverload<>::of(&QTimer::start));
    QTimer::singleShot(10000, &a, &QCoreApplication::quit);
    return a.exec();
}
