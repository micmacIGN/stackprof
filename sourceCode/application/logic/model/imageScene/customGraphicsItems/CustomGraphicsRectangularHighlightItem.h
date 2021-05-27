#ifndef CustomGraphicsRectangularHighlightItem_HPP
#define CustomGraphicsRectangularHighlightItem_HPP

#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>


class CustomGraphicsRectangularHighlightItem: public QObject, public /*QGraphicsItem*/ QGraphicsPolygonItem {

    Q_OBJECT

public:

    CustomGraphicsRectangularHighlightItem(const QVector<QPointF>& qvectqpf_boxCornersNoZLI,
                                           QGraphicsItem *parent=nullptr);

    CustomGraphicsRectangularHighlightItem(Qt::GlobalColor colorForUnselected,
                                           Qt::GlobalColor colorForSelected,
                                           QGraphicsItem *parent=nullptr);

    void adjustLocationAndSizeToZLI(int ZLI);

    ~CustomGraphicsRectangularHighlightItem() override;

     int type() const override;

signals:

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    void setSelected(bool bNewState);

private:

    int _currentZLI;

    QVector<QPointF> _qvectqpf_boxCornersNoZLI;

    QVector<QPointF> _qvectqpf_boxCornersZLI;

    QPolygonF _polygonF;

    bool _bHover; //mouse hover the shape
   
    //
    QPen _pForUnselectedPixel;
    QPen _pForSelectedPixel;
    QPen *_PtrUsedPen; //this one point on one of the QPen available just above

};

#endif // CUSTOMGRAPHICSBOXITEM_HPP
