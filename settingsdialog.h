// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSettings>

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
        int gain;
        double scale;
        double bias_x;
        double bias_y;
        qint32 baudrate;
        uint16_t time_avg_values;
        double efficiency_factor;
    };

    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    Parameters parameters() const;
    void uploadParamsToGraphs();

private:
    Ui::SettingsDialog *ui = nullptr;

    Parameters m_params;
    QSettings m_settings;

    void fillInfo(Parameters &params);

    bool newParamsCorrect();

    void readSettings();
    void writeSettings();

signals:
    void setParams(SettingsDialog::Parameters);

private slots:
    void setParamsPrepare();
    void getParams();
};

#endif // SETTINGSDIALOG_H
