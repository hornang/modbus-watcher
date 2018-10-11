#include <QDateTime>
#include <QDebug>
#include "debuglogger.h"

DebugLogger * debugLogger = 0;

DebugLogger * DebugLogger_Instance()
{
    if (debugLogger == 0)
    {
        debugLogger = new DebugLogger();
    }

    return debugLogger;
}

DebugLogger::DebugLogger(QObject *parent) : QObject(parent)
{
}

void DebugLogger::setFile(QString fileName)
{
    qDebug() << "Set debug log file to:" << fileName;
    m_fileName = fileName;
    m_file.setFileName(m_fileName);
}

QString DebugLogger::fileName()
{
    return m_fileName;
}

QByteArray DebugLogger::readAll()
{
    QFile file;
    file.setFileName(m_fileName);

    if (file.exists() && file.open(QIODevice::ReadOnly))
    {
        return file.readAll();
        file.close();
    }

    return QByteArray();
}

QString DebugLogger::formatTime()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    return dateTime.toString(Qt::ISODate);
}

void DebugLogger::clear()
{
    if (m_file.open(QIODevice::WriteOnly))
    {
        m_file.write("");
        m_file.close();
    }
}

void DebugLogger::write(QString message)
{
    if (m_file.fileName().isEmpty())
    {
        return;
    }

    if (m_file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        message = formatTime() + ": " + message + "\r\n";
        bool result = m_file.write(message.toUtf8());
        m_file.close();
    }
}



