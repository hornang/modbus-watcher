#include <QDebug>

#include "modbusparser.h"

static const quint16 crcTable[] =
{
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

ModbusParser::ModbusParser()
{
    QString portName = "COM1";

    m_serialPort.setBaudRate(9600);
    m_serialPort.setPortName(portName);
    connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));

    if (!m_serialPort.open(QIODevice::ReadWrite))
    {
        qCritical() << "Unable to open serial port:" << portName;
    }
}

void ModbusParser::readData()
{
    QByteArray newData = m_serialPort.readAll();

    m_buffer.append(newData);

    if (m_buffer.size() > 255)
    {
        qWarning() << "Discarding too long packet";
    }

    int start;
    int length;

    forever
    {
        if (findPacket(m_buffer, start, length))
        {           
            analyzePacket((quint8 *) (m_buffer.data()) + start, length);

            QDebug debug = qDebug();
            debug.noquote();

            m_buffer = m_buffer.right(m_buffer.size() - (start + length));

            if (start > 0)
            {
                qDebug() << "Skipping: " << start << " bytes";
            }
        }
        else
        {
            break;
        }
    }
}

bool ModbusParser::findPacket(QByteArray data, int & start, int & length)
{
    for (int i = 0; i < data.size() - 2;  i++)
    {
        // Find packet by brute-forcing using CRC. This is the only certain
        // way to find the packet.
        for (int testLength = 3; testLength < data.size() - i - 2; testLength++)
        {
            quint8 * dataPtr = (quint8 *) (data.data()) + i;
            quint16 crc = Crc16(dataPtr, testLength);

            quint16 packetCrc = dataPtr[testLength] | dataPtr[testLength + 1] << 8;

            if (packetCrc == crc)
            {
                start = i;
                length = testLength + 2;
                return true;
            }
        }
    }

    return false;
}

void ModbusParser::analyzePacket(quint8 * frame, int length)
{
    if (length < 5)
    {
        qWarning() << "Packet too short:" << length << "bytes";
        return;
    }

    quint16 crc = Crc16(frame, length - 2);
    quint16 packetCrc = frame[length - 2] | frame[length - 1] << 8;

    if (crc != packetCrc)
    {
        qDebug() << "Packet has invalid CRC";
        return;
    }

    qDebug() << "Slave addr: " << frame[0];
    qDebug() << "Function: " << frame[1];

    int function = frame[1];
    QString functionName;

    switch (function & 127)
    {
    case 1:
        functionName = "Read Coil Status";
        break;

    case 2:
        functionName = "Read Input Status";
        break;

    case 3:
        functionName = "Read Holding Registers";
        break;

    case 4:
        functionName = "Read Input Registers";
        break;

    case 5:
        functionName = "Force Single Coil";
        break;

    case 6:
        functionName = "Preset Single Register";
        break;

    case 8:
        functionName = "Diagnostics";
        break;

    case 15:
        functionName = "Force Multiple Coils";
        break;

    case 16:
        functionName = "Preset Multiple Registers";
        break;

    default:
        functionName = "Unknown function";
        break;

    }

    QDebug debug = qDebug();
    debug.noquote();

    bool exception = function & 128;

    if (exception)
    {
        debug << "Execption reply for " + functionName;
    }
    else
    {
        if (length < 8)
        {
            qWarning() << "Packet too short:" << length << "bytes";
            return;
        }

        int address = (frame[2] << 8 | frame[3]);
        int points = (frame[4] << 8 | frame[5]);

        if (function == 3 || function == 4)
        {
            if (length > 8)
            {
                debug << functionName + "\nReply\nData:\n";

                for (int i = 0; i < frame[2] / 2; i++)
                {
                    quint16 value = ((frame[3 + i * 2] << 8) | frame[3 + i * 2 + 1]);
                    debug << "" << (m_lastAddress + i) << ":" << (quint16) value << "(" << (qint16) value << ")\n";
                }
            }
            else
            {
                debug << functionName + "\n";
                debug << "  Start:" << address << "\n  Length:" << points;
                m_lastAddress = address;
            }
        }
        else if (function == 8)
        {
            debug << functionName + "\n";
            quint16 subfunction = ((frame[2] << 8) | frame[3]);
            debug << "  Subfunction:" << subfunction << "\n. Length:" << length;

        }
        else if (function == 16)
        {
            if (length > 8)
            {
                debug << functionName + "\n";
                debug << "  Start:" << address << "\n  Length:" << points << "\nData:\n";

                for (int i = 0; i < frame[6] / 2; i++)
                {
                    quint16 value = ((frame[7 + i * 2] << 8) | frame[7 + i * 2 + 1]);
                    debug << "" << (address + i) << ":" << (quint16) value << "(" << (qint16) value << ")\n";
                }
            }
            else
            {
                debug << functionName + " Reply OK\n";
            }
        }
        else
        {
            debug << functionName;
        }
    }
}

quint16 ModbusParser::Crc16Continue(quint16 crc, quint8 * data, int length)
{
    quint8 temp;

    for (int i = 0; i < length; i++)
    {
        temp = data[i] ^ crc;
        crc >>= 8;
        crc ^= crcTable[temp];
    }
    return crc;
}

quint16 ModbusParser::Crc16(quint8 * data, int length)
{
    return Crc16Continue(0xFFFF, data, length);
}
