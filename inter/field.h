#ifndef FIELD_H
#define FIELD_H

#include <QtWidgets>

struct ObjectData
{
    QPointF P;
    qreal V;
    qreal K;
    QColor col;
};


class Field : public QGraphicsView
{
    Q_OBJECT
public:
    Field(QWidget *parent = nullptr);
    ~Field();
    void addObject(QPoint p, qreal v, qreal k);
    ObjectData getObject(quint8 number) const;
    void step(); /// 1 шаг
private:
    std::vector<ObjectData> objs; /// объекты
protected:
    void paintEvent(QPaintEvent *pe) override;
};

#endif // FIELD_H
