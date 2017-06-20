#ifndef MODBUSPARSER_H
#define MODBUSPARSER_H

#include <QObject>
#include <QSerialPort>

class ModbusParser : public QObject
{
    Q_OBJECT

public:
    ModbusParser();

public slots:
    void readData();

private:
    bool findPacket(QByteArray packet, int & start, int & length);
    quint16 Crc16Continue(quint16 crc, quint8 * data, int length);
    quint16 Crc16(quint8 * data, int length);
    void analyzePacket(quint8 * data, int length);

    QSerialPort m_serialPort;
    QByteArray m_buffer;
    quint16 m_lastAddress;
};

#endif // MODBUSPARSER_H
