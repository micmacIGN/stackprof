#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QGraphicsSceneEvent>

#include "../S_booleanKeyStatus.hpp"

#include <qevent.h>

#include <QDebug>

#include "CustomGraphicsTargetPointItem_noMouseInteraction_triangle2.h"

#include "CustomGraphicsItemTypes.h"

CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::CustomGraphicsTargetPointItem_noMouseInteraction_triangle2(
        int idPoint,
        QPointF location,
        qreal sideSizeNoZLI,
        QGraphicsItem *parent): QGraphicsItem(parent), ////QGraphicsObject(parent),
    _id(idPoint),

    _currentZLI(-1),
    _qpointf_locationNoZLI(location),
    _qsizef_sideSizeNoZLI{sideSizeNoZLI, sideSizeNoZLI},
    _qrectf_locationAndSize{.0,.0,.0,.0},
    _qrectf_boundingBoxNoZLI_withWidthBorder{.0,.0,.0,.0},

    _qrectf_boundingBoxZLI{.0,.0,.0,.0},

    _bShowAsHighlighted(false)
{


    qDebug() << __FUNCTION__ << "_qsizef_sideSizeNoZLI =" << _qsizef_sideSizeNoZLI;

//@LP--passing to v2
    _widthBorderNoZLI = 1.0;
    _bUseCosmeticPen = false; //true;
    _bUseAntialiasing = !_bUseCosmeticPen;


    _pForUnselected.setStyle(Qt::PenStyle::SolidLine);
    _pForUnselected.setColor(Qt::red/*blue*/);
    _pForUnselected.setCosmetic(_bUseCosmeticPen);

    _ptrUsedPen = &_pForUnselected;


    //--- passing to 2
    /*    _qrectf_boundingBoxNoZLI_noWidthBorder.setTop   (yTopNoZLI   );
        _qrectf_boundingBoxNoZLI_noWidthBorder.setBottom(yBottomNoZLI);
        _qrectf_boundingBoxNoZLI_noWidthBorder.setLeft  (xLeftNoZLI  );
        _qrectf_boundingBoxNoZLI_noWidthBorder.setRight (xRightNoZLI );
    #endif*/

    computeBoundingBoxNoZLI();

    setAcceptHoverEvents(true);

    setVisible(true);

    _scaleOfViewSideForCurrentZLI = 1.0;
    adjustGraphicsItemWidth(_scaleOfViewSideForCurrentZLI);

    adjustLocationAndSizeToZLI(1);

//@LP--passing to v2

}

void CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::adjustLocationAndSizeToZLI(int ZLI) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsTargetPointItem_noMouseInteraction_triangle) received ZLI = " << ZLI;

    _currentZLI = ZLI;

    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) " << _currentZLI;

    QPointF qpointf_location = _qpointf_locationNoZLI/static_cast<qreal>(_currentZLI);
    QSizeF qsizef_sideSize   = _qsizef_sideSizeNoZLI/static_cast<qreal>(_currentZLI);

    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) qsizef_sideSize " << qsizef_sideSize;

    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) qpointf_location = " << qpointf_location;

    /*
    .-------.
    | \   / |
    |  \ /  |
    .___v___.
        ^
    given location point is located at down triangle corner
    side size = width = height of the bbox shape

    */

    qreal widthDiv2 = static_cast<qreal>(qsizef_sideSize.width()/2.0);
    _qvectPointsForTriangleShape.clear();
    _qvectPointsForTriangleShape.push_back({ qpointf_location.x()            , qpointf_location.y()});//bottom triangle point //bottom center
    _qvectPointsForTriangleShape.push_back({ qpointf_location.x() - widthDiv2, qpointf_location.y() - qsizef_sideSize.height()}); //left top triangle point
    _qvectPointsForTriangleShape.push_back({ qpointf_location.x() + widthDiv2, qpointf_location.y() - qsizef_sideSize.height()}); //right top triangle point

    _qrectf_locationAndSize.setLeft  (_qvectPointsForTriangleShape[1].x());
    _qrectf_locationAndSize.setRight (_qvectPointsForTriangleShape[2].x());


    _qrectf_locationAndSize.setTop   (_qvectPointsForTriangleShape[1].y());
    _qrectf_locationAndSize.setBottom(_qvectPointsForTriangleShape[0].y());

    //second triangle inside the first

    qreal width_4dot5Div16  = qsizef_sideSize.width() * (4.5 / 16.0);
    qreal heightDiv8 = static_cast<qreal>(qsizef_sideSize.height()/8.0);
    _qvectPointsForTriangleInsideleShape.clear();
    _qvectPointsForTriangleInsideleShape.push_back({qpointf_location.x()                   , qpointf_location.y() - (qsizef_sideSize.height() * (5.0 / 16.0))});
    _qvectPointsForTriangleInsideleShape.push_back({qpointf_location.x() - width_4dot5Div16, qpointf_location.y() - (qsizef_sideSize.height() - heightDiv8)});
    _qvectPointsForTriangleInsideleShape.push_back({qpointf_location.x() + width_4dot5Div16, qpointf_location.y() - (qsizef_sideSize.height() - heightDiv8)});

    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) _qvectPointsForTriangleShape: " << _qvectPointsForTriangleShape;
    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) _qvectPointsForTriangleInsideleShape: " << _qvectPointsForTriangleInsideleShape;

    prepareGeometryChange();

    qreal penWidthDiv2 = _widthBorderZLI/2.0; //.0; //1.0/2.0;

    _qrectf_boundingBoxZLI.setTop   (_qrectf_locationAndSize.top ()  -penWidthDiv2);
    _qrectf_boundingBoxZLI.setLeft  (_qrectf_locationAndSize.left()  -penWidthDiv2);
    _qrectf_boundingBoxZLI.setBottom(_qrectf_locationAndSize.bottom()+penWidthDiv2);
    _qrectf_boundingBoxZLI.setRight (_qrectf_locationAndSize.right() +penWidthDiv2);

    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) _qpointf_locationNoZLI = " << _qpointf_locationNoZLI;
    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) _qsizef_sideSizeNoZLI   = " << _qsizef_sideSizeNoZLI;

    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) qpointf_location = " << qpointf_location;
    qDebug() << __FUNCTION__ << "(CGTPI_nMI_triangle) qsizef_sideSize   = " << qsizef_sideSize;

}

QRectF CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::boundingRect() const {
    //qDebug() << __FUNCTION__;
    return(_qrectf_boundingBoxZLI);
}

void CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    //qDebug() << __FUNCTION__ << "_id: " << _id << " ; _qrectf_locationAndSize:" << _qrectf_locationAndSize;

    painter->save();

    //---@#LP debug
    /*
    QBrush brush_debug(Qt::SolidPattern);
    //brush_debug.setColor({0,128,0,128});
    brush_debug.setColor({255,255,0,128});
    painter->setBrush(brush_debug);
    painter->setPen(Qt::PenStyle::NoPen);
    painter->drawRect(static_cast<QRectF>(
        _qrectf_boundingBoxZLI));
        //_qrectf_locationAndSizeZLI));
    //---
    */

    painter->setPen(*_ptrUsedPen);
    painter->drawPolygon(_qvectPointsForTriangleShape.data(), 3);
    painter->drawPolygon(_qvectPointsForTriangleInsideleShape.data(), 3);

    painter->restore();
}

int CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType::e_cgit_CustomGraphicsTargetPointItem_noMouseInteraction_triangle);
}

CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::~CustomGraphicsTargetPointItem_noMouseInteraction_triangle2() {
   qDebug() << "CustomGraphicsTargetPointItem::~CustomGraphicsTargetPointItem " << "_id = " << _id;
}

void CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::computeBoundingBoxNoZLI() {

    qreal penWidthDiv2 = _widthBorderNoZLI/2.0;

    /*
    .-------.
    | \   / |
    |  \ /  |
    .___v___.
        ^
    given location point is located at down triangle corner
    side size = width = height of the bbox shape
    */

    _qrectf_boundingBoxNoZLI_withWidthBorder.setTopLeft    ({_qpointf_locationNoZLI.x()-(_qsizef_sideSizeNoZLI.width()/2.0)-penWidthDiv2,
                                                           _qpointf_locationNoZLI.y()-_qsizef_sideSizeNoZLI.height()-penWidthDiv2});

    _qrectf_boundingBoxNoZLI_withWidthBorder.setBottomRight({_qpointf_locationNoZLI.x()+(_qsizef_sideSizeNoZLI.width()/2.0)+penWidthDiv2,
                                                           _qpointf_locationNoZLI.y()+penWidthDiv2});

    qDebug() << __FUNCTION__ << "(CustomGraphicsTargetPointItem_noMouseInteraction_triangle) _qrectf_boundingBoxNoZLI_withWidthBorder = "
             << _qrectf_boundingBoxNoZLI_withWidthBorder;
}

void CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::set_qpointf_locationNoZLI(QPointF newPos) {
    qDebug() << __FUNCTION__ << "_id: " << _id << " ; newPos received:" << newPos;
    _qpointf_locationNoZLI = newPos * static_cast<qreal>(_currentZLI);

    computeBoundingBoxNoZLI();
    adjustLocationAndSizeToZLI(_currentZLI);
}

void CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::setSideSizeNoZLI(double sideSizeNoZLI) {
    if (sideSizeNoZLI < 0.1) {
        sideSizeNoZLI = 0.1;
    }
    _qsizef_sideSizeNoZLI = { sideSizeNoZLI, sideSizeNoZLI };

    computeBoundingBoxNoZLI();
    adjustLocationAndSizeToZLI(_currentZLI);
}

QRectF CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::getBoundingRectNoZLI_withWidthBorder() const {
    return(_qrectf_boundingBoxNoZLI_withWidthBorder);
}

void CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::debugShowStateAndLocation() const {
    qDebug() << __FUNCTION__ << "_id: " << _id;
    qDebug() << __FUNCTION__ << "_qrectf_locationAndSize: " << _qrectf_locationAndSize;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_qpointf_locationNoZLI: " << _qpointf_locationNoZLI;
    qDebug() << __FUNCTION__ << "_qsizef_sideSizeNoZLI: " << _qsizef_sideSizeNoZLI;
    qDebug() << __FUNCTION__ << "_qrectf_boundingBoxNoZLI_withWidthBorder: " << _qrectf_boundingBoxNoZLI_withWidthBorder;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_currentZLI: " << _currentZLI;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_qrectf_boundingBoxZLI: " << _qrectf_boundingBoxZLI;
    qDebug() << __FUNCTION__ << "_bShowAsHighlighted: " << _bShowAsHighlighted;
}

void CustomGraphicsTargetPointItem_noMouseInteraction_triangle2::adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI) {

    prepareGeometryChange(); //@#LP best location ?

    _scaleOfViewSideForCurrentZLI = scaleOfViewSideForCurrentZLI;

    if (_bUseCosmeticPen) {
        _widthBorderZLI = 1.0;
    } else {
        _widthBorderZLI = _widthBorderNoZLI / _scaleOfViewSideForCurrentZLI;
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _scaleOfViewSideForCurrentZLI: " << _scaleOfViewSideForCurrentZLI;
    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _widthBorderNoZLI: " << _widthBorderNoZLI;
    qDebug() << __FUNCTION__ << "(CustomGraphicsBoxItem2) _widthBorderZLI => " << _widthBorderZLI;

//#define DEF_LP_debug_trace_file_cgbox
#ifdef DEF_LP_debug_trace_file_cgbox

    QString qstrDbg0 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) ---- start -----\n\r";
    QString qstrDbg1 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _currentZLI: " + QString::number(_currentZLI) + "\n\r";
    QString qstrDbg2 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _scaleOfViewSideForCurrentZLI: " + QString::number(_scaleOfViewSideForCurrentZLI) + "\n\r";
    QString qstrDbg3 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _widthBorderNoZLI: " + QString::number(_widthBorderNoZLI) + "\n\r";
    QString qstrDbg4 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) _widthBorderZLI: " + QString::number(_widthBorderZLI) + "\n\r";
    QString qstrDbg5 = QString(__FUNCTION__) + "(CustomGraphicsBoxItem2) ---- end -----\n\r";


    QFile qfdebug("/tmp/cgbox_log.txt");
    if (qfdebug.open(QIODevice::Append)) {
        qfdebug.write(qstrDbg0.toStdString().c_str());
        qfdebug.write(qstrDbg1.toStdString().c_str());
        qfdebug.write(qstrDbg2.toStdString().c_str());
        qfdebug.write(qstrDbg3.toStdString().c_str());
        qfdebug.write(qstrDbg4.toStdString().c_str());
        qfdebug.write(qstrDbg5.toStdString().c_str());
        qfdebug.close();
    }
#endif

    //bool bUseCosmeticPen = true; //false;

    _pForUnselected.setWidthF(_widthBorderZLI);
    _pForUnselected.setCosmetic(_bUseCosmeticPen);

}
