
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      m_settings(new SettingsDialog(this))
{
    ui->setupUi(this);

    /*
     * UI configuration
     */
    setWindowTitle("Engine test");
    ui->lbl_status->setText("Status: not connected");

    ui->combo_box_serials_baud->addItem(QStringLiteral("1000000"), 1000000);
    ui->combo_box_serials_baud->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->combo_box_serials_baud->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->combo_box_serials_baud->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->combo_box_serials_baud->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    ui->combo_box_serials_baud->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    // btn configuration
    ui->btn_connection->setText("&Connect");
    ui->btn_connection->setCheckable(true);

    connect(ui->btn_connection, SIGNAL(toggled(bool)), this, SLOT(btnConnectionSerial(bool)));

    // table
    ui->lbl_val_1->setText("RPM: ");
    ui->lbl_val_2->setText("Torque (N*m): ");
    ui->lbl_val_3->setText("");
    ui->lbl_val_4->setText("");

    /*
     * Serial
     */
    m_pserial = new SerialPort(this);

    /*
     * Signals
     */
    connect(m_pserial, SIGNAL(updateSerialList()), this, SLOT(serialListHandler()));
    connect(m_pserial, SIGNAL(showStatusMessage(QString)), this, SLOT(consoleInfo(QString)));
    connect(m_pserial, SIGNAL(errorSerial(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
    connect(m_pserial, SIGNAL(newDataAvailable(QVector<QMap<QString, uint64_t>>)), ui->realTimeGraphs, SLOT(newDataHandler(QVector<QMap<QString, uint64_t>>)));
    connect(m_pserial, SIGNAL(dataParamsAvailable(QString)), m_settings, SLOT(getParamsHandler(QString)));
    connect(m_pserial, SIGNAL(dataUpdateParamChecker(QString)), m_settings, SLOT(updateParamChecker(QString)));

    connect(ui->realTimeGraphs, SIGNAL(newDataTable(QVector<double>)), this, SLOT(newDataTableHandler(QVector<double>)));

    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);

    connect(m_settings, SIGNAL(getParams()), m_pserial, SLOT(getParamRequest()));
    connect(m_settings, SIGNAL(setParams(SettingsDialog::Parameters)), m_pserial, SLOT(setParamRequest(SettingsDialog::Parameters)));
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete ui;
}

void MainWindow::newDataTableHandler(QVector<double> data)
{
    ui->lbl_val_1->setText(tr("RPM: %1").arg(QString::number(data.at(0), 'f', 3)));
    ui->lbl_val_2->setText(tr("Torque (N*m): %1").arg(QString::number(data.at(1), 'f', 3)));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    m_pserial->closeSerialPort();
}


void MainWindow::serialListHandler()
{
    ui->combo_box_serials->clear();

    auto const info_serials = m_pserial->getSerialPortInfo();

    for(QSerialPortInfo const &info: info_serials){
        ui->combo_box_serials->addItem(info.portName());
    }
}

void MainWindow::btnConnectionSerial(bool state)
{
    if(state){
//        qDebug() << state << "Open serial port" << ui->combo_box_serials->currentText();
        if(m_pserial->openSerialPort(ui->combo_box_serials->currentText(), ui->combo_box_serials_baud->currentText().toInt())){

            ui->lbl_status->setText("Status: connected");
            ui->btn_connection->setText("&Disconnect");
        }else{
            ui->lbl_status->setText("Status: connection failure");
            ui->btn_connection->setChecked(false);
        }
    }else{
//        qDebug() << state << "Close serial port";
        if(m_pserial->closeSerialPort()){
            ui->btn_connection->setText("&Connect");
            ui->lbl_status->setText("Status: disconnected");
        }else{
            ui->btn_connection->setChecked(true);
        }
    }
}

void MainWindow::consoleInfo(QString const message)
{
    ui->console->append(message);
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
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
        consoleInfo(tr("Permission Error: %1").arg(m_pserial->errorString()));
        QMessageBox::critical(this, tr("Permission Error"), m_pserial->errorString());
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
        m_pserial->closeSerialPort();
        ui->btn_connection->setChecked(false);
        consoleInfo(tr("Resource Error: %1").arg(m_pserial->errorString()));
        QMessageBox::critical(this, tr("Resource Error"), m_pserial->errorString());
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


