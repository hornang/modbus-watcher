#include <QCoreApplication>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "modbusparser.h"
#include "debuglogger.h"

DebugLogger * debuglogger = 0;

#ifdef BUILD_DEVICE
#include "logger.h"
Logger logger;
#endif

void MessageOutput(QtMsgType type, const QMessageLogContext& context, const QString &message)
{
    Q_UNUSED(type);
    Q_UNUSED(context);

    QString msg = message;
    msg += QString("\n");

    OutputDebugString(reinterpret_cast<const wchar_t *>(msg.utf16()));

    debuglogger->write(message);
}

int main(int argc, char *argv[])
{
    debuglogger = DebugLogger_Instance();

    debuglogger->setFile("parsing-log.txt");

    qInstallMessageHandler(MessageOutput);

    ModbusParser parser;
    QCoreApplication app(argc, argv);
    return app.exec();
}
