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

    bool openSerialPort(QString port_name);
    bool closeSerialPort();

private:
    QSerialPort *m_pserial = nullptr;

    QTimer m_timer_upd_serials;
    QTimer m_timer_timout_read;

    void writeData(QByteArray const &data);


private slots:
    void timerSlotUpdCountSerials();
    void handleError(QSerialPort::SerialPortError error);
    void readData();

    void getParamRequest();
    void setParamRequest(SettingsDialog::Parameters params);
    void timeoutRead();

signals:
    void updateSerialList();
    void showStatusMessage(QString const message);

    void resourceError();

    void newDataAvailable(QString data);
    void dataParamsAvailable(QString data);

};

#endif // SERIALPORT_H
