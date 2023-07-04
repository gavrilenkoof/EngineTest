#ifndef AXISTAG_H
#define AXISTAG_H

#include <QObject>
#include "qcustomplot.h"

class AxisTag : public QObject
{
    Q_OBJECT
public:
    explicit AxisTag(QCPAxis *parentAxis = nullptr);
    virtual ~AxisTag();

    // setters:
    void setPen(QPen const &pen);
    void setBrush(QBrush const &brush);
    void setText(QString const &text);

    // getters:
    QPen pen() const { return m_label->pen(); }
    QBrush brush() const { return m_label->brush(); }
    QString text() const { return m_label->text(); }

    // other methods:
    void updatePosition(double value);

protected:
    QCPAxis *m_axis;
    QPointer<QCPItemTracer> m_dummy_tracer;
    QPointer<QCPItemLine> m_arrow;
    QPointer<QCPItemText> m_label;
};

#endif // AXISTAG_H
