#include "realtimegraphs.h"
#include "ui_realtimegraphs.h"

RealTimeGraphs::RealTimeGraphs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RealTimeGraphs)
{
    ui->setupUi(this);

//    ui->lbl_1->setText("");
//    ui->lbl_2->setText("");
    ui->lbl_torque->setText("Torque: ");
    ui->lbl_rpm->setText("RPM: ");
    ui->label->setText("");
    ui->label_2->setText("");

    // Configuration first graph
    ui->plot_1->yAxis->setTickLabels(false);
    ui->plot_1->yAxis2->setVisible(true);
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(20);
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setLabel("RPM");
    ui->plot_1->axisRect()->axis(QCPAxis::atBottom, 0)->setLabel("Seconds (s)");
    ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0)->setTickLabelPadding(10);


    ui->plot_1->legend->setVisible(true);
    ui->plot_1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    ui->plot_1->addGraph(ui->plot_1->xAxis, ui->plot_1->axisRect()->axis(QCPAxis::atRight, 0));
    ui->plot_1->graph(0)->setPen(QPen(QColor(250, 120, 0)));
    ui->plot_1->graph(0)->setName("RPM");
    ui->plot_1->setVisible(true);


//    m_tag1 = new AxisTag(ui->plot_1->graph(0)->valueAxis());
//    m_tag1->setPen(ui->plot_1->graph(0)->pen());
//    m_tag1->setText("0");


    // Configuration second graph
    ui->plot_2->yAxis->setTickLabels(false);
    ui->plot_2->yAxis2->setVisible(true);
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(20);
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setLabel("Torque (N*m)");
    ui->plot_2->axisRect()->axis(QCPAxis::atBottom, 0)->setLabel("Seconds (s)");
//    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setNumberPrecision(2);
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setTickLabelPadding(10);
//    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setNumberFormat("f");  // TODO FIX moving yaxis2
//    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setRange(-5, 5);

    ui->plot_2->legend->setVisible(true);
    ui->plot_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    ui->plot_2->addGraph(ui->plot_2->xAxis, ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0));
    ui->plot_2->graph(0)->setPen(QPen(QColor(120, 250, 0)));
    ui->plot_2->graph(0)->setName("Torque");
    ui->plot_2->setVisible(true);

//    m_tag2 = new AxisTag(ui->plot_2->graph(0)->valueAxis());
//    m_tag2->setPen(ui->plot_2->graph(0)->pen());
//    m_tag2->setText("0");

//    m_second_counter = 0.0;
    m_update_val_plot = false;
//    m_last_update_time = QDateTime::currentMSecsSinceEpoch();

    connect(&m_timer_update_graphs, SIGNAL(timeout()), this, SLOT(timerSlot()));
    m_timer_update_graphs.start(1);


}

void RealTimeGraphs::clearGraphsAndBuffers()
{
    m_torque.clear();
    m_rpm.clear();
    m_timestamp.clear();
    m_sampletime.clear();

    ui->plot_1->graph(0)->data()->clear();
    ui->plot_2->graph(0)->data()->clear();

    ui->plot_1->replot();
    ui->plot_2->replot();
}

RealTimeGraphs::~RealTimeGraphs()
{
    delete ui;
}

void RealTimeGraphs::updateGraphs(double &torque, double &rpm, double &timestamp,double &sampletime)
{

      ui->plot_1->graph(0)->addData(timestamp, rpm);
      ui->plot_2->graph(0)->addData(timestamp, torque);
}

void RealTimeGraphs::updateTableValues(double &torque, double &rpm, double &timestamp,double &sampletime)
{
    ui->lbl_rpm->setText(tr("RPM: %1").arg(QString::number(rpm, 'g', 6)));
    ui->lbl_torque->setText(tr("Torque: %1").arg(QString::number(torque, 'g', 6)));
}


void RealTimeGraphs::newDataHandler(QVector<QMap<QString, uint64_t>> data)
{

    static int const size = 500;
    static double torque = 0;
    static double rpm = 0;
    static double timestamp = 0;
    static double sampletime = 0;


    for(auto &data_dict: data){
        torque = ((data_dict["Torque"] / 8388608.f) - 1.0f)*(5.0f / (float)(1 << 7));
//        appendDoubleAndTrunc(&m_torque, torque, size);

        rpm = data_dict["RPM"];
//        appendDoubleAndTrunc(&m_rpm, rpm, size);
        if(rpm > 0){
            qDebug() << "ERROR RPM" <<rpm;
        }

        timestamp = data_dict["Timestamp"] / 1000000.0; // us to sec with point
//        appendDoubleAndTrunc(&m_timestamp, timestamp, size);

        sampletime = data_dict["Sampletime"];
//        appendDoubleAndTrunc(&m_sampletime, sampletime, size);

        updateGraphs(torque, rpm, timestamp, sampletime);
        updateTableValues(torque, rpm, timestamp, sampletime);

        m_update_val_plot = true;
    }

}


void RealTimeGraphs::timerSlot()
{
    if(m_update_val_plot){
        ui->plot_1->xAxis->rescale(true);
        ui->plot_1->yAxis2->rescale(true);

        ui->plot_2->xAxis->rescale(true);
        ui->plot_2->yAxis2->rescale(true);
        if(ui->plot_1->graph(0)->dataMainKey(ui->plot_1->graph(0)->dataCount() - 1) > m_x_axis_range){
            ui->plot_1->xAxis->setRange(ui->plot_1->xAxis->range().upper, m_x_axis_range, Qt::AlignRight);
            ui->plot_2->xAxis->setRange(ui->plot_2->xAxis->range().upper, m_x_axis_range, Qt::AlignRight);
        }

//        double graphValue = ui->plot_1->graph(0)->dataMainValue(ui->plot_1->graph(0)->dataCount() - 1);
//        m_tag1->updatePosition(graphValue);
//        m_tag1->setText(QString::number(graphValue, 'f', 3));
//        graphValue = ui->plot_2->graph(0)->dataMainValue(ui->plot_2->graph(0)->dataCount() - 1);
//        m_tag2->updatePosition(graphValue);
//        m_tag2->setText(QString::number(graphValue, 'f', 3));
        ui->plot_1->replot();
        ui->plot_2->replot();

        m_update_val_plot = false;
    }

}


void RealTimeGraphs::appendDoubleAndTrunc(QVector<double> *vec, double num, int max_size)
{
        vec->append(num);

        if(vec->size() > max_size){
            vec->remove(0, vec->size() - max_size);
        }
}
