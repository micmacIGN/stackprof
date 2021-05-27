#include <QDebug>

#include <QPainter>

#include "CustomGraphicsItemVerticalAdjusterWithCentereredGrip_withAdjustableWithElements.h"

#include "CustomGraphicsItemType_v2.h"

#include "CustomLineMouseOver.h"
#include "CustomGraphicsItem_coloredGripSquare.h"

CustomGraphicsItemVerticalAdjusterWithCentereredGrip::CustomGraphicsItemVerticalAdjusterWithCentereredGrip(
        double xpos,
        double xrange_minValue, double xrange_maxValue,
        double lineHeight,
        bool bShowGripSquare,
        bool bCanBeMouseMoved,
        double handlerWidth, double handlerHeight,

        QGraphicsItem *parent): QGraphicsObject(parent),
    _xpos_qcs(xpos),
    _xrange_minValue_qcs(xrange_minValue),
    _xrange_maxValue_qcs(xrange_maxValue),
    _lineHeight_qcs(lineHeight),
    _bShowGripSquare(bShowGripSquare),
    _bCanBeMouseMoved(bCanBeMouseMoved),
    _pixel_handlerWidth(handlerWidth),
    _pixel_handlerHeight(handlerHeight)
    {

     //qDebug() << "CustomGraphicsItemVerticalAdjuster::CustomGraphicsItemVerticalAdjuster" <<
     //            "_eLRSA_handlerAtSide set to :" << _eLRSA_handlerAtSide;

    _dThickness = 1.0; //sync with default _dThickness for CustomLineMouseOver and CustomGraphicsItem_coloredGripSquare
    _customLineMouseOver = new CustomLineMouseOver(QColor(  0,102,204), //blue
                                                   QColor(255,  0,  0), //red
                                                   _xpos_qcs, _xrange_minValue_qcs, _xrange_maxValue_qcs, 0, _lineHeight_qcs,
                                                   _bCanBeMouseMoved);
    _customLineMouseOver->setParentItem(this);


    _handler = new CustomGraphicsItem_coloredGripSquare(
                QColor(  0,102,204), //blue
                QColor(255,  0,  0), //red
                _pixel_handlerWidth, _pixel_handlerHeight,
                _bCanBeMouseMoved, true);
    _handler->setParentItem(this);
    _handler->setVisible(_bShowGripSquare);


    connect(_customLineMouseOver, &CustomLineMouseOver::signal_customLineMouseOver_movedTo,
                            this, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_customLineMouseOver_movedTo_inPixelDim);
    connect(_handler, &CustomGraphicsItem_coloredGripSquare::signal_coloredSquare_movedTo,
                this, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_coloredSquare_movedTo_inPixelDim);

    connect(            _handler, &CustomGraphicsItem_coloredGripSquare::signal_selectStateChanged,
            _customLineMouseOver, &CustomLineMouseOver::slot_selectStateChangedFromOutside);

    connect(_customLineMouseOver, &CustomLineMouseOver::signal_selectStateChanged,
                        _handler, &CustomGraphicsItem_coloredGripSquare::slot_selectStateChangedFromOutside);



    //change the CustomGraphicsVerticalAdjuster ZValue when selection changed to be able to always have the selected line top of others
    connect(            _handler, &CustomGraphicsItem_coloredGripSquare::signal_selectStateChanged,
                            this, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_selectionStateChanged);

    //mouse released event from coloredGripSquare and CustomLineMouseOver
    connect(            _handler, &CustomGraphicsItem_coloredGripSquare::signal_coloredSquare_mouseReleased,
                            this, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_mouseReleased);
    connect(_customLineMouseOver, &CustomLineMouseOver::signal_customLineMouseOver_mouseReleased,
                            this, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_mouseReleased);


    qDebug() << __FUNCTION__ << "1) before; here #100";
    //direct forward:
    connect(            _handler, &CustomGraphicsItem_coloredGripSquare::signal_locationInAdjustement,
                            this, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_locationInAdjustement);
    connect(_customLineMouseOver, &CustomLineMouseOver::signal_locationInAdjustement,
                            this, &CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_locationInAdjustement);
    qDebug() << __FUNCTION__ << "1) after; here #106";



    //due to sync state between  _handler and _customLineMouseOver, there is no need to connect also the two elements of a verticalAdjuster to manage the ZValue specific use case
    //  connect(_customLineMouseOver, &CustomLineMouseOver::signal_selectStateChanged,
    //                          this, &CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged);

    //qDebug() << __FUNCTION__ << "_qrectfbbox_qcsDim="<< _qrectfbbox_qcsDim;
}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_locationInAdjustement(bool bState) {  //just redirect
    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjusterWithCentereredGrip)";
    emit signal_locationInAdjustement(bState);
}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster) {

    _dThickness = sCT_xAdjuster._dThickness;
    _customLineMouseOver->setGraphicsParameters(sCT_xAdjuster);
    _handler->setGraphicsParameters(sCT_xAdjuster);

}


void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::setState_movableWithAccordGripsSquareVisibility(bool bState) {
    _bShowGripSquare = bState;
    _bCanBeMouseMoved = bState;

    _handler->setVisible(_bShowGripSquare);

    _customLineMouseOver->set_canBeMouseMoved(_bCanBeMouseMoved);
    _handler->set_canBeMouseMoved(_bCanBeMouseMoved);

}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_mouseReleased() {
    /*qDebug() << __FUNCTION__  << "CustomGraphicsItemVerticalAdjuster::slot_mouseReleased";
    qDebug() << __FUNCTION__
             << "will emit signal_xPosition_changed with _xpos = " << _xpos_qcs
             << "and _eLRSA_handlerAtSide: " << _eLRSA_handlerAtSide;*/

    /*if (_eLRSA_handlerAtSide == e_LRsA_left) {
        qDebug() << __FUNCTION__  << "( e_LRsA_left )";
    } else {
        //qDebug() << __FUNCTION__  << "( e_LRsA_right )";
    }*/

    //inform outside
    emit signal_xPosition_changed(_xpos_qcs);
}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::computePixelXPosAndPixelXHandlerPosition() {
    _xpos_pixel = _converterPixToqsc.qsc_x_to_pixel(_xpos_qcs);
    _handlerxposx_pixel = _xpos_pixel - (_pixel_handlerWidth/2.0);
    //qDebug() << __FUNCTION__  << " will return handlerPosx = " << handlerPosx;

}

//showing or not the squared grip, we use the same boudingbox
void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::init_qrectfbbox() {

    double dThicknessDiv2 = _dThickness / 2.0;

    _qrectfbbox_pixelDim.setTop(.0 - dThicknessDiv2);

    _qrectfbbox_pixelDim.setLeft(_handlerxposx_pixel - dThicknessDiv2 );
    _qrectfbbox_pixelDim.setSize(QSizeF(_pixel_handlerWidth + _dThickness,
                                        _converterPixToqsc.qsc_y_to_pixel(_lineHeight_qcs) + _dThickness
                                        + _pixel_handlerHeight + dThicknessDiv2));

}


void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_selectionStateChanged(bool bNewState) {

    /*if (_eLRSA_handlerAtSide == e_LRsA_right) {
        qDebug() << "CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged" << "e_LRsA_right";
    }*/

    if (bNewState) {
        qDebug() << "CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged" << "setZValue(3.0)";
        setZValue(4.0);
    } else {
        qDebug() << "CustomGraphicsItemVerticalAdjuster::slot_selectionStateChanged" << "setZValue(2.0)";
        setZValue(3.0);
    }

    //@#LP it emit for any vertical adjusters (two of them will be never in the specific use case)

    emit signal_selectionStateChanged(bNewState); //to handle the cover possible 'right line of left side' location == 'leftt line of right side' location
    //and hence adjust the zvalue
}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::setXPos(double x) {

    _xpos_qcs = x;

    force_xpos_inRange();

    computePixelXPosAndPixelXHandlerPosition();

    if (_customLineMouseOver) {
        _customLineMouseOver->setX(_xpos_pixel);
    }
    if (_handler) {
        _handler->setXPos(_handlerxposx_pixel);
    }
    //send_signal_setXLimitForMovement(); //send to the other vertical adjuster to update its additional limit value

}


void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_customLineMouseOver_movedTo_inPixelDim(double x) { //received x from line in pixel (not qcs) (the line lives in the scene, which is in pixel with the new way to handle the scene)

    qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster) received x = " << x;


    double dlinePosQcsFromPixel = _converterPixToqsc.pixel_x_to_qsc(x);

    //force to a step value
    int intlinepos_qcs = static_cast<int>(/*dlinepos*/dlinePosQcsFromPixel);
    double dlinepos_qcs = static_cast<double>(intlinepos_qcs);
    _xpos_qcs = dlinepos_qcs;



    force_xpos_inRange();

    computePixelXPosAndPixelXHandlerPosition();

    if (_customLineMouseOver) {
        _customLineMouseOver->setX(_xpos_pixel);
    }
    if (_handler) {
        _handler->setXPos(_handlerxposx_pixel);
    }

    //send_signal_setXLimitForMovement(); //send to the other vertical adjuster to update its additional limit value

}

//received a range limitation from an another adjuster; this is used to constraint an adjuster to do not go beyound the x position of the other
void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_setXLimitForMovement(double xlimit) {
     qDebug() << __FUNCTION__ << "(CustomGraphicsItemVerticalAdjuster)";
     qDebug() << __FUNCTION__ << "xlimit: " << xlimit;

}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::slot_coloredSquare_movedTo_inPixelDim(double x) { //received x from colored square in pixel (not qcs) (the colored square lives in the scene, which is in pixel with the new way to handle the scene)
    qDebug() << __FUNCTION__;

    double dlineposPixel = x;

    dlineposPixel+=_pixel_handlerWidth/2.0;


    double dlinePosQcsFromPixel = _converterPixToqsc.pixel_x_to_qsc(dlineposPixel);

    //force to a step value
    int intlinepos_qcs = static_cast<int>(dlinePosQcsFromPixel);
    double dlinepos_qcs = static_cast<double>(intlinepos_qcs);
    _xpos_qcs = dlinepos_qcs;


    force_xpos_inRange();

    _xpos_pixel = _converterPixToqsc.qsc_x_to_pixel(_xpos_qcs);

    double handlerxPosPixel = _xpos_pixel;

    handlerxPosPixel-=_pixel_handlerWidth/2.0;


    if (_customLineMouseOver) {
        _customLineMouseOver->setXPos(_xpos_pixel);
    }
    if (_handler) {
        _handler->setXPos(handlerxPosPixel);
    }
    //send_signal_setXLimitForMovement();//send to the other vertical adjuster to update its additional limit value
}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc) {

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

CustomGraphicsItemVerticalAdjusterWithCentereredGrip::~CustomGraphicsItemVerticalAdjusterWithCentereredGrip() {
    qDebug() << __FUNCTION__;
}

QRectF CustomGraphicsItemVerticalAdjusterWithCentereredGrip::boundingRect() const {
    qDebug() << __FUNCTION__ << "CustomGraphicsItemVerticalAdjuster" << "_qrectfbbox_pixelDim = " << _qrectfbbox_pixelDim;
    qDebug() << __FUNCTION__ << "CustomGraphicsItemVerticalAdjuster" << "( _xpos_pixel = " << _xpos_pixel << " )";
    qDebug() << __FUNCTION__ << "CustomGraphicsItemVerticalAdjuster" << "( _handlerxposx_pixel = " << _handlerxposx_pixel << " )";

    return(_qrectfbbox_pixelDim);
}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //qDebug() << __FUNCTION__ << "CustomGraphicsItemVerticalAdjuster";

    //@#LP debug:
    /*
    QBrush brush_debug(Qt::SolidPattern);
    brush_debug.setColor({128,0,128,128});
    painter->setBrush(brush_debug);
    painter->drawRect(_qrectfbbox_pixelDim);*/
}

int CustomGraphicsItemVerticalAdjusterWithCentereredGrip::type() const {
    return(e_customGraphicsItemType_part2::e_cgit_CustomGraphicsVerticalAdjusterWithCentereredGrip);
}

void CustomGraphicsItemVerticalAdjusterWithCentereredGrip::force_xpos_inRange() {
    if (_xpos_qcs < _xrange_minValue_qcs) {
        _xpos_qcs = _xrange_minValue_qcs;
    }
    if (_xpos_qcs > _xrange_maxValue_qcs) {
        _xpos_qcs = _xrange_maxValue_qcs;
    }
}
