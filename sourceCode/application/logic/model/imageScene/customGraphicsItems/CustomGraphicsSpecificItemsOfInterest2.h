#ifndef CustomGraphicsSpecificItemsOfInterest2_HPP
#define CustomGraphicsSpecificItemsOfInterest2_HPP

#include <QGraphicsItem>
#include <QDebug>

#include "CustomGraphicsTargetedPointItem2.h"
#include "CustomGraphicsTargetPointItem_noMouseInteraction_triangle2.h"

#include "../../core/route.h"

class CustomGraphicsSpecificItemsOfInterest2: public QGraphicsItem {

public:
    //GraphicsSpecificItemsOfInterest2();
    CustomGraphicsSpecificItemsOfInterest2(QGraphicsItem *parent=nullptr);
    ~CustomGraphicsSpecificItemsOfInterest2() override;

    void initFromRoute(const Route& routeForInit);

    void adjustLocationAndSizeToZLI(int ZLI);

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int type() const override;

    QRectF getBoundingRectNoZLI() const;

    void clearLocations();

    void debugShowStateAndLocations();

    void adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI);

private:
    void computeBoundingBoxNoZLI();
    void removeItemsFromSceneAndDeleteThem();   

private:
    int _currentZLI;

    QRectF _qrectf_boundingBoxNoZLI;
    QRectF _qrectf_boundingBox;

    //bool _bShowBoundingRect;

    //the GraphicsItem to highlight the first and last point of the route (for profil orientation or box distribution) is handlede outside the route
    bool _bFirstAndLastPointOfRouteHighlighted;
    CustomGraphicsTargetPointItem2 _cGTargetgetPoint_firstPointOfRoute;
    CustomGraphicsTargetPointItem_noMouseInteraction_triangle2 _cGTargetPoint_lastPointOfRoute;

    bool _bFeedWithValidLocations;

    QPen _qpen; //for boudingbox show

};

#endif // CustomGraphicsSpecificItemsOfInterest2_HPP
