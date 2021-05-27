#ifndef CustomGraphicsTinyTargetPointItem_notDragrable2_HPP
#define CustomGraphicsTinyTargetPointItem_notDragrable2_HPP

#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

class CustomGraphicsTinyTargetPointItem_notDragrable2: public QGraphicsObject { //public QObject, public QGraphicsItem {

    Q_OBJECT

public:
    CustomGraphicsTinyTargetPointItem_notDragrable2(int idPoint,
                                                   QPointF location, qreal sideSizeNoZLI,
                                                   Qt::GlobalColor colorForUnselected, Qt::GlobalColor colorForSelected,
                                                   bool bVisible, QGraphicsItem *parent=nullptr);

    QRectF boundingRect() const override ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void adjustLocationAndSizeToZLI(int ZLI);
    void adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI);

    int type() const override;

    ~CustomGraphicsTinyTargetPointItem_notDragrable2() override;

    void setSize(qreal sideSizeNoZLI);
    void setLocation(QPointF location);

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
    QRectF _qrectf_boundingBoxZLI;

    bool _bHover; //mouse hover the shape

    //
    QPen _pForUnselectedPixel;
    QPen _pForSelectedPixel;
    QPen *_PtrUsedPen; //this one point on one of the QPen available just above

    bool _bVisible;


    qreal _widthBorderNoZLI;
    qreal _widthBorderZLI;

    bool _bUseCosmeticPen;
    bool _bUseAntialiasing;

    qreal _scaleOfViewSideForCurrentZLI;

};

#endif // CustomGraphicsTinyTargetPointItem_notDragrable2_HPP
