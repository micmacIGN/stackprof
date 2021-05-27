#ifndef CustomGraphicsItemVerticalAdjusterWithCentereredGrip_HPP
#define CustomGraphicsItemVerticalAdjusterWithCentereredGrip_HPP

#include <QGraphicsItem>
#include <QDebug>

class CustomLineMouseOver;
class CustomGraphicsItem_coloredGripSquare;

#include "../../../core/leftRight_sideAccess.hpp"

#include "../../ConverterPixelToQsc.h"

#include "../../../appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h"

class CustomGraphicsItemVerticalAdjusterWithCentereredGrip: public QGraphicsObject {

    Q_OBJECT

public:

    CustomGraphicsItemVerticalAdjusterWithCentereredGrip(
            double xpos,
            double xrange_minValue, double xrange_maxValue,
            double lineHeight,
            bool bShowGripSquare,
            bool bCanBeMouseMoved,
            double pixel_handlerWidth,
            double pixel_handlerHeight,
            QGraphicsItem *parent=nullptr);

    void adaptToPixelScene(const ConverterPixelToQsc& converterPixToqsc);

    ~CustomGraphicsItemVerticalAdjusterWithCentereredGrip() override;

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
    void signal_xPosition_changed(double xpos);

    //sent range limitation to an another adjuster; this is used to constraint an adjuster to do not go beyound the x position of the other
    void signal_setXLimitForMovement(double xlimit);

    void signal_selectionStateChanged(bool bNewState); //to handle the possibility of line over other any of the four left/right adjusters

    void signal_locationInAdjustement(bool bState); //to inform the window containing all (the vertical layout and all inside), that keypressevent should be rejected)

private:

    void computePixelXPosAndPixelXHandlerPosition();

    void init_qrectfbbox();

    void force_xpos_inRange();

    //void send_signal_setXLimitForMovement();

private:

    double _xpos_qcs;

    double _xrange_minValue_qcs;
    double _xrange_maxValue_qcs;

    double _lineHeight_qcs;

    bool _bShowGripSquare;
    bool _bCanBeMouseMoved;

    double _pixel_handlerWidth;
    double _pixel_handlerHeight;

    double _xpos_pixel;
    double _handlerxposx_pixel;

    CustomLineMouseOver *_customLineMouseOver;
    CustomGraphicsItem_coloredGripSquare *_handler;

    QRectF _qrectfbbox_pixelDim;

    ConverterPixelToQsc _converterPixToqsc;

    double _dThickness;
};


#endif // CustomGraphicsItemVerticalAdjusterWithCentereredGrip_HPP
