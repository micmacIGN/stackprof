#ifndef CUSTOMGRAPHICSSQUAREITEM_HPP
#define CUSTOMGRAPHICSSQUAREITEM_HPP

#include <QGraphicsRectItem>
#include <QPen>
#include <QColor>
#include <QBrush>

#include <QDebug>

class CustomGraphicsSquareItem: public QGraphicsRectItem {

public:
    //CustomGraphicsSquareItem();
    CustomGraphicsSquareItem(QPointF location, int sizeNoZLI, QGraphicsItem *parent=nullptr);

    void setBrushColorAsUnselected();
    void setBrushColorAsSelected();

    void adjustLocationAndSizeToZLI(int ZLI);

    int type() const override;

    ~CustomGraphicsSquareItem() override;

private:
    QPen _p;

    QPointF _qpointf_locationNoZLI;
    QSize _qsize_sideSizeNoZLI;

};

#endif // CUSTOMGRAPHICSSQUAREITEM_HPP
