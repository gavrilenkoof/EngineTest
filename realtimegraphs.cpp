#include "realtimegraphs.h"
#include "ui_realtimegraphs.h"

RealTimeGraphs::RealTimeGraphs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RealTimeGraphs)
{
    ui->setupUi(this);

    // Configuration first graph
    ui->plot_1->yAxis->setTickLabels(false);
    ui->plot_1->yAxis2->setVisible(true);
//    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(20);
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setLabel("RPM");
    ui->plot_1->axisRect()->axis(QCPAxis::atBottom, 0)->setLabel("Seconds (s)");
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setTickLabelPadding(10);

//    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setNumberPrecision(2);


    ui->plot_1->legend->setVisible(true);
    ui->plot_1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    ui->plot_1->addGraph(ui->plot_1->xAxis, ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0));
    ui->plot_1->graph(0)->setPen(QPen(QColor(250, 120, 0)));
    ui->plot_1->graph(0)->setName("RPM");
    ui->plot_1->setVisible(true);


    m_tag1 = new AxisTag(ui->plot_1->graph(0)->valueAxis());
    m_tag1->setPen(ui->plot_1->graph(0)->pen());


    // Configuration second graph
    ui->plot_2->yAxis->setTickLabels(false);
    ui->plot_2->yAxis2->setVisible(true);
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(20);
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setLabel("Torque (N*m)");
    ui->plot_2->axisRect()->axis(QCPAxis::atBottom, 0)->setLabel("Seconds (s)");
//    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setNumberPrecision(2);
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setTickLabelPadding(10);
//    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setNumberFormat("f");  // TODO FIX moving yaxis2

    ui->plot_2->legend->setVisible(true);
    ui->plot_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    ui->plot_2->addGraph(ui->plot_2->xAxis, ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0));
    ui->plot_2->graph(0)->setPen(QPen(QColor(120, 250, 0)));
    ui->plot_2->graph(0)->setName("Torque");
    ui->plot_2->setVisible(true);

    m_tag2 = new AxisTag(ui->plot_2->graph(0)->valueAxis());
    m_tag2->setPen(ui->plot_2->graph(0)->pen());



    m_second_counter = 0.0;
    m_update_val_plot = false;
    m_last_update_time = QDateTime::currentMSecsSinceEpoch();

    connect(&m_timer_update_graphs, SIGNAL(timeout()), this, SLOT(timerSlot()));
    m_timer_update_graphs.start(50);

    connect(&m_timer_new_data, SIGNAL(timeout()), this, SLOT(valuesReceived()));
    m_timer_new_data.start(100);

}

RealTimeGraphs::~RealTimeGraphs()
{
    delete ui;
}


void RealTimeGraphs::valuesReceived()
{
    static int const size = 500;
    static int i = 0;

    double new_val = qSin(i/50.0) + qSin(i/50.0/0.3843)*0.25;
    double new_val2 = 50*qSin(i/50.0) + 10*qSin(i/50.0/0.3843)*0.25;

    ++i;

    appendDoubleAndTrunc(&m_values_1, new_val, size);
    appendDoubleAndTrunc(&m_values_2, new_val2, size);

    qint64 time_now = QDateTime::currentMSecsSinceEpoch();
    double elapsed = double((time_now - m_last_update_time)) / 1000.0;
    if (elapsed > 1.0) {
        elapsed = 1.0;
    }

    m_second_counter += elapsed;

    appendDoubleAndTrunc(&m_seconds, m_second_counter, size);
    m_last_update_time = time_now;

    m_update_val_plot = true;
}

void RealTimeGraphs::timerSlot()
{
    if(m_update_val_plot){
        ui->plot_1->graph(0)->addData(m_seconds.back(), m_values_1.back());
        ui->plot_2->graph(0)->addData(m_seconds.back(), m_values_2.back());

        ui->plot_1->xAxis->rescale(true);
        ui->plot_1->yAxis2->rescale(true);

        ui->plot_2->xAxis->rescale(true);
        ui->plot_2->yAxis2->rescale(true);

        if(m_seconds.back() >= m_x_axis_range){
            ui->plot_1->xAxis->setRange(ui->plot_1->xAxis->range().upper, m_x_axis_range, Qt::AlignRight);
            ui->plot_2->xAxis->setRange(ui->plot_2->xAxis->range().upper, m_x_axis_range, Qt::AlignRight);
        }

        m_update_val_plot = false;
        ui->plot_1->replot();
        ui->plot_2->replot();

    }
}


void RealTimeGraphs::appendDoubleAndTrunc(QVector<double> *vec, double num, int max_size)
{
        vec->append(num);

        if(vec->size() > max_size){
            vec->remove(0, vec->size() - max_size);
        }
}
