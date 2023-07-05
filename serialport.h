#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include <QSerialPortInfo>
#include <QSerialPort>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    QList<QSerialPortInfo> getSerialPortInfo(void);

private:
    QTimer m_timer_upd_serials;

private slots:
    void timerSlotUpdCountSerials();

signals:
    void updateSerialList();
};

#endif // SERIALPORT_H
