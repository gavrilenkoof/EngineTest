// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Parameters");

    ui->line_param6->setReadOnly(true);
    ui->line_param7->setReadOnly(true);
    ui->line_param8->setReadOnly(true);
    ui->line_param9->setReadOnly(true);
    ui->line_param10->setReadOnly(true);

    connect(ui->btnGetParam, SIGNAL(clicked()), this, SIGNAL(getParams()));
    connect(ui->btnSetParam, SIGNAL(clicked()), this, SLOT(setParamsPrepare()));


}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

SettingsDialog::Parameters SettingsDialog::parameters() const
{
    return m_params;
}

void SettingsDialog::fillInfo(SettingsDialog::Parameters &params)
{
    ui->line_param1->setText(tr("%1").arg(QString::number(params.gain, 'g')));
    ui->line_param2->setText(tr("%1").arg(QString::number(params.scale, 'g')));
    ui->line_param3->setText(tr("%1").arg(QString::number(params.bias_x, 'g')));
    ui->line_param4->setText(tr("%1").arg(QString::number(params.bias_y, 'g')));
    ui->line_param5->setText(tr("%1").arg(QString::number(params.baudrate)));

    ui->lbl_status->setText("Status: get parameters");
}

void SettingsDialog::getParamsHandler(QString data)
{
//    qDebug() << data;

//    data = "Par:Gain: 7.4;Scale: 1.53;BiasX: 0.001;BiasY: 0.002;Baudrate: 9600;\r\n";
    QVector<double> params;
    QRegularExpression re;
    re.setPattern("([-]?\\d*\\.?\\d+)");

    auto it = re.globalMatch(data);
    while(it.hasNext()){
        auto match = it.next();
        params.append(match.captured(0).toDouble());
    }

    m_params.gain = params.at(0);
    m_params.scale = params.at(1);
    m_params.bias_x = params.at(2);
    m_params.bias_y = params.at(3);
    m_params.baudrate = params.at(4);

    qDebug() << m_params.gain << m_params.scale << m_params.bias_x << m_params.bias_y << m_params.baudrate;

    fillInfo(m_params);

}

bool SettingsDialog::newParamsCorrect()
{
//    SettingsDialog::Parameters copy_param = parameters();

    bool res = false;

    qint32 baudrate = ui->line_param5->text().toInt();
    if(baudrate == 9600 || baudrate == 19200 || baudrate == 38400 || baudrate == 57600 || baudrate == 115200){
        res = true;
    }else{
        ui->lbl_status->setText("Status: baudrate param invalid value");
        res = false;
    }

    return res;

}

void SettingsDialog::setParamsPrepare()
{

    if(newParamsCorrect()){
        ui->lbl_status->setText("Status: set parameters");
        m_params.gain = ui->line_param1->text().toDouble();
        m_params.scale = ui->line_param2->text().toDouble();
        m_params.bias_x = ui->line_param3->text().toDouble();
        m_params.bias_y = ui->line_param4->text().toDouble();
        m_params.baudrate = ui->line_param5->text().toInt();

//        qDebug() << m_params.gain << m_params.scale << m_params.bias_x << m_params.bias_y << m_params.baudrate;

        emit setParams(m_params);
    }
}


void SettingsDialog::updateParamChecker(QString data)
{
    /*
     * Symbolical checking params
     */
    qDebug() << data;
//    QVector<double> params;
////    data = "Set gain -> 31Set scale -> 41.00Set biasX -> 0.00Set biasY -> 12.00\r\n";

//    QRegularExpression re;
//    re.setPattern("(Set)");

//    auto it = re.globalMatch(data);

//    while(it.hasNext()){
//        auto match = it.next();
//        params.append(match.captured(0).toDouble());
//    }

//    qsizetype size_param = sizeof(m_params)/sizeof(m_params.gain);

//    if(params.size() == size_param){
//        ui->lbl_status->setText("Status: parameters are set successfully");
//    }else{
//        ui->lbl_status->setText("Status: parameters are not set");
//    }
}
