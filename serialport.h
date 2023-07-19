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

    QByteArray m_data_bytes;
    QByteArray m_temp_data;
    QByteArray m_prev_data;
    QByteArray m_next_data;
    uint8_t m_temp_arr[8] = {0};

    qsizetype m_data_begin;
    qsizetype m_data_end;

    void writeData(QByteArray const &data);
    void parseData(QByteArray &data, uint8_t values[], int data_begin, qsizetype bytes, qsizetype bias);

    QVector<QMap<QString, uint64_t>> m_dict_values;


private slots:
    void timerSlotUpdCountSerials();
    void readData();


signals:
    void updateSerialList();
    void showStatusMessage(QString const message);

    void newDataAvailable(QVector<QMap<QString, uint64_t>> data);
    void errorSerial(QSerialPort::SerialPortError error);
};

#endif // SERIALPORT_H
