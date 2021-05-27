#include <QGraphicsView>
#include <QDebug>

#include <QMouseEvent>

#include "customgraphicsview_basic.h"

#include "../logic/model/dialogProfilsCurves/CustomGraphicsScene_profil.h"

CustomGraphicsView_basic::CustomGraphicsView_basic(QWidget *parent) : QGraphicsView(parent), _ptrCustomGraphicsScene_profil(nullptr) {

    qDebug() << __FUNCTION__;

    setMouseTracking(true); //to be able to have mouse event in the scene without the need to press a mouse button

}

void CustomGraphicsView_basic::setCustomGraphicsScenePtr(CustomGraphicsScene_profil *ptrCustomGraphicsScene_profil) {
    _ptrCustomGraphicsScene_profil = ptrCustomGraphicsScene_profil;

    //@#LP move this in a connectSignalSlot method
    connect(_ptrCustomGraphicsScene_profil, &CustomGraphicsScene_profil::signal_fitInView,
            this, &CustomGraphicsView_basic::slot_fitInView);
}

CustomGraphicsScene_profil *CustomGraphicsView_basic::customGraphicsScenePtr() {
    return(_ptrCustomGraphicsScene_profil);
}

void CustomGraphicsView_basic::resizeEvent(QResizeEvent *event) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic)";

    qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) " << viewport()->size();

    if (_ptrCustomGraphicsScene_profil) {
        qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) if (_ptrCustomGraphicsScene_profil) {";

        _ptrCustomGraphicsScene_profil->setParentPixelSizeForInit(
                    viewport()->size().width(),
                    viewport()->size().height());
        _ptrCustomGraphicsScene_profil->updateItems();

    } else {
        qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) _ptrCustomGraphicsScene is nullptr";
    }

    QGraphicsView::resizeEvent(event);
}

void CustomGraphicsView_basic::showEvent(QShowEvent* event) {
    qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic)";

    qDebug() << __FUNCTION__ << viewport()->size();


    if (_ptrCustomGraphicsScene_profil) {
         qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) if (_ptrCustomGraphicsScene_profil) {";

        _ptrCustomGraphicsScene_profil->setParentPixelSizeForInit(
                    viewport()->size().width(),
                    viewport()->size().height());
        _ptrCustomGraphicsScene_profil->updateItems();

    } else {
        qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) _ptrCustomGraphicsScene is nullptr";
    }

    qDebug() << __FUNCTION__ << " CustomGraphicsView_basic _ptrCustomQGScene->sceneRect() = " << sceneRect();

    QGraphicsView::showEvent(event);
}


void CustomGraphicsView_basic::updateOnDemand() {

    qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic)";

    qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) viewport()->size() = " << viewport()->size();

    if (_ptrCustomGraphicsScene_profil) {

         qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) if (_ptrCustomGraphicsScene_profil) {";

        _ptrCustomGraphicsScene_profil->setParentPixelSizeForInit(
                    viewport()->size().width(),
                    viewport()->size().height());
        _ptrCustomGraphicsScene_profil->updateItems();

    } else {
        qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) _ptrCustomGraphicsScene is nullptr";
    }

    qDebug() << __FUNCTION__ << "(CustomGraphicsView_basic) sceneRect() = " << sceneRect();

}


void CustomGraphicsView_basic::clear() {
    if (_ptrCustomGraphicsScene_profil) {
        _ptrCustomGraphicsScene_profil->clearContent();
    }
}


void CustomGraphicsView_basic::slot_fitInView() {
    //do nothing, remove me
}

