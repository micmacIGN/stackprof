#ifndef GRAPHICSROUTEITEM_HPP
#define GRAPHICSROUTEITEM_HPP

#include <QGraphicsItem>
#include <QDebug>

#include "CustomGraphicsTargetedPointItem2.h"

#include "CustomGraphicsSegmentItem2.h"

class Route;

class GraphicsRouteItem: //public QObject, public QGraphicsItem {
                          public QGraphicsObject {

    Q_OBJECT

public:
    GraphicsRouteItem();
    GraphicsRouteItem(QGraphicsItem *parent=nullptr);
    ~GraphicsRouteItem() override;

    int getId();
    void resetId(int newId);

    void initFromRoute(const Route& r, qreal minimalElementSideSize);

    void adjustLocationAndSizeToZLI(int ZLI);

    void setRouteHightlighted(bool bShowAsHighlighted);

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int type() const override;

    void adjustColors_segment_unselectedPoint(const QColor& qCol_segmentColor, const QColor& qCol_sunselectPoint);

    void adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI);

    void debugShowStateAndLocation_aboutPoints();

public slots:
    void slot_customGraphicsTargetPointItemMoved(int idPoint, QPointF newPos);
    void slot_customGraphicsTargetPointItemEndOfMove(int idPoint/**/, QPointF newPosNoZLI/**/);

    void slot_pointHoverInOut(bool bHoverInOrOutstate, int idPoint);

    void slot_setGraphicsLocationOfPointForRoute(int idPoint, QPointF newPosNoZLI, int idRoute);

signals:
    void signal_updatePointOfRoute(int idPoint, QPointF qpfnewLocation, int idRoute);
    void signal_removePointOfRoute(int idPoint, int idRoute);

    void signal_pointHoverInOut_ownedByRoute(bool bHoverInOrOutstate, int idPoint, int idRoute);

protected:
    void computeBoundingBoxNoZLIFrom_ptrCGPointItem_addedToScene();

    void removeItemsFromSceneAndDeleteThem();

protected:
    int _id;

    int _currentZLI;

    QVector<CustomGraphicsTargetPointItem2*> _qvectCGTargetPointItem_addedToScene_ptr;
    QVector<CustomGraphicsSegmentItem2*> _qvectCGSegmentItem_addedToScene_ptr;

    QRectF _qrectf_boundingBoxNoZLI;
    QRectF _qrectf_boundingBox;

    QPen _p; //for boudingbox show

    bool _bShowAsHighlighted;

    bool _bShowBoundingRect;
};


#endif // GRAPHICSROUTEITEM_HPP
