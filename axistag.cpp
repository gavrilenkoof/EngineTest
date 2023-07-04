#include "axistag.h"

AxisTag::AxisTag(QCPAxis *parentAxis) :
    QObject(parentAxis),
    m_axis(parentAxis)
{
    // The dummy tracer serves here as an invisible anchor which always sticks to the right side of
    // the axis rect
    m_dummy_tracer = new QCPItemTracer(m_axis->parentPlot());
    m_dummy_tracer->setVisible(false);
    m_dummy_tracer->position->setTypeX(QCPItemPosition::ptAxisRectRatio);
    m_dummy_tracer->position->setTypeY(QCPItemPosition::ptPlotCoords);
    m_dummy_tracer->position->setAxisRect(m_axis->axisRect());
    m_dummy_tracer->position->setAxes(0, m_axis);
    m_dummy_tracer->position->setCoords(1, 0);


    // the arrow end (head) is set to move along with the dummy tracer by setting it as its parent
    // anchor. Its coordinate system (setCoords) is thus pixels, and this is how the needed horizontal
    // offset for the tag of the second y axis is achieved. This horizontal offset gets dynamically
    // updated in AxisTag::updatePosition. the arrow "start" is simply set to have the "end" as parent
    // anchor. It is given a horizontal offset to the right, which results in a 15 pixel long arrow.
    m_arrow = new QCPItemLine(m_axis->parentPlot());
    m_arrow->setLayer("overlay");
    m_arrow->setClipToAxisRect(false);
    m_arrow->setHead(QCPLineEnding::esSpikeArrow);
    m_arrow->end->setParentAnchor(m_dummy_tracer->position);
    m_arrow->start->setParentAnchor(m_arrow->end);
    m_arrow->start->setCoords(15, 0);

    // The text label is anchored at the arrow start (tail) and has its "position" aligned at the
    // left, and vertically centered to the text label box.
    m_label = new QCPItemText(m_axis->parentPlot());
    m_label->setLayer("overlay");
    m_label->setClipToAxisRect(false);
    m_label->setPadding(QMargins(3, 0, 3, 0));
    m_label->setBrush(Qt::white);
    m_label->setPen(QPen(Qt::blue));
    m_label->setPositionAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_label->position->setParentAnchor(m_arrow->start);

}


AxisTag::~AxisTag()
{
    if(m_dummy_tracer)
        m_dummy_tracer->parentPlot()->removeItem(m_dummy_tracer);
    if(m_arrow)
        m_arrow->parentPlot()->removeItem(m_arrow);
    if(m_label)
        m_label->parentPlot()->removeItem(m_label);
}


void AxisTag::setPen(QPen const &pen)
{
    m_arrow->setPen(pen);
    m_label->setPen(pen);
}

void AxisTag::setBrush(QBrush const &brush)
{
    m_label->setBrush(brush);
}

void AxisTag::setText(QString const &text)
{
    m_label->setText(text);
}

void AxisTag::updatePosition(double value)
{
  // since both the arrow and the text label are chained to the dummy tracer (via anchor
  // parent-child relationships) it is sufficient to update the dummy tracer coordinates. The
  // Horizontal coordinate type was set to ptAxisRectRatio so to keep it aligned at the right side
  // of the axis rect, it is always kept at 1. The vertical coordinate type was set to
  // ptPlotCoordinates of the passed parent axis, so the vertical coordinate is set to the new
  // value.
  m_dummy_tracer->position->setCoords(1, value);

  // We want the arrow head to be at the same horizontal position as the axis backbone, even if
  // the axis has a certain offset from the axis rect border (like the added second y axis). Thus we
  // set the horizontal pixel position of the arrow end (head) to the axis offset (the pixel
  // distance to the axis rect border). This works because the parent anchor of the arrow end is
  // the dummy tracer, which, as described earlier, is tied to the right axis rect border.
  m_arrow->end->setCoords(m_axis->offset(), 0);
}
