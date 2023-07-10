#include "serialport.h"

SerialPort::SerialPort(QObject *parent)
    : QObject{parent}
{
    m_pserial = new QSerialPort;


    connect(&m_timer_timout_read, SIGNAL(timeout()), this, SLOT(timeoutRead()));
    m_timer_timout_read.start(10);

    connect(&m_timer_upd_serials, SIGNAL(timeout()), this, SLOT(timerSlotUpdCountSerials()));
    m_timer_upd_serials.start(1000);

    connect(m_pserial, &QSerialPort::errorOccurred, this, &SerialPort::handleError);

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


bool SerialPort::openSerialPort(QString port_name)
{

    if(!m_pserial->isOpen()){
        m_pserial->setPortName(port_name);
        m_pserial->setBaudRate(9600);
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

void SerialPort::timeoutRead()
{

}

void SerialPort::readData()
{

    if(m_pserial->canReadLine()){
        QString data = QString(m_pserial->readLine());
        if(data.contains("T:") && data.contains("R:") && data.contains("Tm:")){
            emit newDataAvailable(data);
        }else if(data.contains("Par:") && data.contains("Gain:") && data.contains("Scale:")
                 && data.contains("BiasX:") && data.contains("BiasY:") && data.contains("Baudrate:")){
            emit dataParamsAvailable(data);
        }else{
            qDebug() << "Unknown data! Skip" << data;
        }

    }
}

void SerialPort::writeData(QByteArray const &data)
{
    m_pserial->write(data);
}

void SerialPort::handleError(QSerialPort::SerialPortError error)
{

    switch (error) {
    case QSerialPort::SerialPortError::DeviceNotFoundError:
        /*  An error occurred while attempting to open an non-existing device.
         */
        qDebug() << error;
        break;
    case QSerialPort::SerialPortError::PermissionError:
        /*  An error occurred while attempting to open an already opened device by
         *  another process or a user not having enough permission and credentials to open.
         */
        qDebug() << error;
        break;
    case QSerialPort::SerialPortError::OpenError:
        /*  An error occurred while attempting to open an already opened device in this object.
         */
        qDebug() << error;
        break;
    case QSerialPort::SerialPortError::WriteError:
        /*  An I/O error occurred while writing the data.
         */
        qDebug() << error;
        break;
    case QSerialPort::SerialPortError::ReadError:
        /*  An I/O error occurred while reading the data.
         */
        qDebug() << error;
        break;
    case QSerialPort::SerialPortError::ResourceError:
        /*
         * An I/O error occurred when a resource becomes unavailable,
         * e.g. when the device is unexpectedly removed from the system.
         */
        qDebug() << error;
        closeSerialPort();
        emit resourceError();
        break;
    case QSerialPort::SerialPortError::UnsupportedOperationError:
        /*  The requested device operation is not supported or prohibited by the running operating system.
         */
        qDebug() << error;
        break;
    case QSerialPort::SerialPortError::UnknownError:
        /*
         * An unidentified error occurred.
         */
        qDebug() << error;
        break;
    case QSerialPort::SerialPortError::TimeoutError:
        /*
         * A timeout error occurred. This value was introduced in QtSerialPort 5.2.
         */
        qDebug() << error;
        break;
    case QSerialPort::SerialPortError::NotOpenError:
        /*
         * This error occurs when an operation is executed that can only be successfully performed
         * if the device is open. This value was introduced in QtSerialPort 5.2.
         */
        qDebug() << error;
        break;
    default:
        break;
    }

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
    data = tr("GAIN=%1\n").arg(QString::number(params.gain, 'f'));
    writeData(data.toUtf8());

    data = tr("SCALE=%1\n").arg(QString::number(params.scale, 'f'));
    writeData(data.toUtf8());

    data = tr("BIASX=%1\n").arg(QString::number(params.bias_x, 'f'));
    writeData(data.toUtf8());

    data = tr("BIASY=%1\n").arg(QString::number(params.bias_y, 'f'));
    writeData(data.toUtf8());

    data = tr("Baudrate=%1\n").arg(QString::number(params.baudrate));
    writeData(data.toUtf8());

}
