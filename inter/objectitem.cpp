#include "objectitem.h"

ObjectItem::ObjectItem(quint8 id, QPointF point1, QPointF point2,std::optional<QPointF> point3,
                       std::optional<qreal> dist, qreal K, qreal V, std::optional<QTime> time)
    : QGraphicsLineItem(QLineF(point1, point2)), id(id),iPos( point3),
      dist(dist), time(time), K(!K ? 0 : qDegreesToRadians(K)), V(V), col(Qt::GlobalColor(Qt::red + (id - 1) % 11))
{
    setPen(QPen(col, 4));
    /// инверсия по Oy
    setLine(QLineF(QPointF(line().p1().x(), -line().p1().y()), QPointF(line().p2().x(), -line().p2().y())));
}


/// определение дистанции между точками в 2d пространстве
qreal ObjectItem::distance()
{
    QLineF line = this->line();
    dist = qSqrt(qPow(line.x2() - iPos->x(), 2) + qPow(line.y2() - iPos->y(), 2));
    return dist.value();
}

void ObjectItem::step()
{
    QPointF p2 = line().p2();
    /// 1px = 10м
    p2.setX(p2.x() + qCos(K) * V);
    p2.setY(p2.y() - qSin(K) * V);
    setLine(QLineF(line().p1(), p2));
}

/// отпределение столкновения или пересечения
bool ObjectItem::checkIntersection(ObjectItem *rhs)
{
    /// this->collidesWithItem(rhs)

    /// столкновение
    if(line().p2() == rhs->line().p2()) {
        rhs->setIPos(line().p2());
        setIPos(line().p2());
        return true;
    }

    if(qFuzzyCompare(1 + qRound(qTan(K) * 100) / 100.0,1 + qRound(qTan(rhs->K) * 100) / 100.0)) {
       // qDebug() << "Параллельные курсы";
        /// TODO проверка при параллельных курсах
        /// движение друг к другу или в противоположные стороны

        if(qFuzzyCompare(1 + qMax(K, rhs->K) - M_PI, 1 + qMin(K, rhs->K))) {
          //  qDebug() << "Движение друг к другу или в противоположные стороны";
        }
        /// один догоняет другого
        /// проверить коллизию
        return false;
    }

    /// Если без столкновения объектов [1 точка пересечения отрезков]
    /// [abs(tan1) != abs(tan2)], то найти
    /// точку пересечения отрезков

    /// для удобства
    QPointF p1 = this->line().p1();
    QPointF p2 = this->line().p2();
    QPointF p3 = rhs->line().p1();
    QPointF p4 = rhs->line().p2();

    /// левый отрезок будет левее
    if (p2.x() < p1.x()) {
        QPointF tmp = p1;
        p1 = p2;
        p2 = tmp;
    }

    if (p4.x() < p3.x()) {
        QPointF tmp = p3;
        p3 = p4;
        p4 = tmp;
    }

    if (p2.x() < p3.x()) { /// нет точки пересечения
        return false;
    }

    /// вертикальные отрезки (частный случай) ( tan Pi / 2 + Pk )
    if(qFuzzyCompare(1 + p1.x(),1 + p2.x()) && qFuzzyCompare(1 + p3.x(),1 + p4.x())) {
        /// лежат на одном X
        if(qFuzzyCompare(1 + p1.x(),1 + p3.x())) {
            /// проверим пересекаются ли они, т.е. есть ли у них общий Y
            if (!((qMax(p1.y(), p2.y()) < qMin(p3.y(), p4.y())) ||
                    (qMin(p1.y(), p2.y()) > qMax(p3.y(), p4.y())))) {
                rhs->setIPos(QPointF(p1.x(), p1.y()));
                setIPos(QPointF(p1.x(), p1.y()));
                return true;
            }
        }
        return false;
    }
    /// 1 отрезок вертикальный ( tan Pi / 2 + Pk )
    if (qFuzzyCompare(1 + p1.x(),1 + p2.x())) {
        /// найдём Xa, Ya - точки пересечения двух прямых
        qreal Xa = p1.x();
        qreal A2 = (p3.y() - p4.y()) / (p3.x() - p4.x());
        qreal B2 = p3.y() - A2 * p3.x();
        qreal Ya = A2 * Xa + B2;
        if (p3.x() <= Xa && p4.x() >= Xa && qMin(p1.y(), p2.y()) <= Ya &&
                qMax(p1.y(), p2.y()) >= Ya) {
            rhs->setIPos(QPointF(Xa, Ya));
            setIPos(QPointF(Xa, Ya));
            return true;
        }
        return false;
    }
    /// 2 отрезок вертикальный ( tan Pi / 2 + Pk )
    if (qFuzzyCompare(1 + p3.x(),1 + p4.x())) {
        /// найдём Xa, Ya - точки пересечения двух прямых
        qreal Xa = p3.x();
        qreal A1 = (p1.y() - p2.y()) / (p1.x() - p2.x());
        qreal B1 = p1.y() - A1 * p1.x();
        qreal Ya = A1 * Xa + B1;

        if (p1.x() <= Xa && p2.x() >= Xa && qMin(p3.y(), p4.y()) <= Ya &&
                qMax(p3.y(), p4.y()) >= Ya) {
            rhs->setIPos(QPointF(Xa, Ya));
            setIPos(QPointF(Xa, Ya));
            return true;
        }
        return false;
    }

    /// общий случай
    qreal A1 = (p1.y() - p2.y()) / (p1.x() - p2.x());
    qreal A2 = (p3.y() - p4.y()) / (p3.x() - p4.x());
    qreal B1 = p1.y() - A1 * p1.x();
    qreal B2 = p3.y() - A2 * p3.x();

    if (A1 == A2) {
        return false; /// отрезки параллельны
    }

    /// Xa - абсцисса точки пересечения
    qreal Xa = (B2 - B1) / (A1 - A2);

    if ((Xa < qMax(p1.x(), p3.x())) || (Xa > qMin( p2.x(), p4.x()))) {
        return false; ///точка Xa находится вне пересечения проекций отрезков на ось X
    }

    qreal Ya = A2 * Xa + B2; /// ордината точки пересечения
    rhs->setIPos(QPointF(Xa, Ya));
    setIPos(QPointF(Xa, Ya));
    return true;
}
