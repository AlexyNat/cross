#ifndef OBJECTITEM_H
#define OBJECTITEM_H

#include <QGraphicsItem>
#include <QPainter>
#include <optional>
#include <QTime>
#include <QtMath>
#include <QtCore>

class ObjectItem : public QGraphicsLineItem
{
public:
    ObjectItem(quint8 id, QPointF point1, QPointF point2, std::optional<QPointF> point3,
               std::optional<qreal> dist, qreal K, qreal V, std::optional<QTime> time);
    quint8 getId() const { return id; } /// получить id
    QColor getColor() const { return col; } /// получить цвет
    qreal getAngle() const { return qRadiansToDegrees(K); } /// получить угол наклона
    qreal getVelocity() const { return V * 10; } /// получить скорость

    QPointF getStartPoint() const { return QPointF(line().p1().x() * 10, -line().p1().y() * 10); } /// точка начала
    QPointF getEndPoint() const { return QPointF(line().p2().x() * 10, -line().p2().y() * 10); } /// точка конца
    QPointF getInterPoint() const { return QPointF(iPos->x() * 10, -iPos->y() * 10); } /// точка пересечения

    std::optional<QPointF> getIntersectionPoint() const { return iPos; } /// получить точку пересечения
    std::optional<qreal> getDistance() const { return dist; } /// получить расстояние
    /// установить точки
    void setStartPoint(QPointF point) { point /= 10; setLine(point.x(), -point.y(), line().p2().x(), line().p2().y()); }
    void setEndPoint(QPointF point)   { point /= 10; setLine(line().p1().x(), line().p1().y(), point.x(), -point.y()); }
    qreal getDist() const { return dist.value() * 10; } /// получить расстояние
    qreal distance(); /// вычислить дистанцию между точками
    bool checkIntersection(ObjectItem *rhs); /// проверка
    void setVelocity(qreal V) { this->V = V / 10; } /// установить скорость
    void setAngle(qreal K) { this->K = !K ? 0 : qDegreesToRadians(K); } /// установить угол наклона
    std::optional<QPointF> getIPos() const { return iPos; }
    std::optional<QTime> getTime() const { return time; }
    void setIPos(QPointF point) { iPos = point; }
    void setInterPos(QPointF point) { point /= 10; iPos = point; }
    void setDist(qreal dist) { dist /= 10; this->dist = dist; }
    void setTime(QTime time) { this->time = time; }
    void resetIPos() { iPos.reset(); } /// сбросить точку пересечения
    void resetDist() { dist.reset(); } /// сбросить растояние
    void resetTime() { time.reset(); } /// сброс времени
private:
    /// идентификатор
    quint8 id;
    /// точка пересечения
    std::optional<QPointF> iPos;
    /// дистанция
    std::optional<qreal> dist;
    /// время пересечения
    std::optional<QTime> time;
    /// угол наклона
    qreal K;
    /// скорость
    qreal V;
    /// цвет
    QColor col;
public slots:
     /// приращение координат
     void step();
};

#endif // OBJECTITEM_H
