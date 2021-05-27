#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QPainter>

#include <QGraphicsSceneEvent>

#include "../S_booleanKeyStatus.hpp"

#include <qevent.h>

#include <QDebug>

#include "CustomGraphicsTargetedPointItem2.h"

#include "CustomGraphicsItemTypes.h"

CustomGraphicsTargetPointItem2::CustomGraphicsTargetPointItem2(int idPoint, bool bCanBeMouseMoved, bool bCanBeSelected, QPointF location, qreal sideSizeNoZLI, QGraphicsItem *parent):
    CustomGraphicsTargetPointItem2(idPoint,
                                  bCanBeMouseMoved, bCanBeSelected,
                                  location, sideSizeNoZLI,
                                  Qt::yellow, Qt::red,
                                  parent) {
}

CustomGraphicsTargetPointItem2::CustomGraphicsTargetPointItem2(int idPoint,
                                                             bool bCanBeMouseMoved, bool bCanBeSelected,
                                                             QPointF location, qreal sideSizeNoZLI,
                                                             QColor QColor_unselected, QColor QColor_selected,
                                                             QGraphicsItem *parent): QGraphicsObject(parent),
    _id(idPoint),

    _bCanBeMouseMoved(bCanBeMouseMoved),
    _bCanBeSelected(bCanBeSelected),

    _bDoubleCircle(false),
    _bSquareBox(true),

    _currentZLI(-1),

    _qpointf_locationNoZLI(location),
    _qsizef_sideSizeNoZLI{sideSizeNoZLI, sideSizeNoZLI},

    _pForUnselected(QColor_unselected),
    _pForSelected(QColor_selected),

    _qrectf_boundingBoxNoZLI{.0,.0,.0,.0},

    _qrectf_boundingBoxZLI{.0,.0,.0,.0},
    _qrectf_locationAndSizeZLI{.0,.0,.0,.0},
    _qrectf_locationAndSizeZLI_seconCircle{.0,.0,.0,.0},

    _bHover{false},
    _bLocationInAdjustement{false},

    _offsetMouseClickToPointCenterPos {.0,.0},
    _bShowAsHighlighted(false)
{

    _widthBorderNoZLI = 1.0;

    _bUseCosmeticPen = false; //true;
    _bUseAntialiasing = false;//!_bUseCosmeticPen;

    _pForUnselected.setStyle(Qt::PenStyle::SolidLine);
    _pForUnselected.setWidthF(_widthBorderNoZLI);
    _pForUnselected.setCosmetic(_bUseCosmeticPen);

    _pForSelected.setStyle(Qt::PenStyle::SolidLine);
    _pForSelected.setWidthF(_widthBorderNoZLI);
    _pForSelected.setCosmetic(_bUseCosmeticPen);

    setSelected(false);

    qDebug() << __FUNCTION__ << "_qsizef_sideSizeNoZLI =" << _qsizef_sideSizeNoZLI;

    _scaleOfViewSideForCurrentZLI = 1.0;
    adjustGraphicsItemWidth(_scaleOfViewSideForCurrentZLI);

    computeBoundingBoxNoZLI();   
    adjustLocationAndSizeToZLI(1);

    qDebug() << __FUNCTION__;

    _qpointf_locationInMove = {.0,.0};

    setAcceptHoverEvents(true);

    setVisible(true);

    qDebug() << __FUNCTION__ << "(end of constructor:)";
    debugShowStateAndLocation();

}

void CustomGraphicsTargetPointItem2::setDisplayState_doubleCircle_squareBox(bool bDoubleCircle, bool bSquareBox) {
    if (  (bDoubleCircle == _bDoubleCircle)
        &&(bSquareBox == _bSquareBox)) {
        return;
    }
    _bDoubleCircle = bDoubleCircle;
    _bSquareBox = bSquareBox;
    computeBoundingBoxNoZLI();
    adjustLocationAndSizeToZLI(_currentZLI);
}

void CustomGraphicsTargetPointItem2::computeBoundingBoxNoZLI() {

    qreal penWidthDiv2 = _widthBorderNoZLI/2.0;

    _qrectf_boundingBoxNoZLI.setTopLeft    ({_qpointf_locationNoZLI.x()-(_qsizef_sideSizeNoZLI.width() /2.0)-penWidthDiv2,
                                             _qpointf_locationNoZLI.y()-(_qsizef_sideSizeNoZLI.height()/2.0)-penWidthDiv2});

    _qrectf_boundingBoxNoZLI.setBottomRight({_qpointf_locationNoZLI.x()+(_qsizef_sideSizeNoZLI.width() /2.0)+penWidthDiv2,
                                             _qpointf_locationNoZLI.y()+(_qsizef_sideSizeNoZLI.height()/2.0)+penWidthDiv2});

    qDebug() << __FUNCTION__ << "(CustomGraphicsTargetPointItem2) _qrectf_boundingBoxNOZLI = " << _qrectf_boundingBoxNoZLI;

    qDebug() << __FUNCTION__ << "(end of): ";
    debugShowStateAndLocation();

}

void CustomGraphicsTargetPointItem2::adjustLocationAndSizeToZLI(int ZLI) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsTargetPointItem2) received ZLI = " << ZLI;

    _currentZLI = ZLI;

    QPointF qpointf_locationZLI = _qpointf_locationNoZLI/static_cast<qreal>(_currentZLI);
    QSizeF qsizef_sideSizeZLI   = _qsizef_sideSizeNoZLI/static_cast<qreal>(_currentZLI);


    qDebug() << __FUNCTION__ << "_qpointf_locationNoZLI = " << _qpointf_locationNoZLI;
    qDebug() << __FUNCTION__ << "_qsizef_sideSizeNoZLI   = " << _qsizef_sideSizeNoZLI;

    qDebug() << __FUNCTION__ << "qpointf_location = " << qpointf_locationZLI;
    qDebug() << __FUNCTION__ << "qsizef_sideSize   = " << qsizef_sideSizeZLI;

    qDebug() << __FUNCTION__ << "qsizef_sideSizeZLI " << qsizef_sideSizeZLI;

    //qDebug() << __FUNCTION__ << "_id: " << _id << " ; (CustomGraphicsTargetPointItem2) qpointf_location = " << qpointf_location;

    /*
    .-------.
    |       |
    |   +   |
    |       |
    .-------.

    + = center
    side size = width = height of the shape

    */

    qreal widthDiv2  = static_cast<qreal>(qsizef_sideSizeZLI.width()/2.0);
    qreal heightDiv2 = static_cast<qreal>(qsizef_sideSizeZLI.height()/2.0);

    _qrectf_locationAndSizeZLI.setLeft  (qpointf_locationZLI.x()-widthDiv2);
    _qrectf_locationAndSizeZLI.setRight (qpointf_locationZLI.x()+widthDiv2);

    _qrectf_locationAndSizeZLI.setTop   (qpointf_locationZLI.y()-heightDiv2);
    _qrectf_locationAndSizeZLI.setBottom(qpointf_locationZLI.y()+heightDiv2);

    qDebug() << __FUNCTION__ << "(CustomGraphicsTargetPointItem2) _qrectf_locationAndSizeZLI top left bottom right = " <<
                _qrectf_locationAndSizeZLI.top() <<
                _qrectf_locationAndSizeZLI.left() <<
                _qrectf_locationAndSizeZLI.bottom() <<
                _qrectf_locationAndSizeZLI.right();


    if (_bDoubleCircle) {
        qreal widthDiv2DeuxTiers  = widthDiv2  * (2.0/3.0);
        qreal heightDiv2DeuxTiers = heightDiv2 * (2.0/3.0);
        _qrectf_locationAndSizeZLI_seconCircle.setLeft  (qpointf_locationZLI.x()-widthDiv2DeuxTiers);
        _qrectf_locationAndSizeZLI_seconCircle.setRight (qpointf_locationZLI.x()+widthDiv2DeuxTiers);
        _qrectf_locationAndSizeZLI_seconCircle.setTop   (qpointf_locationZLI.y()-heightDiv2DeuxTiers);
        _qrectf_locationAndSizeZLI_seconCircle.setBottom(qpointf_locationZLI.y()+heightDiv2DeuxTiers);
    }

    //qDebug() << __FUNCTION__ << "_id: " << _id << " ; (CustomGraphicsTargetPointItem2) _qrectf_locationAndSize = " << _qrectf_locationAndSize;

    prepareGeometryChange();

    qreal penWidthDiv2 = _widthBorderZLI/2.0; //1.0/2.0;

    qDebug() << __FUNCTION__ << "(CustomGraphicsTargetPointItem2) penWidthDiv2 = " << penWidthDiv2;

    //int extraSizeBorderBoundingBox = 0; // 1.5 /* ZLI /* scale
    _qrectf_boundingBoxZLI.setTop   (_qrectf_locationAndSizeZLI.top ()  -penWidthDiv2);
    _qrectf_boundingBoxZLI.setLeft  (_qrectf_locationAndSizeZLI.left()  -penWidthDiv2);
    _qrectf_boundingBoxZLI.setBottom(_qrectf_locationAndSizeZLI.bottom()+penWidthDiv2);
    _qrectf_boundingBoxZLI.setRight (_qrectf_locationAndSizeZLI.right() +penWidthDiv2);

    qDebug() << __FUNCTION__ << "(CustomGraphicsTargetPointItem2) _qrectf_boundingBoxZLI top left bottom right = " <<
                _qrectf_boundingBoxZLI.top() <<
                _qrectf_boundingBoxZLI.left() <<
                _qrectf_boundingBoxZLI.bottom() <<
                _qrectf_boundingBoxZLI.right();

    qDebug() << __FUNCTION__ << "(CustomGraphicsTargetPointItem2) _qrectf_boundingBoxZLI width height = " <<
                _qrectf_boundingBoxZLI.width() << _qrectf_boundingBoxZLI.height();

    qDebug() << __FUNCTION__ << "(end of): ";
    debugShowStateAndLocation();
}

QRectF CustomGraphicsTargetPointItem2::boundingRect() const {
    //qDebug() << __FUNCTION__;
    return(_qrectf_boundingBoxZLI);
}

QRectF CustomGraphicsTargetPointItem2::getBoundingRectNoZLI() const {
    return(_qrectf_boundingBoxNoZLI);
}

void CustomGraphicsTargetPointItem2::debugShowStateAndLocation() const {
    qDebug() << __FUNCTION__ << "_id: " << _id;
    qDebug() << __FUNCTION__ << "_bCanBeMouseMoved: " << _bCanBeMouseMoved;
    qDebug() << __FUNCTION__ << "_bCanBeSelected: " << _bCanBeSelected;
    qDebug() << __FUNCTION__ << "_bSquareBox: " << _bSquareBox;
    qDebug() << __FUNCTION__ << "_bDoubleCircle: " << _bDoubleCircle;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_qrectf_locationAndSizeZLI: " << _qrectf_locationAndSizeZLI;
    qDebug() << __FUNCTION__ << "_qpointf_locationNoZLI: " << _qpointf_locationNoZLI;
    qDebug() << __FUNCTION__ << "_qsizef_sideSizeNoZLI: " << _qsizef_sideSizeNoZLI;
    qDebug() << __FUNCTION__ << "_qrectf_boundingBoxNoZLI: " << _qrectf_boundingBoxNoZLI;
    qDebug() << __FUNCTION__ << "_qrectf_boundingBoxZLI: " << _qrectf_boundingBoxZLI;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_qrectf_locationAndSizeZLI_seconCircle: " << _qrectf_locationAndSizeZLI_seconCircle;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_currentZLI: " << _currentZLI;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_qrectf_boundingBoxZLI: " << _qrectf_boundingBoxZLI;
    qDebug() << __FUNCTION__ << "_bShowAsHighlighted: " << _bShowAsHighlighted;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "_widthBorderNoZLI:" << _widthBorderNoZLI;
    qDebug() << __FUNCTION__ << "_widthBorderZLI  :" << _widthBorderZLI;
    qDebug() << __FUNCTION__ << "_scaleOfViewSideForCurrentZLI:" << _scaleOfViewSideForCurrentZLI;
    qDebug() << __FUNCTION__ << "_bUseCosmeticPen :" << _bUseCosmeticPen;
    qDebug() << __FUNCTION__ << "_bUseAntialiasing:" << _bUseAntialiasing;
}


void CustomGraphicsTargetPointItem2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    qDebug() << __FUNCTION__ << "_id: " << _id << " ; _qrectf_locationAndSize:" << _qrectf_locationAndSizeZLI;
    qDebug() << __FUNCTION__ << "_id: " << _id << " ; _qrectf_boundingBoxZLI:" << _qrectf_boundingBoxZLI;


    painter->save();

    /*//---@#LP debug
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


    painter->setPen(*_PtrUsedPen);

    if (_bSquareBox) {
        painter->drawRect(_qrectf_locationAndSizeZLI);
    }

    painter->drawEllipse(/*_qrectf_boundingBox*/_qrectf_locationAndSizeZLI);

    if (_bDoubleCircle) {
        painter->drawEllipse(_qrectf_locationAndSizeZLI_seconCircle);
    }

    /*if (_bHover) {
        painter->setRenderHints(QPainter::Antialiasing);
        //painter->drawEllipse(_qrectf_locationAndSize);
    }*/

    painter->restore();

}

int CustomGraphicsTargetPointItem2::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return(e_customGraphicsItemType::e_cgit_CustomGraphicsTargetPointItem);
}

CustomGraphicsTargetPointItem2::~CustomGraphicsTargetPointItem2() {
   qDebug() << "CustomGraphicsTargetPointItem::~CustomGraphicsTargetPointItem " << "_id = " << _id;
}

void CustomGraphicsTargetPointItem2::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << __FUNCTION__ << " CustomGraphicsTargetPointItem2 *-* ";

    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    bool bLeftButtonPressed = event->button() == Qt::LeftButton;
    if (!bLeftButtonPressed) {
        qDebug() << __FUNCTION__ << "!bLeftButtonPressed";
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    qDebug() << __FUNCTION__ << "entering location in adjustement";
    _bLocationInAdjustement = true;

    if (_bLocationInAdjustement) {

        event->accept();

        _offsetMouseClickToPointCenterPos = (event->scenePos()) - _qpointf_locationNoZLI/static_cast<qreal>(_currentZLI);

        qDebug() << __FUNCTION__ << "offsetMouseClickToPointCenter = " << _offsetMouseClickToPointCenterPos;
        return;
    }

    QGraphicsItem::mousePressEvent(event);

}

void CustomGraphicsTargetPointItem2::set_qpointf_locationInMove(QPointF newPos) {
    qDebug() << __FUNCTION__ << "_id: " << _id << " ;  newPos received:" << newPos;
    _qpointf_locationInMove = newPos;
}

void CustomGraphicsTargetPointItem2::set_qpointf_locationNoZLI(QPointF newPos) {
    qDebug() << __FUNCTION__ << "_id: " << _id << " ; newPos received:" << newPos;
    _qpointf_locationNoZLI = newPos * static_cast<qreal>(_currentZLI);
    qDebug() << __FUNCTION__ << "_qpointf_locationNoZLI become: " << _qpointf_locationNoZLI;
    computeBoundingBoxNoZLI(); //was missing ?
    adjustLocationAndSizeToZLI(_currentZLI);
}


QPointF CustomGraphicsTargetPointItem2::get_qpointf_locationNoZLI() const {
    qDebug() << __FUNCTION__ << "_id: " << _id << " ; CustomGraphicsTargetPointItem   _qpointf_locationNoZLI = " << _qpointf_locationNoZLI;
    return(_qpointf_locationNoZLI);
}


void CustomGraphicsTargetPointItem2::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    qDebug() << __FUNCTION__ << "_id: " << _id << " ; CustomGraphicsTargetPixelItem";

    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    if (_bLocationInAdjustement) {
        movePointFromMouseEvent(event);
        event->accept();
    }

}

void CustomGraphicsTargetPointItem2::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

    if (!_bCanBeMouseMoved) {
        QGraphicsItem::mouseReleaseEvent(event);
        return;
    }

    qDebug() << __FUNCTION__ << "_id: " << _id << " ; CustomGraphicsTargetPixelItem";
    if (event->button() == Qt::LeftButton) {
        if (_bLocationInAdjustement) {

            _bLocationInAdjustement = false;

            movePointFromMouseEvent(event);

            endOfMovePointFromMouseEvent(event);//do not loose the last location when shaking the mouse

            event->accept();
        }
    }
}

//try this to find segment close to the mouse cursor:
//in scene:
//QGraphicsItem *item = scene><itemAt(mapToScene(event->pos()), QTransform());

qreal CustomGraphicsTargetPointItem2::BBoxWidth() {
    qDebug() << __FUNCTION__ << "_qrectf_boundingBox.width() = " << _qrectf_boundingBoxZLI.width();
    return(_qrectf_boundingBoxZLI.width());
}

qreal CustomGraphicsTargetPointItem2::BBoxHeight() {
    qDebug() << __FUNCTION__ << "_qrectf_boundingBox.height() = " << _qrectf_boundingBoxZLI.height();
    return(_qrectf_boundingBoxZLI.height());
}

void CustomGraphicsTargetPointItem2::movePointFromMouseEvent(QGraphicsSceneMouseEvent *event) {

    QPointF qpEventScenePos = event->scenePos();
    qDebug() << __FUNCTION__ << "_id: " << _id << " ; qpEventScenePos: " << qpEventScenePos;

    qDebug() << __FUNCTION__ << "__offsetMouseClickToPointCenterPos:" << _offsetMouseClickToPointCenterPos;

    qpEventScenePos -= _offsetMouseClickToPointCenterPos;
    qDebug() << __FUNCTION__ << "_id: " << _id << " ; remove mouse offset to qpEventScenePos:" << qpEventScenePos;

    QPointF qpEventScenePosNoZLI = qpEventScenePos * static_cast<qreal>(_currentZLI);
    qDebug()<< __FUNCTION__ << "_id: " << _id << " ; qpEventScenePosNoZLI: " << qpEventScenePosNoZLI;

    QPointF deltaNoZLI = qpEventScenePosNoZLI - _qpointf_locationNoZLI;
    qDebug()<< __FUNCTION__ << "_id: " << _id << " ; deltaNoZLI: " << deltaNoZLI;

    QPointF newPosWithZLI = deltaNoZLI/static_cast<qreal>(_currentZLI);
    setPos(newPosWithZLI);
    qDebug() << __FUNCTION__ << "_id: " << _id << " ; setPos(newPosWithZLI =: " << newPosWithZLI;

    set_qpointf_locationInMove(qpEventScenePos); //try to avoid update rect, setting only the loc and avoid update rect back from slot_customGraphicsTargetPointItemMoved

    //update the according graphicsSegmentItem(s)
    //@#LP trying to solve the noisy position
    //the noisy position bug occurs using this:
    //. moving the targeted (mouse press & mouse drag)
    //. setting _qpointf_locationNoZLI at each move
    //. getting the point location _qpointf_locationNoZLI from the route item

    //solved; storing the current in movement new position, getting this information to the segment item and altering the model only at end of mouse move

    emit signal_customGraphicsTargetPointItemMoved(_id, qpEventScenePos);

}

void CustomGraphicsTargetPointItem2::endOfMovePointFromMouseEvent(QGraphicsSceneMouseEvent *event) {

    qDebug() << __FUNCTION__ << "_id: " << _id << " ;  _qpointf_locationInMove=" << _qpointf_locationInMove;
    setPos(.0,.0);//because mouse move used setPos() to set the relative position (in scene coordinates), the position needs to be reset
    set_qpointf_locationNoZLI(_qpointf_locationInMove);

    //qDebug() << __FUNCTION__ << "_id: " << _id << " ;  _qpointf_locationInMove=" << _qpointf_locationInMove;

    emit signal_customGraphicsTargetPointItemEndOfMove(_id, _qpointf_locationNoZLI);

}

void CustomGraphicsTargetPointItem2::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    qDebug() << __FUNCTION__;
    if (!_bCanBeSelected) {
        QGraphicsItem::hoverEnterEvent(event);
        return;
    }
    setSelected(true);
    event->accept();
}

void CustomGraphicsTargetPointItem2::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    qDebug() << __FUNCTION__;    
    if (!_bCanBeSelected) {
        QGraphicsItem::hoverLeaveEvent(event);
        return;
    }
    //if (!_bLocationInAdjustement) { //because it's posible to drag the line out the window with the selected line (mouse keep pressed)
    setSelected(false);
    //}
    event->accept();
}

void CustomGraphicsTargetPointItem2::setHightlighted(bool bShowAsHighlighted) {

    _bShowAsHighlighted = bShowAsHighlighted;
    if (_bShowAsHighlighted) {
       show();
    } else {
       hide();
    }
}

void CustomGraphicsTargetPointItem2::setColor_unselectedPoint(const QColor& qCol_segmentColor) {
    _pForUnselected.setColor(qCol_segmentColor);
    update();
}

void CustomGraphicsTargetPointItem2::setSelected(bool bNewState) {
    if (_bHover != bNewState) {
        emit signal_pointHoverInOut(bNewState, _id);
    }
    _bHover = bNewState;
    if (_bHover) {
        _PtrUsedPen = &_pForSelected;
    } else {
        _PtrUsedPen = &_pForUnselected;
    }
    update();
}


void CustomGraphicsTargetPointItem2::setSideSizeNoZLI(double sideSizeNoZLI) {
    if (sideSizeNoZLI < 0.1) {
        sideSizeNoZLI = 0.1;
    }
    _qsizef_sideSizeNoZLI = { sideSizeNoZLI, sideSizeNoZLI };
    computeBoundingBoxNoZLI(); //was missing ?
    adjustLocationAndSizeToZLI(_currentZLI);
}



void CustomGraphicsTargetPointItem2::adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI) {

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

    _pForSelected.setWidthF(_widthBorderZLI);
    _pForSelected.setCosmetic(_bUseCosmeticPen);

}
