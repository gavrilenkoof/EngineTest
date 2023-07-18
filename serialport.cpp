#include "serialport.h"

SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
{
    m_pserial = new QSerialPort;


    connect(&m_timer_upd_serials, SIGNAL(timeout()), this, SLOT(timerSlotUpdCountSerials()));
    m_timer_upd_serials.start(1000);

//    connect(m_pserial, &QSerialPort::errorOccurred, this, &SerialPort::handleError);
    connect(m_pserial, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SIGNAL(errorSerial(QSerialPort::SerialPortError)));
    connect(m_pserial, &QSerialPort::readyRead, this, &SerialPort::readData);

}


SerialPort::~SerialPort()
{
    delete m_pserial;
}


QList<QSerialPortInfo> SerialPort::getSerialPortInfo(void)
{
    return QSerialPortInfo::availablePorts();
}


void SerialPort::timerSlotUpdCountSerials()
{
    static int count_serials = 0;

    int upd_count_serials = getSerialPortInfo().count();

    if(upd_count_serials != count_serials){
        count_serials = upd_count_serials;
        emit updateSerialList();
    }
}


bool SerialPort::openSerialPort(QString port_name, int baudrate)
{

    if(!m_pserial->isOpen()){
        m_pserial->setPortName(port_name);
        m_pserial->setBaudRate(baudrate);
        m_pserial->setDataBits(QSerialPort::DataBits::Data8);
        m_pserial->setParity(QSerialPort::Parity::NoParity);
        m_pserial->setStopBits(QSerialPort::StopBits::OneStop);
        m_pserial->setFlowControl(QSerialPort::FlowControl::NoFlowControl);
        if(m_pserial->open(QIODevice::ReadWrite)){
            qDebug() << "Connected to " << m_pserial->portName() << ":" << m_pserial->baudRate()
                     << "," << m_pserial->dataBits() << "," << m_pserial->parity() << "," << m_pserial->stopBits()
                     << "," <<  m_pserial->flowControl();
            emit showStatusMessage(tr("Connected to %1").arg(m_pserial->portName()));
            m_pserial->clear();
            return true;
        }else{
            emit showStatusMessage(tr("Cannot connect to %1").arg(m_pserial->portName()));
            m_pserial->clear();
            return false;
        }
    }else{
        qDebug() << "Critical error: port already opened";
        return false;
    }
}

bool SerialPort::closeSerialPort()
{

    if(m_pserial->isOpen()){
        m_pserial->close();
        emit showStatusMessage(tr("Close %1 port").arg(m_pserial->portName()));
    }else{
//        emit showStatusMessage("Critical error: port already closed or never been opened!");
        qDebug() << "Critical error: port already closed or never been opened or other error";
    }

    m_pserial->setPortName("");

    return true;
}

void SerialPort::parseData(QByteArray &data, uint8_t values[], int data_begin, qsizetype bytes, qsizetype bias)
{
    for(int i = 0; i < bytes; ++i){
        values[i] = data.at(data_begin + (bias++));
    }
}

// GAIN = 7
// SCALE = 13421
// BiasX = 0.0
// BiasY = -0.0000100

void SerialPort::readData()
{
    static int const data_bytes = 34;
    static QString find_adc_start;

    if(m_pserial->bytesAvailable() < (data_bytes * 100)){
        return;
    }

    QMap<QString, uint64_t> value;

    m_data_bytes += m_pserial->readAll();
    find_adc_start = QString(m_data_bytes);

    if(find_adc_start.contains("AD7190 begin:")){
        m_pserial->clear();
        m_data_bytes.clear();
        qDebug() << "RESTART ARDUINO. CLEAR BUFFER";
        return;
    }

    while(m_data_bytes.size() > 0){
        m_data_begin = m_data_bytes.indexOf("T:");
        m_data_end = m_data_bytes.indexOf("endl");

        if(m_data_begin < m_data_end){
            m_temp_data = m_data_bytes.mid(m_data_begin, m_data_end - m_data_begin + 4); // 4 bytes = "endl"
            m_data_bytes.remove(m_data_begin, m_data_end - m_data_begin + 4);

            if(m_temp_data.size() == data_bytes){
                /*
                 *  T:(4 bytes);R:(4 bytes);Tm:(8 bytes)-(4 bytes)endl;
                 *  T - torque (N*m)
                 *  R - RPM
                 *  Tm: timestamp (us)
                 *  - - sampletime (us)
                 */
                m_data_begin = m_temp_data.indexOf("T:");
                uint32_t torque_adc = 0;
                parseData(m_temp_data, m_temp_arr, m_data_begin, 4, 2);
                memmove(&torque_adc, m_temp_arr, 4);
                value["Torque"] = torque_adc;

                m_data_begin = m_temp_data.indexOf("R:");
                uint32_t rpm = 0;
                parseData(m_temp_data, m_temp_arr, m_data_begin, 4, 2);
                memmove(&rpm, m_temp_arr, 4);
                value["RPM"] = rpm;

                m_data_begin = m_temp_data.indexOf("Tm:");
                uint64_t timestamp = 0;
                parseData(m_temp_data, m_temp_arr, m_data_begin, 8, 3);
                memmove(&timestamp, m_temp_arr, 8);
                value["Timestamp"] = timestamp;

                m_data_begin = 26;
                uint32_t sampletime = 0;
                parseData(m_temp_data, m_temp_arr, m_data_begin, 4, 0);
                memmove(&sampletime, m_temp_arr, 4);
                value["Sampletime"] = sampletime;
//                qDebug() << value;
                m_dict_values.append(value);

            }else{
//                qDebug() << "ERROR: data size error.";
            }


        }else if(m_data_end == -1){
            m_next_data = m_data_bytes.mid(0, m_data_bytes.size());
            m_data_bytes.remove(0, m_data_bytes.size());
//             qDebug() << "SKIP" << m_next_data;
        }else if(m_data_begin > m_data_end){
            m_prev_data = m_data_bytes.mid(0, m_data_end + 4);
            m_data_bytes.remove(0, m_data_end + 4);
//            qDebug() << "SKIP" << m_prev_data;
        }else{
            qDebug() << "Error: unknown parse.";
        }

    }

    emit newDataAvailable(m_dict_values);

    m_dict_values.clear();
    m_data_bytes.clear();

}

void SerialPort::writeData(QByteArray const &data)
{
    m_pserial->write(data);
}


void SerialPort::getParamRequest()
{
    if(isOpen()){
        QByteArray msg_bytes = QString("GET_PARAMS").toUtf8();
        writeData(msg_bytes);
    }else{
        qDebug() << "Port is not opened";
    }

}

void SerialPort::setParamRequest(SettingsDialog::Parameters params)
{

    QString data;
    data = tr("GAIN=%1\n").arg(QString::number(params.gain, 'g'));
    writeData(data.toUtf8());
//    qDebug() << data;

    data = tr("SCALE=%1\n").arg(QString::number(params.scale, 'g'));
    writeData(data.toUtf8());
//    qDebug() << data;

    data = tr("BIASX=%1\n").arg(QString::number(params.bias_x, 'g'));
    writeData(data.toUtf8());
//    qDebug() << data;

    data = tr("BIASY=%1\n").arg(QString::number(params.bias_y, 'g'));
    writeData(data.toUtf8());
//    qDebug() << data;

    data = tr("Baudrate=%1\n").arg(QString::number(params.baudrate));
    writeData(data.toUtf8());
//    qDebug() << data;

}
