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
    static int const m_size = 2500; // 2.5 khz data -> filtering 1 sec

    struct AvgFilter
    {
        double data[m_size];
        uint16_t index;
        double sum;
        uint16_t count;
        uint16_t filter_size;

        double getAvg() {return sum / count;};

        void initFilter(uint16_t size)
        {
            memset(this, 0, sizeof(struct AvgFilter));

            this->filter_size = size;
            this->sum = 0.0;
        }

        double filterRunAvg(double newVal)
        {
            this->sum -= this->data[this->index];
            this->data[this->index] = newVal;
            this->sum += this->data[this->index];
            this->index++;
            this->index = this->index % this->filter_size;
            if(this->count < this->filter_size)
                this->count++;

            return this->sum / this->count;
        }
    };

    AvgFilter m_torque_filter;
    AvgFilter m_rpm_filter;
    AvgFilter m_power_filter;

    MotorCharacteristics m_motor_char;

    bool m_update_val_plot;

    double const m_x_axis_range = 60.0; // seconds

    void updateGraphs(double torque, double rpm, double timestamp, double sampletime);
    void updateTableValues(double &torque, double &rpm, double &power);
    void logData(double &torque, double &rpm, double &timestamp,double &sampletime, double &power);
    void appendDoubleAndTrunc(QVector<double> *vec, double num, int max_size);

    SettingsDialog::Parameters m_params;



};

#endif // REALTIMEGRAPHS_H
