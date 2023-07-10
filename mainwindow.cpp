
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
    connect(m_pserial, SIGNAL(resourceError()), this, SLOT(resourceErrorHandler()));
    connect(m_pserial, SIGNAL(newDataAvailable(QString)), ui->realTimeGraphs, SLOT(newDataHandler(QString)));
    connect(m_pserial, SIGNAL(dataParamsAvailable(QString)), m_settings, SLOT(getParamsHandler(QString)));

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
        if(m_pserial->openSerialPort(ui->combo_box_serials->currentText())){
            ui->btn_connection->setText("&Disconnect");
        }else{
            ui->btn_connection->setChecked(false);
        }
    }else{
//        qDebug() << state << "Close serial port";
        if(m_pserial->closeSerialPort()){
            ui->btn_connection->setText("&Connect");
        }else{
            ui->btn_connection->setChecked(true);
        }
    }
}

void MainWindow::consoleInfo(QString const message)
{
    ui->console->append(message);
}

void MainWindow::resourceErrorHandler()
{
    ui->btn_connection->setChecked(false);

    QMessageBox::critical(this, tr("Critical Error"), m_pserial->errorString());

}


