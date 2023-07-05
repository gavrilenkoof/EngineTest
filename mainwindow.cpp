
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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

    /*
     * Serial
     */
    m_pserial = new SerialPort(this);

    connect(m_pserial, SIGNAL(updateSerialList()), this, SLOT(serialListHandler()));

}

MainWindow::~MainWindow()
{
    delete ui;
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
    qDebug() << state;
}


