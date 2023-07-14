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
            return false;
        }
    }else{
//        emit showStatusMessage("Critical error: port already opened!");
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

void SerialPort::readData()
{
//    qDebug() << m_pserial->bytesAvailable();

    if(m_pserial->bytesAvailable() < (34 * 100)){
        return;
    }


    m_data_bytes += m_pserial->readAll();
//    qDebug() << m_data_bytes;


    while(m_data_bytes.size() > 0){
        m_data_begin = m_data_bytes.indexOf("T:");
        m_data_end = m_data_bytes.indexOf("endl");

        if(m_data_begin < m_data_end /*&& m_data_begin != -1*/){
            m_temp_data = m_data_bytes.mid(m_data_begin, m_data_end - m_data_begin + 4);
            m_data_bytes.remove(m_data_begin, m_data_end - m_data_begin + 4);
//            qDebug() << "1:"<< m_temp_data;

            if(m_temp_data.size() == 34){
//                double torque = m_temp_data
                uint8_t arr[8] = {0};
                arr[0] = m_temp_data.at(2);
                arr[1] = m_temp_data.at(3);
                arr[2] = m_temp_data.at(4);
                arr[3] = m_temp_data.at(5);

//                uint32_t torque = (arr[3] & 0x00FF) << 24 | (arr[2] & 0x00FF) << 16 | (arr[1] & 0x00FF) << 8 | (arr[0] & 0x00FF) << 0;
                uint32_t torque = 0;
                memmove(&torque, arr, 4);

                double torque_double = (((float)torque / 8388608.f) - 1.0f)*(5.0f / (float)(1 << 1));

//                qDebug() << tr("%1").arg(QString::number(torque_double, 'f', 7));
//                qDebug() << m_temp_data.mid(17, 8);
                uint64_t time = 0;

                arr[0] = m_temp_data.at(17);
                arr[1] = m_temp_data.at(18);
                arr[2] = m_temp_data.at(19);
                arr[3] = m_temp_data.at(20);
                arr[4] = m_temp_data.at(21);
                arr[5] = m_temp_data.at(22);
                arr[6] = m_temp_data.at(23);
                arr[7] = m_temp_data.at(24);

                memmove(&time, arr, 8);

                qDebug() << time;

//                qDebug() << m_temp_data.at()
            }else{
                qDebug() << "ERROR";
            }


        }else if(m_data_end == -1){
            m_next_data = m_data_bytes.mid(0, m_data_bytes.size());
            m_data_bytes.remove(0, m_data_bytes.size());
//            qDebug() << "2:"<< m_next_data;
        }else if(m_data_begin > m_data_end){
            m_prev_data = m_data_bytes.mid(0, m_data_end + 4);
            m_data_bytes.remove(0, m_data_end + 4);
//            qDebug() << "3:"<< m_prev_data;
        }else{
            qDebug() << "ERROR";
        }

//        m_temp_data.clear();
    }


//    m_data_begin = m_data_bytes.indexOf("T:");
//    m_data_end = m_data_bytes.indexOf("end");
//    qDebug() << m_data_begin << " " << m_data_end;


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
