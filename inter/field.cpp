#include "field.h"

Field::Field(QWidget *parent)
    : QGraphicsView(parent)
{

}

void Field::step()
{
    for(auto &val : objs) {
        val.P.setX(val.P.x() + val.K * qFloor(qCos(val.V) * 100) / 100);
        val.P.setY(val.P.y() - val.K * qFloor(qSin(val.V) * 100) / 100);
    }
}

void Field::addObject(QPoint p, qreal v, qreal k)
{
    ObjectData tmp;
    tmp.P = p; /// начальная позиция
    tmp.V = qDegreesToRadians(v); /// в радианы
    tmp.K = k / 10; /// 1 px = 10 m/c
    tmp.col = QColor(Qt::GlobalColor(Qt::GlobalColor::red + (objs.size()) % 11)); /// назначить цвет
    objs.emplace_back(tmp);
}

ObjectData Field::getObject(quint8 number) const
{
    if(number == 0)
        throw QString("number >= 1");
    return objs[number - 1];
}

void Field::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.translate(rect().center());
    for(auto val : objs) {
        p.setPen(QPen(val.col, 3));
        p.drawPoint(val.P);
    }
}



Field::~Field()
{

}
