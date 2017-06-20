#include <QCoreApplication>
#include "modbusparser.h"

int main(int argc, char *argv[])
{
    ModbusParser parser;
    QCoreApplication app(argc, argv);
    return app.exec();
}
