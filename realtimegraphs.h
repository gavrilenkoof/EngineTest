#ifndef REALTIMEGRAPHS_H
#define REALTIMEGRAPHS_H

#include <QWidget>

#include "qcustomplot.h"
#include "axistag.h"
#include "settingsdialog.h"
#include "motorcharacteristics.h"


namespace Ui {
class RealTimeGraphs;
}

class RealTimeGraphs : public QWidget
{
    Q_OBJECT

public:
    explicit RealTimeGraphs(QWidget *parent = nullptr);
    ~RealTimeGraphs();

    void clearGraphsAndBuffers();




private slots:
    void timerSlot();
    void updateTableSlot();

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

    QTimer m_timer_update_table;


    MotorCharacteristics m_motor_char;

    bool m_update_val_plot;

    double const m_x_axis_range = 60.0; // seconds

    void updateGraphs(double torque, double rpm, double timestamp, double sampletime);
    void updateTableValues(double &torque, double &rpm, double &power);
    void logData(double torque, double torque_avg, double rpm, double rpm_avg, double power_avg, double timestamp);
    void appendDoubleAndTrunc(QVector<double> *vec, double num, int max_size);

    SettingsDialog::Parameters m_params;



};

#endif // REALTIMEGRAPHS_H
