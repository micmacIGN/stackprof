#ifndef CustomGraphicsTargetPointItem_noMouseInteraction_triangle2_H
#define CustomGraphicsTargetPointItem_noMouseInteraction_triangle2_H

#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

class CustomGraphicsTargetPointItem_noMouseInteraction_triangle2: public QGraphicsItem {

public:
    CustomGraphicsTargetPointItem_noMouseInteraction_triangle2(int idPoint, QPointF location, qreal sideSizeNoZLI, QGraphicsItem *parent=nullptr);

    QRectF boundingRect() const override ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void adjustLocationAndSizeToZLI(int ZLI); //try a way to void noisy movement of the rect around the point

    int type() const override;

    ~CustomGraphicsTargetPointItem_noMouseInteraction_triangle2() override;

    void set_qpointf_locationNoZLI(QPointF newPos);

    void setSideSizeNoZLI(double sideSizeNoZLI);

    QRectF getBoundingRectNoZLI_withWidthBorder() const;

    void debugShowStateAndLocation() const;

    void adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI);

signals:

protected:

private:

    void computeBoundingBoxNoZLI();

    void setSelected(bool bNewState);

private:

    int _id;

    int _currentZLI;

    QPointF _qpointf_locationNoZLI; //center of the shape: the point at ZLI = 1
    QSizeF _qsizef_sideSizeNoZLI;     //side size of the shape around the center

    QRectF _qrectf_locationAndSize; //location and size of the shape around the center and ZLI dependent

    QRectF _qrectf_boundingBoxNoZLI_withWidthBorder;

    //boudingBox of the shape around the point, ZLI dependent
    QRectF _qrectf_boundingBoxZLI;

    //
    QPen _pForUnselected;

    QPen *_ptrUsedPen; //this one point on one of the QPen available just above

    QVector<QPointF> _qvectPointsForTriangleShape;
    QVector<QPointF> _qvectPointsForTriangleInsideleShape;

    bool _bShowAsHighlighted;

    qreal _widthBorderNoZLI;
    qreal _widthBorderZLI;
    bool _bUseAntialiasing;

    bool _bUseCosmeticPen;

    qreal _scaleOfViewSideForCurrentZLI;

};

#endif // CustomGraphicsTargetPointItem_noMouseInteraction_triangle2_H
