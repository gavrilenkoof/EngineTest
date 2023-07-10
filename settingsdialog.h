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

class QIntValidator;

QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct Parameters {
        double val_1;
        double val_2;
        double val_3;
        double val_4;
    };

    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    Parameters parameters() const;


private:
    Ui::SettingsDialog *ui = nullptr;
    Parameters m_currentParams;
    QIntValidator *m_intValidator = nullptr;

signals:
    void getParams();

private slots:
    void getParamsHandler(QString data);
};

#endif // SETTINGSDIALOG_H
