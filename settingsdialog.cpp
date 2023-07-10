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
    m_intValidator(new QIntValidator(0, 4000000, this))
{
    ui->setupUi(this);
    setWindowTitle("Parameters");

    connect(ui->btnGetParam, SIGNAL(clicked()), this, SIGNAL(getParams()));

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

SettingsDialog::Parameters SettingsDialog::parameters() const
{
    return m_currentParams;
}

void SettingsDialog::getParamsHandler(QString data)
{

    QRegularExpression re;
    re.setPattern("([-]?\\d*\\.?\\d+)");

    qDebug() << data;

}

