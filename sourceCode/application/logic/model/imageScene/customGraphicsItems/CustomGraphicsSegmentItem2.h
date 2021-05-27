#ifndef CUSTOMGRAPHICSSEGMENT2_H
#define CUSTOMGRAPHICSSEGMENT2_H

#include <QGraphicsLineItem>
#include <QPen>
#include <QColor>
#include <QBrush>

class CustomGraphicsSegmentItem2: public QGraphicsItem {

public:
    CustomGraphicsSegmentItem2(int idSegment,
                              QPointF pointA_noZLI, QPointF pointB_noZLI,
                              Qt::GlobalColor colorHightlighted,
                              Qt::GlobalColor colorNotHightlighted,
                              QGraphicsItem *parent=nullptr);

    void adjustLocationAndSizeToZLI(int ZLI);

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    //QRectF getBoundingRectNoZLI() const;


    //QPainterPath shape() const override ;
    //QRectF boundingRect() const override;

    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;

    void setHightlighted(bool bShowAsHighlighted);

    ~CustomGraphicsSegmentItem2() override;

    int type() const override;

    void setPointAnoZLI_andUpdate(const QPointF& qptfA);
    void setPointBnoZLI_andUpdate(const QPointF& qptfB);

    void setColorNotHighlighted_Highlighted(const QColor &qCol_notHighlighted, const QColor &qCol_highlighted);

    void adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI);

protected:
    void setPen(QPen *ptrQPenToUse);
    void computeBoundingBoxNoZLI();


private:
    int _id;
    QPointF _pointAB_noZLI[2];
    QPointF _pointAB[2];

    QPen _pHighlighted;
    QPen _pNotHighlighted;
    QPen *_ptrUsedPen; //this one point on one of the QPen available just above

    int _currentZLI;

    bool _bShowAsHighlighted;

    QRectF _qrectf_boundingBoxNoZLI;
    QRectF _qrectf_boundingBox;

    bool _bShowBoundingRect;

    qreal _widthBorderNoZLI;
    qreal _widthBorderZLI;
    bool _bUseAntialiasing;

    bool _bUseCosmeticPen;

    qreal _scaleOfViewSideForCurrentZLI;
};

#endif // CUSTOMGRAPHICSSEGMENT2_H
