#ifndef CustomGraphicsItemVerticalAdjuster_HPP
#define CustomGraphicsItemVerticalAdjuster_HPP

#include <QGraphicsItem>
#include <QDebug>

class CustomLineMouseOver;
class CustomGraphicsItem_coloredGripSquare;

#include "../../../core/leftRight_sideAccess.hpp"

#include "../../ConverterPixelToQsc.h"

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h"

enum e_xLimitType {
    e_xLT_min,
    e_xLT_max
};

class CustomGraphicsItemVerticalAdjuster: // public QObject, public QGraphicsItem {
                                          public QGraphicsObject {

    Q_OBJECT

public:

    CustomGraphicsItemVerticalAdjuster(
            double xpos,
            double xrange_minValue, double xrange_maxValue,
            double additionnal_xlimit,
            e_LRsideAccess eLRSA_handlerAtSide,
            double lineHeight,

            bool bCanBeMouseMoved,

            double pixel_handlerWidth,
            double pixel_handlerHeight,

            QGraphicsItem *parent=nullptr);

    void adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc);

    ~CustomGraphicsItemVerticalAdjuster() override;

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int type() const override;

    void setXPos(double x);

    void set_lineHeight(double lineHeight);

    void setGraphicsParameters(const S_Color_Thickness& sCT_xAdjuster);

    void setState_movableWithAccordGripsSquareVisibility(bool bState);

public slots:
    void slot_customLineMouseOver_movedTo_inPixelDim(double x);
    void slot_coloredSquare_movedTo_inPixelDim(double x);

    void slot_selectionStateChanged(bool bNewState);

    void slot_mouseReleased();

    //received a range limitation from an another adjuster; this is used to constraint an adjuster to do not go beyound the x position of the other
    void slot_setXLimitForMovement(double xlimit);

    void slot_locationInAdjustement(bool bState);

signals:
    void signal_xPosition_changed(double xpos, int i_eLRsA);

    //sent range limitation to an another adjuster; this is used to constraint an adjuster to do not go beyound the x position of the other
    void signal_setXLimitForMovement(double xlimit);

    void signal_selectionStateChanged(bool bNewState); //to handle the possible 'right line of left side' location == 'leftt line of right side' location

    void signal_locationInAdjustement(bool bState); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)

private:

    void computePixelXPosAndPixelXHandlerPosition();

    void init_qrectfbbox();

    void force_xpos_inRange();
    void force_xpos_inLimitedRange();

    void send_signal_setXLimitForMovement();

private:

    double _xpos_qcs;

    double _xpos_pixel;
    double _handlerxposx_pixel;

    double _xrange_minValue_qcs;
    double _xrange_maxValue_qcs;

    double _additionnal_xlimit_qcs; //an another vertical adjuster can create an x limit inside the x range, which need to be taken into account

    e_LRsideAccess _eLRSA_handlerAtSide;

    double _lineHeight_qcs;

    bool _bCanBeMouseMoved;

    double _pixel_handlerWidth;
    double _pixel_handlerHeight;

    CustomLineMouseOver *_customLineMouseOver;
    CustomGraphicsItem_coloredGripSquare *_handler;

    QRectF _qrectfbbox_pixelDim;

    ConverterPixelToQsc _converterPixToqsc;

    double _dThickness;
};


#endif // CustomGraphicsItemVerticalAdjuster_HPP
