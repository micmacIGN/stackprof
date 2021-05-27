#ifndef CUSTOMGRAPHICSBOXITEM2_HPP
#define CUSTOMGRAPHICSBOXITEM2_HPP

#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>


#include "../../core/ComputationCore_structures.h"

#include "../../appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"

struct S_ProfilsBoxParameters;

class CustomGraphicsBoxItem2: public QGraphicsObject {

    Q_OBJECT

public:

    CustomGraphicsBoxItem2(
        const S_ProfilsBoxParameters& profilsBoxParameters,
        int boxID,
        qreal devicePixelRatioF,
        QGraphicsItem *parent=nullptr);

    CustomGraphicsBoxItem2(const S_ProfilsBoxParameters& profilsBoxParameters,
                          int boxID,
                          Qt::GlobalColor colorForUnselected,
                          Qt::GlobalColor colorForSelected,
                          Qt::GlobalColor colorForHighlight,
                          qreal devicePixelRatioF,
                          QGraphicsItem *parent=nullptr);

    QPainterPath shape() const override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void adjustLocationAndSizeToZLI(int ZLI);

    int type() const override;

    ~CustomGraphicsBoxItem2() override;

    void setState_highlighted(bool bHighlight); //this is about current selection @#LP rename, to avoid amibigiguity with setSelected (which is for now about mouseHover)
    bool getState_highlighted();

    void setSelectable(bool bState);

    bool getState_hover();

    void adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI);

    int getBoxID();

signals:

    void signal_selectBox(int boxID);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:

    //void setDevicePixelRatioF(qreal devicePixelRatioF);

    void setSelected(bool bNewState); //this is about mouse hover @#LP rename

private:

    qreal _devicePixelRatioF;

    int _boxID;
    S_ProfilsBoxParameters _profilsBoxParametersNoZLI;

    int _currentZLI;

    QVector<QPointF> _qvectqpf_boxCornersNoZLI;
    QPointF _centerPointLocationZLI;

    QVector<QPointF> _qvectqpf_boxCornersZLI;

    //try with qpolygongraphictiem instead of
    //- rotate a rectangle inside a graphicsItem (transform, rotate, transform, => produce a bouding box but to late as we have to apply the ZLI)
    //- drawing the 4 segments of the rectangle inside a graphicsitem
    //The goal is to permit selection of the box when cliking in the oriented rectangle only,
    //improve accuracy of the drawing (not so nice drawing each segments)

    QPolygonF _polygonFNoZLI;
    QPolygonF _polygonFZLI;

    //QRectF _qrectf_boundingBoxNoZLI;
    QRectF _qrectf_boundingBoxNoZLI_noWidthBorder;
    QPainterPath _qPainterPath_shapeNoZLI;

    QRectF _qrectf_boundingBoxZLI;
    QPainterPath _qPainterPath_shapeZLI;

    bool _bHover; //mouse hover the shape

    bool _bHighlight; //this is about current selection @#LP rename, to avoid amibigiguity with setSelected (which is for now about mouseHover)

    bool _bSelectable;

    //
    QPen _pForUnselected;
    QPen _pForSelected;
    QPen *_PtrUsedPen; //this one point on one of the QPen available just above

    QPen _pForOverflowOnPrevious;
    QPen _pFor_tooCloseBox_rejected_needGoFurther;
    QPen _pFor_tooFarBox_rejected_needGoCloser;
    QPen _pForFinalAfterAdjustement;

    QPen _pForHighlight;

    qreal _widthBorderNoZLI;
    qreal _widthBorderZLI;
    bool _bUseAntialiasing;

    bool _bUseCosmeticPen;

    qreal _scaleOfViewSideForCurrentZLI;

};

#endif // CUSTOMGRAPHICSBOXITEM2_HPP
