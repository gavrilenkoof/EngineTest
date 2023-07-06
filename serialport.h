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

    QString errorString(){
        return m_pserial->errorString();
    }

    QList<QSerialPortInfo> getSerialPortInfo(void);

    bool openSerialPort(QString port_name);
    bool closeSerialPort();

private:
    QSerialPort *m_pserial = nullptr;

    QTimer m_timer_upd_serials;

private slots:
    void timerSlotUpdCountSerials();
    void handleError(QSerialPort::SerialPortError error);
    void readData();

signals:
    void updateSerialList();
    void showStatusMessage(QString const message);

    void resourceError();

};

#endif // SERIALPORT_H
