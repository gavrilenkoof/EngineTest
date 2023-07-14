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

    m_data_bytes += m_pserial->readAll();
//    m_data_bytes += m_pserial->read(32);

//    if(m_pserial->canReadLine()){
//        QString data = QString(m_pserial->readLine());
//        if(data.contains("T:") && data.contains("R:") && data.contains("Tm:")){
//            emit newDataAvailable(data);
//        }else if(data.contains("Par:") && data.contains("Gain:") && data.contains("Scale:")
//                 && data.contains("BiasX:") && data.contains("BiasY:") && data.contains("Baudrate:")){
//            emit dataParamsAvailable(data);
//        }else if(data.contains("Set") && data.contains("->")){
//            emit dataUpdateParamChecker(data);
//        }else{
//            qDebug() << "Unknown data! Skip" << data;
//        }
//    }
    m_data_begin = m_data_bytes.indexOf("T:");
    m_data_end = m_data_bytes.indexOf("endl");

    int temp = 50;

    while(m_data_bytes.size() > 0){


//        if(m_data_begin != -1 && m_data_end < m_data_begin){
//            m_prev_data = m_data_bytes.mid(0, m_data_begin + 4);
//            m_data_bytes.remove(0, m_data_begin + 4);
//        }else if(m_data_end == -1){
//            break;
//            qDebug() << "BREAK";
//        }else{
//            m_temp_data = m_data_bytes.mid(m_data_begin , m_data_end - m_data_begin + 4);
//            m_data_bytes.remove(m_data_begin, m_data_end - m_data_begin + 4);
//        }

        if(m_data_begin < m_data_end && m_data_begin != -1){
            m_temp_data = m_data_bytes.mid(m_data_begin, m_data_end - m_data_begin + 4);
            m_data_bytes.remove(m_data_begin, m_data_end - m_data_begin + 4);
        }else if(m_data_end == -1){
            m_next_data = m_data_bytes.mid(0, m_data_bytes.size());
            m_data_bytes.remove(0, m_data_bytes.size());
        }else if(m_data_begin > m_data_end){
            m_prev_data = m_data_bytes.mid(0, m_data_end + 4);
            m_data_bytes.remove(0, m_data_end + 4);
        }else{
            qDebug() << "UNKNOWN";
        }


        qDebug() << m_temp_data;


        m_data_begin = m_data_bytes.indexOf("T:");
        m_data_end = m_data_bytes.indexOf("endl");

    }

//    m_data_begin = m_data_bytes.indexOf("T:");
//    m_data_end = m_data_bytes.indexOf("end");

//    qDebug() << m_data_begin << " " << m_data_end;


    m_data_bytes.clear();

    qDebug() << m_pserial->bytesAvailable();


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
    qDebug() << data;

    data = tr("SCALE=%1\n").arg(QString::number(params.scale, 'g'));
    writeData(data.toUtf8());
    qDebug() << data;

    data = tr("BIASX=%1\n").arg(QString::number(params.bias_x, 'g'));
    writeData(data.toUtf8());
    qDebug() << data;

    data = tr("BIASY=%1\n").arg(QString::number(params.bias_y, 'g'));
    writeData(data.toUtf8());
    qDebug() << data;

    data = tr("Baudrate=%1\n").arg(QString::number(params.baudrate));
    writeData(data.toUtf8());
    qDebug() << data;

}
