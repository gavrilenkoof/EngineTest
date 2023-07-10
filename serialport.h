#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QTimer>
#include <QDebug>

#include <QSerialPortInfo>
#include <QSerialPort>

#include "settingsdialog.h"

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

    bool openSerialPort(QString port_name, int baudrate);
    bool closeSerialPort();

private:
    QSerialPort *m_pserial = nullptr;

    QTimer m_timer_upd_serials;

    void writeData(QByteArray const &data);


private slots:
    void timerSlotUpdCountSerials();
    void readData();

    void getParamRequest();
    void setParamRequest(SettingsDialog::Parameters params);

signals:
    void updateSerialList();
    void showStatusMessage(QString const message);

    void newDataAvailable(QString data);
    void dataParamsAvailable(QString data);
    void dataUpdateParamChecker(QString data);
    void errorSerial(QSerialPort::SerialPortError error);


};

#endif // SERIALPORT_H
