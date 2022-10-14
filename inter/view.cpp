#include "view.h"

View::View(QGraphicsScene* pScene, QGraphicsView *parent)
    : QGraphicsView{pScene, parent}
{
    setViewportUpdateMode(FullViewportUpdate);
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

QSize View::sizeHint() const
{
    return QSize(640, 480);
}

void View::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->save();
    painter->setPen(QPen(Qt::darkGray,1));

    /// Сетка
    for(int left = rect.center().x(); left > rect.topLeft().x();left -= 10) {
        painter->drawLine(left, rect.topLeft().y(),
                          left,  rect.bottomLeft().y());
    }

    for(int right = rect.center().x(); right < rect.topRight().x();right += 10) {
        painter->drawLine(right, rect.topLeft().y(),
                          right,  rect.bottomLeft().y());
    }

    for(int top = rect.center().y(); top > rect.topLeft().y();top -= 10) {
        painter->drawLine(rect.topLeft().x(), top,
                          rect.topRight().x(),  top);
    }

    for(int bottom = rect.center().y(); bottom < rect.bottomRight().y();bottom += 10) {
        painter->drawLine(rect.topLeft().x(), bottom,
                          rect.topRight().x(),  bottom);
    }
    /// середина
    painter->setPen(QPen(Qt::black,3));
    painter->drawPoint(scene()->sceneRect().center());
    painter->setFont(QFont("Times", 10, QFont::Bold));
    painter->drawText(QPointF(scene()->sceneRect().center().x() + 5, scene()->sceneRect().center().y() + 12), "(0, 0)");
    painter->drawText(QPointF(rect.topLeft().x() + 15, rect.topLeft().y() + 15), "Масштаб 100 x 100 метров [1px = 10м]");
    painter->restore();
}
