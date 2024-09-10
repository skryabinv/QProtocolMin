#include <QCoreApplication>
#include "SerialDevice.h"

int main(int argc, char *argv[])
{
    min::SerialDevice device;
    QCoreApplication a(argc, argv);

    return a.exec();
}
