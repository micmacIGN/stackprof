#include <QDebug>


#include <QPainter>

#include "CustomGraphicsItemVerticalAdjuster_withAdjustableWithElements.h"

#include "CustomGraphicsItemType_v2.h"

#include "CustomLineMouseOver.h"
#include "CustomGraphicsItem_coloredGripSquare.h"

#include "../../../core/leftRight_sideAccess.hpp"

CustomGraphicsItemVerticalAdjuster::CustomGraphicsItemVerticalAdjuster(
        double xpos,
        double xrange_minValue, double xrange_maxValue,
        double additionnal_xlimit,
        e_LRsideAccess eLRSA_handlerAtSide,
        double lineHeight,
        bool bCanBeMouseMoved,
        double handlerWidth, double handlerHeight,
        QGraphicsItem *parent): /*QGraphicsItem(parent)*/QGraphicsObject(parent),

    _xpos_qcs(xpos),
    _xrange_minValue_qcs(xrange_minValue),
    _xrange_maxValue_qcs(xrange_maxValue),
    _additionnal_xlimit_qcs(additionnal_xlimit),
    _eLRSA_handlerAtSide(eLRSA_handlerAtSide),
    _lineHeight_qcs(lineHeight),

    _bCanBeMouseMoved(bCanBeMouseMoved),

    _pixel_handlerWidth(handlerWidth),
    _pixel_handlerHeight(handlerHeight)
     {

     qDebug() << "CustomGraphicsItemVerticalAdjuster::CustomGraphicsItemVerticalAdjuster" <<
                 "_eLRSA_handlerAtSide set to :" << _eLRSA_handlerAtSide;


    _dThickness = 1.0; //sync with default _dThickness for CustomLineMouseOver and CustomGraphicsItem_coloredGripSquare
    _customLineMouseOver = new CustomLineMouseOver( Qt::darkGreen,
                                                    QColor(255,0,0), //red
                                                    _xpos_qcs, _xrange_minValue_qcs, _xrange_maxValue_qcs, 0, _lineHeight_qcs,
                                                    _bCanBeMouseMoved);
    _customLineMouseOver->setParentItem(this);


    _handler = new CustomGraphicsItem_coloredGripSquare(
                Qt::darkGreen,
                QColor(255,0,0), //red
                _pixel_handlerWidth, _pixel_handlerHeight,
                _bCanBeMouseMoved, false);
    _handler->setParentItem(this);


    connect(_customLineMouseOver, &CustomLineMouseOver::signal_customLineMouseOver_movedTo,
                            this, &CustomGraphicsItemVerticalAdjuster::slot_customLineMouseOver_movedTo_inPixelDim);
    connect(_handler, &CustomGraphicsItem_coloredGripSquare::signal_coloredSquare_movedTo,
                this, &CustomGraphicsItemVerticalAdjuster::slot_coloredSquare_movedTo_inPixelDim);

    connect(            _handler, &CustomGraphicsItem_coloredGripSquare::signal_selectStateChanged,
            _customLineMouseOver, &CustomLineMouseOver::slot_selectStateChangedFromOutside);

    connect(_customLineMouseOver, &CustomLineMouseOver::signal_selectStateChanged,
                        _handler, &CustomGraphicsItem_coloredGripSquare::slot_selectStateChangedFromOutside);


    //change the CustomGraphicsVerticalAdjuster ZValue when selection changed to be able to always have the selected line top of others
    connect(            _handler, &CustomGraphicsItem_coloredGripSquare::signal_selectStateChanged,
                            this, &CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged);

    //mouse released event from coloredGripSquare and CustomLineMouseOver
    connect(            _handler, &CustomGraphicsItem_coloredGripSquare::signal_coloredSquare_mouseReleased,
                            this, &CustomGraphicsItemVerticalAdjuster::slot_mouseReleased);
    connect(_customLineMouseOver, &CustomLineMouseOver::signal_customLineMouseOver_mouseReleased,
                            this, &CustomGraphicsItemVerticalAdjuster::slot_mouseReleased);

    qDebug() << __FUNCTION__ << "2) before; here #103";
    connect(            _handler, &CustomGraphicsItem_coloredGripSquare::signal_locationInAdjustement,
                            this, &CustomGraphicsItemVerticalAdjuster::slot_locationInAdjustement);
    connect(_customLineMouseOver, &CustomLineMouseOver::signal_locationInAdjustement,
                            this, &CustomGraphicsItemVerticalAdjuster::slot_locationInAdjustement);
    qDebug() << __FUNCTION__ << "2) after; here #108";


    //due to sync state between  _handler and _customLineMouseOver, there is no need to connect also the two elements of a verticalAdjuster to manage the ZValue specific use case
    //  connect(_customLineMouseOver, &CustomLineMouseOver::signal_selectStateChanged,
    //                          this, &CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged);

    //qDebug() << __FUNCTION__ << "_qrectfbbox_qcsDim="<< _qrectfbbox_qcsDim;
}

void CustomGraphicsItemVerticalAdjuster::slot_mouseReleased() {
    qDebug() << __FUNCTION__  << "CustomGraphicsItemVerticalAdjuster::slot_mouseReleased";
    qDebug() << __FUNCTION__
             << "will emit signal_xPosition_changed with _xpos = " << _xpos_qcs
             << "and _eLRSA_handlerAtSide: " << _eLRSA_handlerAtSide;

    //inform outside
    emit signal_xPosition_changed(_xpos_qcs, _eLRSA_handlerAtSide);
}

void CustomGraphicsItemVerticalAdjuster::computePixelXPosAndPixelXHandlerPosition() {
    _xpos_pixel = _converterPixToqsc.qsc_x_to_pixel(_xpos_qcs);
    _handlerxposx_pixel = _xpos_pixel;
    if (_eLRSA_handlerAtSide == e_LRsA_right) {
        _handlerxposx_pixel -= _pixel_handlerWidth;
    }
    //qDebug() << __FUNCTION__  << " will return handlerPosx = " << handlerPosx;

}

void CustomGraphicsItemVerticalAdjuster::init_qrectfbbox() {

    double dThicknessDiv2 = _dThickness / 2.0;

    _qrectfbbox_pixelDim.setTop(.0 - dThicknessDiv2);

    _qrectfbbox_pixelDim.setLeft(_handlerxposx_pixel - dThicknessDiv2);
    _qrectfbbox_pixelDim.setSize(QSizeF(_pixel_handlerWidth + _dThickness, //in x (_dThickness = 2*dThicknessDiv2)
                                        _converterPixToqsc.qsc_y_to_pixel(_lineHeight_qcs) + _pixel_handlerHeight + _dThickness + dThicknessDiv2 )); //in y (_dThickness = 2*dThicknessDiv2)

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster)  _lineHeight_qcs = " << _lineHeight_qcs;
    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster)  _converterPixToqsc.pixel_y_to_qsc(_lineHeight_qcs) = " << _converterPixToqsc.pixel_y_to_qsc(_lineHeight_qcs);
    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster)  _pixel_handlerHeight = " << _pixel_handlerHeight;

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster) _qrectfbbox_pixelDim as tl, br:"
             << _qrectfbbox_pixelDim.topLeft() << ", "
             << _qrectfbbox_pixelDim.bottomRight();

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster) _qrectfbbox_pixelDim as topleft and size:" << _qrectfbbox_pixelDim;

}


void CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged(bool bNewState) {

    if (_eLRSA_handlerAtSide == e_LRsA_right) {
        qDebug() << "CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged" << "e_LRsA_right";
    }
    if (bNewState) {
        qDebug() << "CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged" << "setZValue(2.0//1.0)";
        setZValue(4.0);
    } else {
        qDebug() << "CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged" << "setZValue(1.0//0.0)";
        setZValue(2.0);
    }
    //@#LP it emit for any vertical adjusters (two of them will be never in the specific use case)
    emit signal_selectionStateChanged(bNewState); //to handle the possible 'right line of left side' location == 'leftt line of right side' location
    //and hence adjust the zvalue
}

void CustomGraphicsItemVerticalAdjuster::setXPos(double x) {

    _xpos_qcs = x;
    force_xpos_inLimitedRange(); //@#LP should not be useful; but add this just to be sure; in case of file hack edition (with invalid hack mistake)
    computePixelXPosAndPixelXHandlerPosition();

    if (_customLineMouseOver) {
        _customLineMouseOver->setX(_xpos_pixel);
    }
    if (_handler) {
        _handler->setXPos(_handlerxposx_pixel);
    }
    send_signal_setXLimitForMovement(); //send to the other vertical adjuster to update its additional limit value

}


void CustomGraphicsItemVerticalAdjuster::setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster) {

    _dThickness = sCT_xAdjuster._dThickness;
    _customLineMouseOver->setGraphicsParameters(sCT_xAdjuster);
    _handler->setGraphicsParameters(sCT_xAdjuster);

}

void CustomGraphicsItemVerticalAdjuster::setState_movableWithAccordGripsSquareVisibility(bool bState) {

    _bCanBeMouseMoved = bState;

    _customLineMouseOver->set_canBeMouseMoved(_bCanBeMouseMoved);
    _handler->set_canBeMouseMoved(_bCanBeMouseMoved);
}

void CustomGraphicsItemVerticalAdjuster::slot_customLineMouseOver_movedTo_inPixelDim(double x) { //received x from line in pixel (not qcs) (the line lives in the scene, which is in pixel with the new way to handle the scene)

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster) received x = " << x;

    double dlinePosQcsFromPixel = _converterPixToqsc.pixel_x_to_qsc(x);

    //force to a step value
    int intlinepos_qcs = static_cast<int>(dlinePosQcsFromPixel);
    double dlinepos_qcs = static_cast<double>(intlinepos_qcs);
    _xpos_qcs = dlinepos_qcs;

    force_xpos_inLimitedRange();

    computePixelXPosAndPixelXHandlerPosition();

    if (_customLineMouseOver) {
        _customLineMouseOver->setX(_xpos_pixel);
    }
    if (_handler) {
        _handler->setXPos(_handlerxposx_pixel);
    }

    send_signal_setXLimitForMovement(); //send to the other vertical adjuster to update its additional limit value

}

void CustomGraphicsItemVerticalAdjuster::slot_locationInAdjustement(bool bState) { //just redirect
    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster) bState = " << bState;
    emit signal_locationInAdjustement(bState);
}


//received a range limitation from an another adjuster; this is used to constraint an adjuster to do not go beyound the x position of the other
void CustomGraphicsItemVerticalAdjuster::slot_setXLimitForMovement(double xlimit/*, int i_exLimitType*/) {
     qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster)";
     qDebug() << __FUNCTION__ << "xlimit: " << xlimit;

    _additionnal_xlimit_qcs = xlimit;
}

void CustomGraphicsItemVerticalAdjuster::slot_coloredSquare_movedTo_inPixelDim(double x) { //received x from colored square in pixel (not qcs) (the colored square lives in the scene, which is in pixel with the new way to handle the scene)
    qDebug() << __FUNCTION__;

    double dlineposPixel = x;
    if (_eLRSA_handlerAtSide == e_LRsA_right) {
        dlineposPixel+=_pixel_handlerWidth;
    }

    double dlinePosQcsFromPixel = _converterPixToqsc.pixel_x_to_qsc(dlineposPixel);

    //force to a step value
    int intlinepos_qcs = static_cast<int>(/*dlinepos*/dlinePosQcsFromPixel);
    double dlinepos_qcs = static_cast<double>(intlinepos_qcs);
    _xpos_qcs = dlinepos_qcs;

    force_xpos_inLimitedRange();


    _xpos_pixel = _converterPixToqsc.qsc_x_to_pixel(_xpos_qcs);

    double handlerxPosPixel = _xpos_pixel;
    if (_eLRSA_handlerAtSide == e_LRsA_right) {
        handlerxPosPixel-=_pixel_handlerWidth;
    }

    if (_customLineMouseOver) {
        _customLineMouseOver->setXPos(_xpos_pixel);
    }
    if (_handler) {
        _handler->setXPos(handlerxPosPixel);
    }
    send_signal_setXLimitForMovement();//send to the other vertical adjuster to update its additional limit value
}


void CustomGraphicsItemVerticalAdjuster::send_signal_setXLimitForMovement() {
    if (_eLRSA_handlerAtSide == e_LRsA_left) {
        qDebug() << __FUNCTION__ << "from slot_coloredSquare_movedTo emit signal_setXLimitForMovement( " << _xpos_qcs  << " , e_xLimitType::e_xLT_min)";
        emit signal_setXLimitForMovement(_xpos_qcs);
    } else {
        qDebug() << __FUNCTION__ << "from slot_coloredSquare_movedTo emit signal_setXLimitForMovement( " << _xpos_qcs  << " , e_xLimitType::e_xLT_max)";
        emit signal_setXLimitForMovement(_xpos_qcs);
    }
}


void CustomGraphicsItemVerticalAdjuster::adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc) {

    prepareGeometryChange();    
    _converterPixToqsc = converterPixToqsc;

    computePixelXPosAndPixelXHandlerPosition();
    init_qrectfbbox();

    if (_customLineMouseOver) {
        _customLineMouseOver->set_lineHeight(_converterPixToqsc.qsc_y_to_pixel(_lineHeight_qcs));
        _customLineMouseOver->setXPos(_xpos_pixel);
    }
    if (_handler) {
        _handler->setHandlerPositionAndSize(_handlerxposx_pixel,
                                            _converterPixToqsc.qsc_y_to_pixel(_lineHeight_qcs), _pixel_handlerWidth, _pixel_handlerHeight);
    }
}

CustomGraphicsItemVerticalAdjuster::~CustomGraphicsItemVerticalAdjuster() {
    qDebug() << __FUNCTION__;
}

QRectF CustomGraphicsItemVerticalAdjuster::boundingRect() const {
    //qDebug() << __FUNCTION__ << "CustomGraphicsItemVerticalAdjuster";

    return(_qrectfbbox_pixelDim);
}

void CustomGraphicsItemVerticalAdjuster::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //qDebug() << __FUNCTION__ << "CustomGraphicsItemVerticalAdjuster";

    //@#LP debug:
    /*QBrush brush_debug(Qt::SolidPattern);
    brush_debug.setColor({128,0,128,128});
    painter->setBrush(brush_debug);
    painter->drawRect(_qrectfbbox_pixelDim);*/

}

int CustomGraphicsItemVerticalAdjuster::type() const {
    return(e_customGraphicsItemType_part2::e_cgit_CustomGraphicsVerticalAdjuster);
}

void CustomGraphicsItemVerticalAdjuster::force_xpos_inLimitedRange() {

    force_xpos_inRange();

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster)" << "_additionnal_xlimit = " << _additionnal_xlimit_qcs;

    //avoid vertical adjuster to be at the same x than an another
    if (_eLRSA_handlerAtSide == e_LRsA_left) {
        if (_xpos_qcs >= _additionnal_xlimit_qcs) {
            _xpos_qcs = _additionnal_xlimit_qcs - 1;
        }
    } else { //e_LRsA_right
        if (_xpos_qcs <= _additionnal_xlimit_qcs) {
            _xpos_qcs = _additionnal_xlimit_qcs + 1 ;
        }
    }
}

void CustomGraphicsItemVerticalAdjuster::force_xpos_inRange() {
    if (_xpos_qcs < _xrange_minValue_qcs) {
        _xpos_qcs = _xrange_minValue_qcs;
    }
    if (_xpos_qcs > _xrange_maxValue_qcs) {
        _xpos_qcs = _xrange_maxValue_qcs;
    }
}
