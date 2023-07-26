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
    ui(new Ui::SettingsDialog),
    m_settings("UAV", "EngineTest")
{
    ui->setupUi(this);
    setWindowTitle("Parameters");

//    ui->line_param6->setReadOnly(true);
//    ui->line_param7->setReadOnly(true);
    ui->line_param8->setReadOnly(true);
    ui->line_param9->setReadOnly(true);
    ui->line_param10->setReadOnly(true);

    connect(ui->btnGetParam, SIGNAL(clicked()), this, SLOT(getParams()));
    connect(ui->btnSetParam, SIGNAL(clicked()), this, SLOT(setParamsPrepare()));

    readSettings();
    fillInfo(m_params);

}

SettingsDialog::~SettingsDialog()
{
    writeSettings();
    delete ui;
}

SettingsDialog::Parameters SettingsDialog::parameters() const
{
    return m_params;
}

void SettingsDialog::readSettings()
{
    m_settings.beginGroup("/Parameters");

    m_params.gain = m_settings.value("/gain", 7).toInt();
    m_params.scale = m_settings.value("/scale", 13421.0).toDouble();
    m_params.bias_x = m_settings.value("/bias_x", 0.0).toDouble();
    m_params.bias_y = m_settings.value("/bias_y", -0.00001).toDouble();
    m_params.baudrate = m_settings.value("/baudrate", 1000000).toUInt();
    m_params.time_avg_values = m_settings.value("/time_avg_values", 1000).toUInt();
    m_params.efficiency_factor = m_settings.value("/efficiency_factor", 1.0).toDouble();

    m_settings.endGroup();

    emit setParams(m_params);
}

void SettingsDialog::writeSettings()
{
    qDebug() << "Save params";
    m_settings.beginGroup("/Parameters");

    m_settings.setValue("/gain", m_params.gain);
    m_settings.setValue("/scale", m_params.scale);
    m_settings.setValue("/bias_x", m_params.bias_x);
    m_settings.setValue("/bias_y", m_params.bias_y);
    m_settings.setValue("/baudrate", m_params.baudrate);
    m_settings.setValue("/time_avg_values", m_params.time_avg_values);
    m_settings.setValue("/efficiency_factor", m_params.efficiency_factor);

    m_settings.endGroup();
}

void SettingsDialog::fillInfo(SettingsDialog::Parameters &params)
{
    ui->line_param1->setText(tr("%1").arg(QString::number(params.gain)));
    ui->line_param2->setText(tr("%1").arg(QString::number(params.scale, 'g')));
    ui->line_param3->setText(tr("%1").arg(QString::number(params.bias_x, 'g')));
    ui->line_param4->setText(tr("%1").arg(QString::number(params.bias_y, 'g')));
    ui->line_param5->setText(tr("%1").arg(QString::number(params.baudrate)));
    ui->line_param6->setText(tr("%1").arg(QString::number(params.time_avg_values)));
    ui->line_param7->setText(tr("%1").arg(QString::number(params.efficiency_factor)));

    ui->lbl_status->setText("Status: get parameters");
}

bool SettingsDialog::newParamsCorrect()
{
    return true;

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
        m_params.time_avg_values = ui->line_param6->text().toUInt();
        m_params.efficiency_factor = ui->line_param7->text().toDouble();

        uploadParamsToGraphs();
    }
}

void SettingsDialog::uploadParamsToGraphs()
{
    emit setParams(m_params);
}

void SettingsDialog::getParams()
{
    fillInfo(m_params);
}
