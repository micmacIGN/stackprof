#ifndef CUSTOMGRAPHICSTARGETEDPOINTITEM2_HPP
#define CUSTOMGRAPHICSTARGETEDPOINTITEM2_HPP

#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

class CustomGraphicsTargetPointItem2: public QGraphicsObject {

    Q_OBJECT

public:
    //yellow/red default colors for unselected/selected
    CustomGraphicsTargetPointItem2(int idPoint,
                                  bool bCanBeMouseMoved, bool bCanBeHighlighted,
                                  QPointF location, qreal sideSizeNoZLI,
                                  QGraphicsItem *parent=nullptr);

    CustomGraphicsTargetPointItem2(int idPoint,
                                  bool bCanBeMouseMoved, bool bCanBeSelected,
                                  QPointF location, qreal sideSizeNoZLI,
                                  QColor QColor_unselected, QColor QColor_selected,
                                  QGraphicsItem *parent);

    void setDisplayState_doubleCircle_squareBox(bool bDoubleCircle, bool bSquareBox);

    QRectF boundingRect() const override ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void adjustLocationAndSizeToZLI(int ZLI);

    void adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI);

    void setHightlighted(bool bShowAsHighlighted);

    int type() const override;

    ~CustomGraphicsTargetPointItem2() override;

    QPointF get_qpointf_locationNoZLI() const;
    void set_qpointf_locationNoZLI(QPointF newPos);

    qreal BBoxWidth();
    qreal BBoxHeight();

    void setSideSizeNoZLI(double sideSizeNoZLI);

    QRectF getBoundingRectNoZLI() const;

    void debugShowStateAndLocation() const;

    void setColor_unselectedPoint(const QColor& qCol_segmentColor);

signals:
    void signal_customGraphicsTargetPointItemMoved(int idPoint, QPointF newPos);
    void signal_customGraphicsTargetPointItemEndOfMove(int idPoint/**/, QPointF newPos/**/);

    void signal_customGraphicsTargetPointItemRemove(int idPoint);

    void signal_pointHoverInOut(bool bHoverInOrOutstate, int idPoint);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:

    void computeBoundingBoxNoZLI();

    void setSelected(bool bNewState);
    void movePointFromMouseEvent(QGraphicsSceneMouseEvent *event);

    void endOfMovePointFromMouseEvent(QGraphicsSceneMouseEvent *event);

    void set_qpointf_locationInMove(QPointF newPos);

private:

    int _id;
    bool _bCanBeMouseMoved;
    bool _bCanBeSelected;

    bool _bDoubleCircle; //sued to show typically the point as the first point of the route
    bool _bSquareBox;

    int _currentZLI;

    QPointF _qpointf_locationNoZLI; //center of the shape: the point at ZLI = 1
    QSizeF _qsizef_sideSizeNoZLI;     //side size of the shape around the center

    QPen _pForUnselected;
    QPen _pForSelected;

    QRectF _qrectf_boundingBoxNoZLI;

    //boudingBox of the shape around the point, ZLI dependent
    QRectF _qrectf_boundingBoxZLI;

    QRectF _qrectf_locationAndSizeZLI; //location and size of the shape around the center and ZLI dependent
    QRectF _qrectf_locationAndSizeZLI_seconCircle;

    bool _bHover; //mouse hover the shape
    bool _bLocationInAdjustement;
    //bool _bDelKeyPressed;

    QPointF _offsetMouseClickToPointCenterPos; //stores the offset between the mouse cursor location when clicking for location adjustement and then point center

    QPen *_PtrUsedPen; //this one point on on of the QPen available just above

    QPointF _qpointf_locationInMove;

    bool _bShowAsHighlighted;

    qreal _widthBorderNoZLI;
    qreal _widthBorderZLI;

    bool _bUseCosmeticPen;
    bool _bUseAntialiasing;

    qreal _scaleOfViewSideForCurrentZLI;
};

#endif // CUSTOMGRAPHICSTARGETEDPOINTITEM2_HPP
