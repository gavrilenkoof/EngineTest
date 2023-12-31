#include "serialport.h"

SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
{
    m_pserial = new QSerialPort;


    connect(&m_timer_upd_serials, SIGNAL(timeout()), this, SLOT(timerSlotUpdCountSerials()));
    m_timer_upd_serials.start(500);

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
        m_pserial->setReadBufferSize((100 * 1024));
        if(m_pserial->open(QIODevice::ReadWrite)){
            qDebug() << "Connected to " << m_pserial->portName() << ":" << m_pserial->baudRate()
                     << "," << m_pserial->dataBits() << "," << m_pserial->parity() << "," << m_pserial->stopBits()
                     << "," <<  m_pserial->flowControl();
            emit showStatusMessage(tr("Connected to %1").arg(m_pserial->portName()));
            m_pserial->clear();
            m_pserial->setDataTerminalReady(true);
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


bool SerialPort::isMsgCorrect(QByteArray const &data, qsizetype &data_begin)
{
    static qsizetype check_err_msg = 0;
    static int count = 0;

    if(data.size() != m_data_bytes_size){
        return false;
    }

    data_begin = data.indexOf("T:");
    check_err_msg = data.indexOf("T:", data_begin + 1);
    if(data_begin == -1 || check_err_msg != -1){
        qDebug() << "Error parse: bad message, could not found 'T:' or error";
        return false;
    }

    data_begin = data.indexOf(";R:");
    check_err_msg = data.indexOf(";R:", data_begin + 1);
    if(data_begin == -1 || check_err_msg != -1){
        qDebug() << "Error parse: bad message, could not found 'R:' or error";
        return false;
    }

    data_begin = data.indexOf(";Tm:");
    check_err_msg = data.indexOf(";Tm:", data_begin + 1);
    if(data_begin == -1 || check_err_msg != -1){
        qDebug() << "Error parse: bad message, could found 'Tm:' or error";
        return false;
    } 

    data_begin = data.indexOf("-");
    if(data_begin == -1){
        qDebug() << "Error parse: bad message, could found '-' or error";
        return false;
    }

    data_begin = 0;
    count = 0;
    while((data_begin = data.indexOf(":", data_begin + 1)) >= 0){
        ++count;
    }

//    if(count != 3){
//        return false;
//    }

    return true;
}

// GAIN = 7
// SCALE = 13421
// BiasX = 0.0
// BiasY = -0.0000100

void SerialPort::readData()
{
//    static int correct_data = 0;
//    static int incorrect_data = 0;
    static QString find_adc_start;

    if(m_pserial->bytesAvailable() < (m_data_bytes_size * 10)){
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

    while(m_data_bytes.size() >= m_data_bytes_size){
        m_data_begin = m_data_bytes.indexOf("T:");
        m_data_end = m_data_bytes.indexOf("endl");

        if(m_data_begin < m_data_end && m_data_begin != -1){
            m_temp_data = m_data_bytes.mid(m_data_begin, m_data_end - m_data_begin + 4); // 4 bytes = "endl"
            m_data_bytes.remove(m_data_begin, m_data_end - m_data_begin + 4);
        }else if(m_data_end == -1){
            m_start_batch_data = m_data_bytes.mid(0, m_data_bytes.size());
            m_data_bytes.remove(0, m_data_bytes.size());
        }else if(m_data_begin > m_data_end || m_data_begin == -1){
            m_end_batch_data = m_data_bytes.mid(0, m_data_end + 4);
            m_temp_data = m_start_batch_data + m_end_batch_data;
            m_data_bytes.remove(0, m_data_end + 4);
        }else{
            qDebug() << "Error: unknown parse.";
            m_data_bytes.remove(0, m_data_bytes_size);
        }

        if(isMsgCorrect(m_temp_data, m_data_begin)){
            handleMsg(m_temp_data, m_data_begin, m_temp_arr, value);
//            ++m_correct_data;
        }else{
//            ++m_incorrect_data;
        }


        m_temp_data.clear();

    }

    emit newDataAvailable(m_dict_values);


//    double percent = (static_cast<double>(m_incorrect_data))/(m_incorrect_data + m_correct_data);
//    qDebug() << tr("%1").arg(QString::number(percent, 'g')) << "correct_data: " << m_correct_data << "incorrect_data: " << m_incorrect_data;

    m_dict_values.clear();
    m_data_bytes.clear();

}

void SerialPort::writeData(QByteArray const &data)
{
    m_pserial->write(data);
}

void SerialPort::handleMsg(QByteArray &temp_data, qsizetype &data_begin, uint8_t temp_arr[], QMap<QString, uint64_t> &value)
{

    /*
     *  T:(4 bytes);R:(4 bytes);Tm:(8 bytes)-(4 bytes)endl;
     *  T - torque (N*m)
     *  R - RPM
     *  Tm: timestamp (us)
     *  - - sampletime (us)
     */

    static uint32_t torque_adc = 0;
    static uint32_t rpm = 0;
    static uint64_t timestamp = 0;
    static uint32_t sampletime = 0;
    static uint64_t error_timestamp = 4000000000000; // TODO timestamp error 4179340000000 (from logs)

    data_begin = temp_data.indexOf("T:");
    torque_adc = 0;
    parseData(temp_data, temp_arr, data_begin, 4, 2);
    memmove(&torque_adc, temp_arr, 4);
    value["Torque"] = torque_adc;

    data_begin = temp_data.indexOf(";R:");
    rpm = 0;
    parseData(temp_data, temp_arr, data_begin, 4, 3);
    memmove(&rpm, temp_arr, 4);
    value["RPM"] = rpm;

    data_begin = temp_data.indexOf(";Tm:");
    timestamp = 0;
    parseData(temp_data, temp_arr, data_begin, 8, 4);
    memmove(&timestamp, temp_arr, 8);
    value["Timestamp"] = timestamp;

    // TEST Code
    if(timestamp >= error_timestamp){
        return;
    }


    data_begin = temp_data.indexOf("-");
    sampletime = 0;
    parseData(temp_data, temp_arr, data_begin, 4, 1);
    memmove(&sampletime, temp_arr, 4);
    value["Sampletime"] = sampletime;

    m_dict_values.append(value);
}
