#include <QGraphicsLineItem>

#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

#include "CustomGraphicsSegmentItem2.h"

#include "CustomGraphicsItemTypes.h"

#include <QPainter>
#include <QPainterPath>

CustomGraphicsSegmentItem2::CustomGraphicsSegmentItem2(int idSegment,
                                                     QPointF pointA_noZLI, QPointF pointB_noZLI,
                                                     Qt::GlobalColor colorHightlighted,
                                                     Qt::GlobalColor colorNotHightlighted,
                                                     QGraphicsItem *parent):

    QGraphicsItem(parent),
    _id(idSegment),
    _pointAB_noZLI { pointA_noZLI, pointB_noZLI },
    _currentZLI(-1),
    _bShowAsHighlighted(false),

    _qrectf_boundingBoxNoZLI {.0,.0,.0,.0},
    _qrectf_boundingBox {.0,.0,.0,.0},
    _bShowBoundingRect(false)
    {

    //1.5 true
    _widthBorderNoZLI = 1.0;

    _bUseCosmeticPen = false; //true; //false;
    _bUseAntialiasing = !_bUseCosmeticPen;

    qDebug() << __FUNCTION__ << "_pointAB_noZLI =" << _pointAB_noZLI[0] << " , " << _pointAB_noZLI[1];
    computeBoundingBoxNoZLI();

    //qreal widthBorderZLI = 1.0;

    _pHighlighted.setStyle(Qt::PenStyle::SolidLine);
    _pHighlighted.setColor(colorHightlighted);
    _pHighlighted.setCosmetic(_bUseCosmeticPen);
    _pHighlighted.setWidthF(_widthBorderNoZLI);
    _pHighlighted.setCapStyle(Qt::PenCapStyle::FlatCap);

    _pNotHighlighted.setStyle(Qt::PenStyle::SolidLine);
    _pNotHighlighted.setColor(colorNotHightlighted);
    _pNotHighlighted.setCosmetic(_bUseCosmeticPen);
    _pNotHighlighted.setWidthF(_widthBorderNoZLI);
    _pNotHighlighted.setCapStyle(Qt::PenCapStyle::FlatCap);

    setHightlighted(false);

    setVisible(true);

    _scaleOfViewSideForCurrentZLI = 1;
    adjustGraphicsItemWidth(_scaleOfViewSideForCurrentZLI);

    adjustLocationAndSizeToZLI(1);
}

void CustomGraphicsSegmentItem2::setPen(QPen *ptrQPenToUse) {
    _ptrUsedPen = ptrQPenToUse;
    update();
}

void CustomGraphicsSegmentItem2::setHightlighted(bool bShowAsHighlighted) {
    _bShowAsHighlighted = bShowAsHighlighted;
    if  (_bShowAsHighlighted) {
        setPen(&_pHighlighted);
    } else {
       setPen(&_pNotHighlighted);
    }
}

void CustomGraphicsSegmentItem2::computeBoundingBoxNoZLI() {

    qreal top_noZLI    = _pointAB_noZLI[0].y();
    qreal bottom_noZLI = _pointAB_noZLI[1].y();
    qDebug() << __FUNCTION__ << ": default feed top bottom = " << top_noZLI << ", " << bottom_noZLI;
    if (top_noZLI > bottom_noZLI) {
        top_noZLI    = _pointAB_noZLI[1].y();
        bottom_noZLI = _pointAB_noZLI[0].y();
        qDebug() << __FUNCTION__ << ": adjusted top bottom = " << top_noZLI << ", " << bottom_noZLI;
    }

    qreal left_noZLI  = _pointAB_noZLI[0].x();
    qreal right_noZLI = _pointAB_noZLI[1].x();
    qDebug() << __FUNCTION__ << ": default feed left right = " << left_noZLI << ", " << right_noZLI;
    if (left_noZLI > right_noZLI) {
        left_noZLI  = _pointAB_noZLI[1].x();
        right_noZLI = _pointAB_noZLI[0].x();
        qDebug() << __FUNCTION__ << ": adjusted left right = " << left_noZLI << ", " << right_noZLI;
    }

    qDebug() << __FUNCTION__ << ": CustomGraphicsSegmentItem2, _pointAB_noZLI[0] = " << _pointAB_noZLI[0];
    qDebug() << __FUNCTION__ << ": CustomGraphicsSegmentItem2, _pointAB_noZLI[1] = " << _pointAB_noZLI[1];


    //_qrectf_boundingBoxNoZLI.setTopLeft({left_noZLI, top_noZLI});
    //_qrectf_boundingBoxNoZLI.setBottomRight({right_noZLI, bottom_noZLI});

//#define def_segmentItem2_bboxcomputeMethod0
#ifdef def_segmentItem2_bboxcomputeMethod0
    qreal widthBorderNoZLI_div2 = _widthBorderNoZLI / 2.0;
    _qrectf_boundingBoxNoZLI.setTop   (top_noZLI    - widthBorderNoZLI_div2);
    _qrectf_boundingBoxNoZLI.setBottom(bottom_noZLI + widthBorderNoZLI_div2);
    _qrectf_boundingBoxNoZLI.setLeft  (left_noZLI   - widthBorderNoZLI_div2);
    _qrectf_boundingBoxNoZLI.setRight (right_noZLI  + widthBorderNoZLI_div2);
#else
    _qrectf_boundingBoxNoZLI.setTop   (top_noZLI   );
    _qrectf_boundingBoxNoZLI.setBottom(bottom_noZLI);
    _qrectf_boundingBoxNoZLI.setLeft  (left_noZLI  );
    _qrectf_boundingBoxNoZLI.setRight (right_noZLI );
#endif

    qDebug() << __FUNCTION__ << ": CustomGraphicsSegmentItem2, _qrectf_boundingBoxNoZLI  top, left, bottom, right= "
             << _qrectf_boundingBoxNoZLI.top()
             << _qrectf_boundingBoxNoZLI.left()
             << _qrectf_boundingBoxNoZLI.bottom()
             << _qrectf_boundingBoxNoZLI.right();
}

void CustomGraphicsSegmentItem2::setPointAnoZLI_andUpdate(const QPointF& qptfA) {
     prepareGeometryChange();
    _pointAB_noZLI[0] = qptfA;
    if (_currentZLI == -1) {
        qDebug() << __FUNCTION__ << " errror: code implementation: _currentZLI is -1";
        return;
    }
    computeBoundingBoxNoZLI();
    adjustLocationAndSizeToZLI(_currentZLI);

}

void CustomGraphicsSegmentItem2::setPointBnoZLI_andUpdate(const QPointF& qptfB) {
    prepareGeometryChange();
    _pointAB_noZLI[1] = qptfB;
    if (_currentZLI == -1) {
        qDebug() << __FUNCTION__ << " errror: code implementation: _currentZLI is -1";
        return;
    }
    computeBoundingBoxNoZLI();
    adjustLocationAndSizeToZLI(_currentZLI);
}


void CustomGraphicsSegmentItem2::adjustLocationAndSizeToZLI(int ZLI) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsSegmentItem2) received ZLI = " << ZLI;

    prepareGeometryChange();

    _currentZLI = ZLI;
    #ifdef def_segmentItem2_bboxcomputeMethod0
        _widthBorderZLI = _widthBorderNoZLI / _currentZLI;
        _scaleOfViewSideForCurrentZLI = 1.0;
    #else
        _widthBorderZLI = _widthBorderNoZLI;
        _scaleOfViewSideForCurrentZLI = 1.0;
    #endif

    _pointAB[0] = _pointAB_noZLI[0]/static_cast<qreal>(_currentZLI);
    _pointAB[1] = _pointAB_noZLI[1]/static_cast<qreal>(_currentZLI);

    //qreal penWidthDiv2 = 1.0; //1.0/2.0;
    //int extraSizeBorderBoundingBox = 0; // 1.5 /* ZLI /* scale
#ifdef def_segmentItem2_bboxcomputeMethod0
    _qrectf_boundingBox.setTop   ( (_qrectf_boundingBoxNoZLI.top()   /static_cast<qreal>(_currentZLI)) - penWidthDiv2);
    _qrectf_boundingBox.setLeft  ( (_qrectf_boundingBoxNoZLI.left()  /static_cast<qreal>(_currentZLI)) - penWidthDiv2);
    _qrectf_boundingBox.setBottom( (_qrectf_boundingBoxNoZLI.bottom()/static_cast<qreal>(_currentZLI)) + penWidthDiv2);
    _qrectf_boundingBox.setRight ( (_qrectf_boundingBoxNoZLI.right() /static_cast<qreal>(_currentZLI)) + penWidthDiv2);
#else
    qreal widthBorderZLI = _widthBorderZLI;
    _qrectf_boundingBox.setTop   ( (_qrectf_boundingBoxNoZLI.top()   /static_cast<qreal>(_currentZLI)) - widthBorderZLI);
    _qrectf_boundingBox.setLeft  ( (_qrectf_boundingBoxNoZLI.left()  /static_cast<qreal>(_currentZLI)) - widthBorderZLI);
    _qrectf_boundingBox.setBottom( (_qrectf_boundingBoxNoZLI.bottom()/static_cast<qreal>(_currentZLI)) + widthBorderZLI);
    _qrectf_boundingBox.setRight ( (_qrectf_boundingBoxNoZLI.right() /static_cast<qreal>(_currentZLI)) + widthBorderZLI);
#endif

     qDebug() << __FUNCTION__ << ": CustomGraphicsSegmentItem2, _qrectf_boundingBox top, left, bottom, right= "
                 << _qrectf_boundingBox.top()
                 << _qrectf_boundingBox.left()
                 << _qrectf_boundingBox.bottom()
                 << _qrectf_boundingBox.right();

    /*setLine(_qpfPointA.x(),
            _qpfPointA.y(),
            _qpfPointB.x(),
            _qpfPointB.y());*/

    //qDebug() << __FUNCTION__ << "SegmentItem: adjustedPointA = " << adjustedPointA;
    //qDebug() << __FUNCTION__ << "SegmentItem: adjustedPointB = " << adjustedPointB;

}

QRectF CustomGraphicsSegmentItem2::boundingRect() const {

    return(_qrectf_boundingBox);
}

void CustomGraphicsSegmentItem2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) {

    painter->save();

    qDebug() << __FUNCTION__ << ": CustomGraphicsSegmentItem2, _qrectf_boundingBox = " << _qrectf_boundingBox;

    //bool bUseAntialiasing = true;
    painter->setRenderHint(QPainter::Antialiasing, _bUseAntialiasing);

    //@#LP debug
    /*QBrush brush_debug(Qt::SolidPattern);
    brush_debug.setColor({128,0,0,128});
    painter->setBrush(brush_debug);
    painter->setPen(Qt::PenStyle::NoPen);
    painter->drawRect(static_cast<QRectF>(_qrectf_boundingBox));

    QBrush brush_debug2(Qt::NoBrush);
    painter->setBrush(brush_debug2);
    */

    painter->setPen(*_ptrUsedPen);
    painter->drawLine(
      QPointF{_pointAB[0].x(), _pointAB[0].y()},
      QPointF{_pointAB[1].x(), _pointAB[1].y()});

    /*painter->setPen(*_ptrUsedPen);
    painter->drawLine(_pointAB[0], _pointAB[1]);*/

    painter->restore();
}

void CustomGraphicsSegmentItem2::setColorNotHighlighted_Highlighted(const QColor &qCol_notHighlighted, const QColor &qCol_highlighted) {
    _pNotHighlighted.setColor(qCol_notHighlighted);
    _pHighlighted.setColor(qCol_highlighted);
    qDebug() << __FUNCTION__ << "(CustomGraphicsSegmentItem2) color: qCol_notHighlighted; qCol_highlighted = " << qCol_notHighlighted << ", " << qCol_notHighlighted;
    qDebug() << __FUNCTION__ << "(CustomGraphicsSegmentItem2) color: _pNotHighlighted; _pHighlighted       = " << _pNotHighlighted.color() << ", " << _pHighlighted.color();
    setHightlighted(_bShowAsHighlighted);
}

CustomGraphicsSegmentItem2::~CustomGraphicsSegmentItem2() {
    qDebug() << __FUNCTION__;
}

int CustomGraphicsSegmentItem2::type() const {
   // Enable the use of qgraphicsitem_cast with this item.
   return( e_customGraphicsItemType::e_cgit_CustomGraphicsSegmentItem2);
}

void CustomGraphicsSegmentItem2::adjustGraphicsItemWidth(qreal scaleOfViewSideForCurrentZLI) {

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


    QFile qfdebug("/tmp/segm2.txt");
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

    _pHighlighted.setCosmetic(_bUseCosmeticPen);
    _pHighlighted.setWidthF(_widthBorderZLI);

    _pNotHighlighted.setCosmetic(_bUseCosmeticPen);
    _pNotHighlighted.setWidthF(_widthBorderZLI);


}
