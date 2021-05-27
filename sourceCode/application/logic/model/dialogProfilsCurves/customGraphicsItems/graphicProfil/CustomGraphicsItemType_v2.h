#ifndef CUSTOMGRAPHICSITEMTYPE_V2_H
#define CUSTOMGRAPHICSITEMTYPE_V2_H

#include <QGraphicsItem>

enum e_customGraphicsItemType_part2 {

    //@#LP be careful to do not have conflit values with e_customGraphicsItemType

    e_cgit_CustomGraphicsProfilWithLRAL = QGraphicsItem::UserType +100,
    e_cgit_CustomGraphicsVerticalAdjuster,
    e_cgit_CustomLineMouseOver,
    e_cgit_coloredSquareMouseOver,

    e_cgit_CustomGraphicsItemCurve,
    e_cgit_CustomGraphicsItemEnvelop,

    e_cgit_CustomGraphicsItemDoubleSidesXAdjustersWithComputedLines,
    e_cgit_CustomGraphicsItemXAdjustersWithComputedLine,

    e_cgit_CustomGraphicsVerticalAdjusterWithCentereredGrip,
    e_cgit_CustomGraphicsLineItem,

    e_cgit_CustomLineNoEvent

};

#endif

