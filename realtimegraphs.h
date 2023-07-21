#ifndef REALTIMEGRAPHS_H
#define REALTIMEGRAPHS_H

#include <QWidget>

#include "qcustomplot.h"
#include "axistag.h"
#include "settingsdialog.h"


namespace Ui {
class RealTimeGraphs;
}

class RealTimeGraphs : public QWidget
{
    Q_OBJECT

public:
    explicit RealTimeGraphs(QWidget *parent = nullptr);
    ~RealTimeGraphs();

    void appendDoubleAndTrunc(QVector<double> *vec, double num, int max_size);
    void clearGraphsAndBuffers();

private slots:
    void timerSlot();

    void newDataHandler(QVector<QMap<QString, uint64_t>> data);
    void setParamRequest(SettingsDialog::Parameters params);

signals:
    void newDataTable(QVector<double> data);

private:
    Ui::RealTimeGraphs *ui;

    AxisTag *m_tag1;
    AxisTag *m_tag2;

    QCPRange m_range;

    QTimer m_timer_update_graphs;
    double m_second_counter;
    qint64 m_last_update_time;
    QVector<double> m_seconds;

    QVector<double> m_torque;
    QVector<double> m_rpm;
    QVector<double> m_timestamp;
    QVector<double> m_sampletime;
    bool m_update_val_plot;

    double const m_x_axis_range = 60.0; // seconds

    void updateGraphs(double &torque, double &rpm, double &timestamp, double &sampletime);
    void updateTableValues(double &torque, double &rpm, double &timestamp,double &sampletime);
    void logData(double &torque, double &rpm, double &timestamp,double &sampletime);

    SettingsDialog::Parameters m_params;



};

#endif // REALTIMEGRAPHS_H
