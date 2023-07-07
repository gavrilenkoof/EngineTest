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
    m_ui(new Ui::SettingsDialog),
    m_intValidator(new QIntValidator(0, 4000000, this))
{
    m_ui->setupUi(this);
    setWindowTitle("Parameters");


}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

SettingsDialog::Settings SettingsDialog::settings() const
{
    return m_currentSettings;
}

void SettingsDialog::showPortInfo(int idx)
{

}

void SettingsDialog::apply()
{
//    updateSettings();
//    hide();
}

void SettingsDialog::checkCustomBaudRatePolicy(int idx)
{

}

void SettingsDialog::checkCustomDevicePathPolicy(int idx)
{

}

void SettingsDialog::fillPortsParameters()
{

}

void SettingsDialog::fillPortsInfo()
{

}

void SettingsDialog::updateSettings()
{


//    m_currentSettings.localEchoEnabled = m_ui->localEchoCheckBox->isChecked();
}
