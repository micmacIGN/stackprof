#ifndef CUSTOMGRAPHICSITEMTYPES_H
#define CUSTOMGRAPHICSITEMTYPES_H

#include <QGraphicsItem>

enum e_customGraphicsItemType {

    e_cgit_CustomGraphicsSegmentItem = QGraphicsItem::UserType + 1,
    e_cgit_CustomGraphicsSquareItem,
    e_cgit_CustomGraphicsTargetPointItem,
    e_cgit_CustomGraphicsTinyTargetPointItem,
    e_cgit_GraphicsRouteItem,
    e_cgit_CustomGraphicsVectorDirectionAtPointItem,
    e_cgit_CustomGraphicsBoxItem,
    e_cgit_CustomGraphicsRectangularHighlightItem,
    e_cgit_CustomGraphicsTinyTargetPointItem_notDraggable,
    e_cgit_GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd,
    e_cgit_CustomGraphicsTargetPointItem_noMouseInteraction_triangle,

    e_cgit_CustomGraphicsSpecificItemsOfInterest,

    e_cgit_CustomGraphicsBoxItem2,

    e_cgit_CustomGraphicsSegmentItem2
};

#endif // CUSTOMGRAPHICSITEMTYPES_H
