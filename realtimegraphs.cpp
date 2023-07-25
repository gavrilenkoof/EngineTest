#include "realtimegraphs.h"
#include "ui_realtimegraphs.h"

RealTimeGraphs::RealTimeGraphs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RealTimeGraphs)
{
    ui->setupUi(this);

    ui->lbl_torque->setText("Torque (N*m): ");
    ui->lbl_rpm->setText("RPM: ");
    ui->lbl_power->setText("Power (W): ");
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


    // Configuration second graph
    ui->plot_2->yAxis->setTickLabels(false);
    ui->plot_2->yAxis2->setVisible(true);
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setPadding(20);
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setLabel("Torque (N*m)");
    ui->plot_2->axisRect()->axis(QCPAxis::atBottom, 0)->setLabel("Seconds (s)");
    ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0)->setTickLabelPadding(10);

    ui->plot_2->legend->setVisible(true);
    ui->plot_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    ui->plot_2->addGraph(ui->plot_2->xAxis, ui->plot_2->axisRect()->axis(QCPAxis::atRight, 0));
    ui->plot_2->graph(0)->setPen(QPen(QColor(120, 250, 0)));
    ui->plot_2->graph(0)->setName("Torque (N*m)");
    ui->plot_2->setVisible(true);

    m_params.gain = 1;
    m_params.scale = 0;

    m_update_val_plot = false;

    connect(&m_timer_update_graphs, SIGNAL(timeout()), this, SLOT(timerSlot()));
    m_timer_update_graphs.start(1);

    connect(&m_timer_update_table, SIGNAL(timeout()), this, SLOT(updateTableSlot()));
    m_timer_update_table.start(100);

    // Init log table in file
    qInfo() << "Torque (N*m)" << " " << "Torque avg (N*m)" << " " << "RPM" << " " << "RPM avg" << "Power avg (W)" << " " << "Timestamp (s)";

    m_motor_char.m_torque_filter.initFilter();
    m_motor_char.m_rpm_filter.initFilter();
    m_motor_char.m_power_filter.initFilter();



}


void RealTimeGraphs::clearGraphsAndBuffers()
{

    ui->plot_1->graph(0)->data()->clear();
    ui->plot_2->graph(0)->data()->clear();

    m_motor_char.m_torque_filter.initFilter();
    m_motor_char.m_rpm_filter.initFilter();
    m_motor_char.m_power_filter.initFilter();

    ui->plot_1->replot();
    ui->plot_2->replot();
}

RealTimeGraphs::~RealTimeGraphs()
{
    delete ui;
}

void RealTimeGraphs::updateGraphs(double torque, double rpm, double timestamp,double sampletime)
{
    Q_UNUSED(sampletime);
    ui->plot_1->graph(0)->addData(timestamp, rpm);
    ui->plot_2->graph(0)->addData(timestamp, torque);
}

void RealTimeGraphs::updateTableValues(double &torque, double &rpm, double &power)
{
    ui->lbl_rpm->setText(tr("RPM: %1").arg(QString::number(rpm, 'g', 6)));
    ui->lbl_torque->setText(tr("Torque (N*m): %1").arg(QString::number(torque, 'g', 6)));
    ui->lbl_power->setText(tr("Power (W): %1").arg(QString::number(power, 'g', 6)));
}


void RealTimeGraphs::updateTableSlot()
{
    static double torque_avg = 0;
    static double rpm_avg = 0;
    static double power_avg = 0;

    torque_avg = m_motor_char.m_torque_filter.getAvg();
    rpm_avg = m_motor_char.m_rpm_filter.getAvg();
//    power_avg = m_motor_char.m_power_filter.getAvg();
    power_avg = m_motor_char.m_power_avg;

    updateTableValues(torque_avg, rpm_avg, power_avg);

}


void RealTimeGraphs::logData(double torque, double torque_avg, double rpm, double rpm_avg, double timestamp, double power_avg)
{

#ifdef RELEASE
    qInfo() << torque << " " << torque_avg << " " << rpm << rpm_avg << " " << power_avg << " " << timestamp;
#else
    Q_UNUSED(torque);
    Q_UNUSED(torque_avg);
    Q_UNUSED(rpm);
    Q_UNUSED(rpm_avg);
    Q_UNUSED(timestamp);
    Q_UNUSED(power_avg);
#endif

}

void RealTimeGraphs::newDataHandler(QVector<QMap<QString, uint64_t>> data)
{

    static double torque = 0;
    static double rpm = 0;
    static double timestamp = 0;
    static double sampletime = 0;


    for(auto &data_dict: data){
        torque = ((data_dict["Torque"] / 8388608.f) - 1.0f)*(5.0f / (float)(1 << m_params.gain));
        torque = (torque - m_params.bias_x) * m_params.scale + m_params.bias_y;
        m_motor_char.m_torque_filter.filterRunAvg(torque);

        rpm = data_dict["RPM"];

        timestamp = data_dict["Timestamp"] / 1000000.0; // us to s with point
        sampletime = data_dict["Sampletime"];

        rpm = rpm * 1.58;
        m_motor_char.m_rpm_filter.filterRunAvg(rpm);

        m_motor_char.m_power_avg = 1.0 * m_motor_char.m_torque_filter.getAvg() * m_motor_char.m_rpm_filter.getAvg() / 9550.0;

        m_motor_char.checkMaxTorque(m_motor_char.m_torque_filter.getAvg(), m_motor_char.m_rpm_filter.getAvg());
        m_motor_char.checkMaxRpm(m_motor_char.m_rpm_filter.getAvg());

        updateGraphs(m_motor_char.m_torque_filter.getAvg(), rpm, timestamp, sampletime);
        logData(torque, m_motor_char.m_torque_filter.getAvg(), rpm, m_motor_char.m_rpm_filter.getAvg(), m_motor_char.m_power_avg, timestamp);
        m_update_val_plot = true;
    }



}


void RealTimeGraphs::timerSlot()
{
//    static bool found_range = false;
//    static double new_upper = 0.0;
//    static double new_lower = 0.0;

    if(m_update_val_plot){
        ui->plot_1->xAxis->rescale(true);
        ui->plot_1->yAxis2->rescale(true);

        ui->plot_2->xAxis->rescale(true);
        ui->plot_2->yAxis2->rescale(true);

//        if(ui->plot_1->graph(0)->dataMainKey(ui->plot_1->graph(0)->dataCount() - 1) > m_x_axis_range){
            ui->plot_1->xAxis->setRange(ui->plot_1->xAxis->range().upper, m_x_axis_range, Qt::AlignRight);
            ui->plot_2->xAxis->setRange(ui->plot_2->xAxis->range().upper, m_x_axis_range, Qt::AlignRight);
            ui->plot_1->graph(0)->data().data()->removeBefore((ui->plot_1->graph(0)->dataMainKey(ui->plot_1->graph(0)->dataCount() - 1) - m_x_axis_range));
            ui->plot_2->graph(0)->data().data()->removeBefore((ui->plot_2->graph(0)->dataMainKey(ui->plot_2->graph(0)->dataCount() - 1) - m_x_axis_range));
//        }

//        m_range =  ui->plot_1->graph(0)->getValueRange(found_range);
//        if(found_range){
//            new_upper = m_range.upper + m_range.center() / 2;
//            new_lower = m_range.lower - m_range.center() / 2;
//            ui->plot_1->yAxis2->setRange(new_lower, new_upper);
//        }

//        m_range =  ui->plot_2->graph(0)->getValueRange(found_range);
//        if(found_range){
//            new_upper = m_range.upper + m_range.center() / 2;
//            new_lower = m_range.lower - m_range.center() / 2;
//            ui->plot_2->yAxis2->setRange(new_lower, new_upper);
//        }

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


void RealTimeGraphs::setParamRequest(SettingsDialog::Parameters params)
{
    m_params.gain = params.gain;
    m_params.scale = params.scale;
    m_params.bias_x = params.bias_x;
    m_params.bias_y = params.bias_y;
}
