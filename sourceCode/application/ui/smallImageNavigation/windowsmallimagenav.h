#ifndef WindowSmallImageNav_H
#define WindowSmallImageNav_H

#include <QDialog>

#include <QGraphicsRectItem>

//class SmallImageNav_CustomGraphicsScene;
class SmallImageNav_CustomGraphicsScene;

namespace Ui {
class WindowSmallImageNav;
}

class WindowSmallImageNav : public QDialog
{
    Q_OBJECT

public:
    explicit WindowSmallImageNav(QWidget *parent = nullptr);

    void setCustomGraphicsScenePtr(SmallImageNav_CustomGraphicsScene *ptrSmallImageNav_CustomGraphicsScene);
    SmallImageNav_CustomGraphicsScene* getCustomGraphicsScenePtr();

    void initConnectionModelViewSignalsSlots();

    ~WindowSmallImageNav() override;

    void showEvent(QShowEvent *event) override;

    void resizeToMatchWithSmallImageNav();

signals:
    void signal_zoomRequest_fromSmallImaveNav(int i_e_ZoomDir/*, QPointF mouseViewPosMappedToScene*/);

/*public slots:
    void slot_debug_valueChanged(int v);*/

private:
    Ui::WindowSmallImageNav *ui;

    SmallImageNav_CustomGraphicsScene* _ptrCustomGraphicsScene;
};


#endif // WindowSmallImageNav_H
