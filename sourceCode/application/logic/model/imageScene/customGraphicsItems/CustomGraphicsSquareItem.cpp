#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

#include "CustomGraphicsSquareItem.h"

#include "CustomGraphicsItemTypes.h"

CustomGraphicsSquareItem::CustomGraphicsSquareItem(QPointF location, int sideSizeNoZLI, QGraphicsItem *parent): QGraphicsRectItem(parent),
    _qpointf_locationNoZLI(location),
    _qsize_sideSizeNoZLI{sideSizeNoZLI, sideSizeNoZLI}


{
    qDebug() << __FUNCTION__;

    _p.setStyle(Qt::PenStyle::SolidLine);
    _p.setColor(Qt::green);
    _p.setCosmetic(true);
    setPen(_p);

    setBrush(Qt::NoBrush);

    adjustLocationAndSizeToZLI(1);
    setVisible(true);
}

void CustomGraphicsSquareItem::adjustLocationAndSizeToZLI(int ZLI) {

    qDebug() << __FUNCTION__ << "(CustomGraphicsSquareItem) received ZLI = " << ZLI;

    QRectF adjustedToZLI;
    adjustedToZLI.setTopLeft(_qpointf_locationNoZLI/static_cast<qreal>(ZLI));
    adjustedToZLI.setSize(_qsize_sideSizeNoZLI/static_cast<qreal>(ZLI));
    setRect(adjustedToZLI);
}

int CustomGraphicsSquareItem::type() const {
   // Enable the use of qgraphicsitem_cast with this item.

   return(e_customGraphicsItemType::e_cgit_CustomGraphicsSquareItem);
}

CustomGraphicsSquareItem::~CustomGraphicsSquareItem() {
    qDebug() << __FUNCTION__;
}



