// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}


QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:

    struct Parameters{
        double gain;
        double scale;
        double bias_x;
        double bias_y;
        qint32 baudrate;
    };

    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    Parameters parameters() const;

private:
    Ui::SettingsDialog *ui = nullptr;

    Parameters m_params;

    void fillInfo(Parameters &params);

signals:
    void getParams();
    void setParams(SettingsDialog::Parameters);

private slots:
    void getParamsHandler(QString data);
    void setParamsPrepare();
};

#endif // SETTINGSDIALOG_H
