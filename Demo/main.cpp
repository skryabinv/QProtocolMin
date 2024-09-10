#include <QCoreApplication>
#include "ContextWrapper.h"

int main(int argc, char *argv[])
{
    min::ContextWrapper device;
    QCoreApplication a(argc, argv);

    return a.exec();
}
