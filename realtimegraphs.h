#ifndef REALTIMEGRAPHS_H
#define REALTIMEGRAPHS_H

#include <QWidget>

#include "qcustomplot.h"
#include "axistag.h"

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

private slots:
    void timerSlot();

    void newDataHandler(QByteArray data);

private:
    Ui::RealTimeGraphs *ui;

    AxisTag *m_tag1;
    AxisTag *m_tag2;

    QTimer m_timer_update_graphs;
    double m_second_counter;
    qint64 m_last_update_time;
    QVector<double> m_seconds;

    QVector<double> m_values_1;
    QVector<double> m_values_2;
    bool m_update_val_plot;

    double const m_x_axis_range = 60.0; // seconds

};

#endif // REALTIMEGRAPHS_H
