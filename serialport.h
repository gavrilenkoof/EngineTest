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

    bool isOpen() const{
        return m_pserial->isOpen();
    }

    QList<QSerialPortInfo> getSerialPortInfo(void);

    void openSerialPort(QString port_name);
    void closeSerialPort();

private:
    QSerialPort *m_pserial = nullptr;

    QTimer m_timer_upd_serials;

private slots:
    void timerSlotUpdCountSerials();

signals:
    void updateSerialList();
    void showStatusMessage(QString const message);
};

#endif // SERIALPORT_H