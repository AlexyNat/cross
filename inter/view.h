#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include <QtCore>

/// отображение объектов
class View : public QGraphicsView
{
    Q_OBJECT
public:
    View(QGraphicsScene* pScene, QGraphicsView *parent = nullptr);
    QSize sizeHint() const override;
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
};

#endif // VIEW_H
