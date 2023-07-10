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
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(20);
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setLabel("RPM");
    ui->plot_1->axisRect()->axis(QCPAxis::atBottom, 0)->setLabel("Seconds (s)");
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setTickLabelPadding(10);
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setRange(-50, 50);

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
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setRange(-5, 5);

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


}

RealTimeGraphs::~RealTimeGraphs()
{
    delete ui;
}


void RealTimeGraphs::newDataHandler(QString data)
{
    /*
     * T - 0 : torque (N*m)
     * R - 1 : rpm
     * Tm - 2 : time (ms)
     */
//    qDebug() << data;
    static int const size = 500;
//    static int const count_data = 3;


    QRegularExpression re;
    re.setPattern("([-]?\\d*\\.?\\d+)");

    auto it = re.globalMatch(data);

    QVector<QString> values;
    while(it.hasNext()){
        auto match = it.next();
        values.append(match.captured(0));
    }

    QVector<double> data_double;
    for (auto &val : values) {
        data_double.append(val.toDouble());
    }

    // save in vectors for plots
    appendDoubleAndTrunc(&m_values_1, data_double.at(0), size); // torque
    appendDoubleAndTrunc(&m_values_2, data_double.at(1), size); // rpm
//       appendDoubleAndTrunc(&m_seconds, values.at(2).toDouble(), size); // ms

    // timestamp of new values
    qint64 time_now = QDateTime::currentMSecsSinceEpoch(); // timestamp
    double elapsed = double((time_now - m_last_update_time)) / 1000.0;
    if (elapsed > 1.0) {
        elapsed = 1.0;
    }
    m_second_counter += elapsed;

    // save in time of new values
    appendDoubleAndTrunc(&m_seconds, m_second_counter, size);
    m_last_update_time = time_now;

    emit newDataTable(data_double);

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

        double graphValue = ui->plot_1->graph(0)->dataMainValue(ui->plot_1->graph(0)->dataCount() - 1);
        m_tag1->updatePosition(graphValue);
        m_tag1->setText(QString::number(graphValue, 'f', 3));

        graphValue = ui->plot_2->graph(0)->dataMainValue(ui->plot_2->graph(0)->dataCount() - 1);
        m_tag2->updatePosition(graphValue);
        m_tag2->setText(QString::number(graphValue, 'f', 3));

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
