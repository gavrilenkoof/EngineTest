#ifndef REALTIMEGRAPHS_H
#define REALTIMEGRAPHS_H

#include <QWidget>

namespace Ui {
class RealTimeGraphs;
}

class RealTimeGraphs : public QWidget
{
    Q_OBJECT

public:
    explicit RealTimeGraphs(QWidget *parent = nullptr);
    ~RealTimeGraphs();

private:
    Ui::RealTimeGraphs *ui;
};

#endif // REALTIMEGRAPHS_H
