#ifndef GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd_HPP
#define GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd_HPP

#include <QGraphicsItem>
#include <QDebug>

#include "../S_ForbiddenRouteSection.h"

//#include "CustomGraphicsTargetedPointItem.h"
//#include "CustomGraphicsSegmentItem.h"

#include "GraphicsRouteItem.h"
#include "../../core/S_Segment.h"

class Route;

class GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd: public GraphicsRouteItem {

    Q_OBJECT

public:
    //GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd();
    GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd(QGraphicsItem *parent=nullptr);
    ~GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd() override;

    bool initFromRouteAndFirstAndLastPointFromStartAndEnd(const Route& r, const S_ForbiddenRouteSectionsFromStartAndEnd& sForbiddenRSFromStartAndEnd/*,qreal minimalElementSideSize*/);

    int type() const override;

public slots:   

signals:

private:
    void insertCustomGraphicsSegmentItem_fromSegment(int idxSegmentInserted,  S_Segment segment,
                                                    Qt::GlobalColor colorHightlighted, Qt::GlobalColor colorNotHightlighted,
                                                    QGraphicsItem *parent);
private:

};

#endif // GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd_HPP
