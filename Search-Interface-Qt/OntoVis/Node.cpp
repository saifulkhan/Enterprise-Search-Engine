#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "GraphWidget.h"
#include "Edge.h"

#include "Node.h"


Node::Node(QString node_name, GraphWidget *graphWidget) : m_node_name(node_name), graph(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}


QRectF Node::boundingRect() const
{
     qreal adjust = 2;
     return QRectF(-10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}


QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}


void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    QRadialGradient gradient(-3, -3, 10);
    if (option->state & QStyle::State_Sunken)
    {
        gradient.setCenter(3, 3);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::cyan).light(120));
        gradient.setColorAt(0, QColor(Qt::darkCyan).light(120));
    }
    else
    {
        gradient.setColorAt(0, Qt::cyan);
        gradient.setColorAt(1, Qt::darkCyan);
    }
    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);

    // The node name
    float factor = boundingRect().width() / painter->fontMetrics().width(m_node_name);
    if ((factor < 1) || (factor > 1.25))
    {
        QFont f = painter->font();
        f.setPointSizeF(f.pointSizeF()*factor);
        painter->setFont(f);
    }
    painter->drawText(boundingRect(), Qt::AlignBottom, m_node_name);
}


QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
        case ItemPositionHasChanged:
            foreach (Edge *edge, edgeList)
                edge->adjust();

            graph->itemMoved();
            break;

        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}


void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}


void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}


void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}


QList<Edge*> Node::edges() const
{
    return edgeList;
}


void Node::calculateForces()
{
     if (!scene() || scene()->mouseGrabberItem() == this) {
         newPos = pos();
         return;
     }

     // Sum up all forces pushing this item away
     qreal xvel = 0;
     qreal yvel = 0;
     foreach (QGraphicsItem *item, scene()->items()) {
         Node *node = qgraphicsitem_cast<Node *>(item);
         if (!node)
             continue;

         QPointF vec = mapToItem(node, 0, 0);
         qreal dx = vec.x();
         qreal dy = vec.y();
         double l = 2.0 * (dx * dx + dy * dy);
         if (l > 0) {
             xvel += (dx * 150.0) / l;
             yvel += (dy * 150.0) / l;
         }
     }

     // Now subtract all forces pulling items together
     double weight = (edgeList.size() + 1) * 10;
     foreach (Edge *edge, edgeList) {
         QPointF vec;
         if (edge->sourceNode() == this)
             vec = mapToItem(edge->destNode(), 0, 0);
         else
             vec = mapToItem(edge->sourceNode(), 0, 0);
         xvel -= vec.x() / weight;
         yvel -= vec.y() / weight;
     }

     if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
         xvel = yvel = 0;

     QRectF sceneRect = scene()->sceneRect();
     newPos = pos() + QPointF(xvel, yvel);
     newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
     newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
}


bool Node::advance()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}



