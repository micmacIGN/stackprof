#ifndef CUSTOMGRAPHICSTINYTARGETEDPOINTITEM_HPP
#define CUSTOMGRAPHICSTINYTARGETEDPOINTITEM_HPP

#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

class CustomGraphicsTinyTargetPointItem: public QGraphicsObject {

    Q_OBJECT

public:
    CustomGraphicsTinyTargetPointItem(int idPoint, QPointF location, qreal sideSizeNoZLI, QGraphicsItem *parent=nullptr);

    CustomGraphicsTinyTargetPointItem(
            int idPoint,
            QPointF location, qreal sideSizeNoZLI,
            Qt::GlobalColor colorForUnselected, Qt::GlobalColor colorForSelected,
            QGraphicsItem *parent=nullptr);

    QRectF boundingRect() const override ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void adjustLocationAndSizeToZLI(int ZLI);

    int type() const override;

    ~CustomGraphicsTinyTargetPointItem() override;

signals:

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:

    void setSelected(bool bNewState);

private:

    int _id;

    int _currentZLI;

    QPointF _qpointf_locationNoZLI; //center of the shape: the point at ZLI = 1
    QSizeF _qsizef_sideSizeNoZLI;     //side size of rge shape around the center

    QRectF _qrectf_locationAndSize; //location and size of the shape around the center and ZLI dependent

    //boudingBox of the shape around the point, ZLI dependent
    QRectF _qrectf_boundingBox;

    bool _bHover; //mouse hover the shape

    //
    QPen _pForUnselectedPixel;
    QPen _pForSelectedPixel;
    QPen *_PtrUsedPen; //this one point on one of the QPen available just above

};

#endif // CUSTOMGRAPHICSTINYTARGETEDPOINTITEM_HPP
