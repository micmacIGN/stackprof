#ifndef CUSTOMGRAPHICSVECTORDIRECTIONATPOINTITEM_HPP
#define CUSTOMGRAPHICSVECTORDIRECTIONATPOINTITEM_HPP

#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

class CustomGraphicsVectorDirectionAtPointItem: public QGraphicsObject {

    Q_OBJECT

public:

    CustomGraphicsVectorDirectionAtPointItem(int idPoint,
                                            QPointF startingPointLocationNoZLI,
                                            QPointF unitvectorDirectionNoZLI,
                                            QGraphicsItem *parent=nullptr);

    CustomGraphicsVectorDirectionAtPointItem(int idPoint,
                                            QPointF startingPointLocationNoZLI,
                                            QPointF unitvectorDirectionNoZLI,
                                            Qt::GlobalColor colorForUnselected, Qt::GlobalColor colorForSelected,
                                            QGraphicsItem *parent=nullptr);

    QRectF boundingRect() const override ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void adjustLocationAndSizeToZLI(int ZLI);

    int type() const override;

    ~CustomGraphicsVectorDirectionAtPointItem() override;

signals:

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:

    void setSelected(bool bNewState);

private:

    int _id;

    int _currentZLI;

    QPointF _qpf_startingPointLocationNoZLI; //starting point + of the graphic vector like that +--->
    QPointF _qpf_unitvectorDirectionNoZLI;
    QPointF _qpf_endingPointLocationNoZLI; //computed using _qpf_startingPointLocationNoZLI and _qpf_unitvectorDirectionNoZLI
    QPointF _arrowExtremitySide1NoZLI;
    QPointF _arrowExtremitySide2NoZLI;

    bool _bShowPerpendicularDirectionsAtPoint;
    QPointF _perpendicularExtermitySegment1NoZLI;
    QPointF _perpendicularExtermitySegment2NoZLI;

    QRectF _qrectf_boundingBoxNoZLI;
    //boudingBox of the shape around the point, ZLI dependent
    QRectF _qrectf_boundingBox;

    bool _bHover; //mouse hover the shape

    //
    QPen _pForUnselectedPixel;
    QPen _pForSelectedPixel;
    QPen *_PtrUsedPen; //this one point on one of the QPen available just above

};

#endif // CUSTOMGRAPHICSVECTORDIRECTIONATPOINTITEM_HPP
