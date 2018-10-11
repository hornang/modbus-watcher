#ifndef DEBUGLOGGER_H
#define DEBUGLOGGER_H

#include <QObject>
#include <QFile>

class DebugLogger : public QObject
{
    Q_OBJECT
public:
    explicit DebugLogger(QObject *parent = 0);
    void setFile(QString file);
    void write(QString message);
    QString fileName();
    QByteArray readAll();
    void clear();
    QString formatTime();

private:
    QFile m_file;
    QString m_fileName;
signals:

public slots:
};

DebugLogger * DebugLogger_Instance();

#endif // DEBUGLOGGER_H
