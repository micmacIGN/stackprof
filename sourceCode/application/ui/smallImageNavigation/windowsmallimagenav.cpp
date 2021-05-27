#include "windowsmallimagenav.h"

#include "ui_windowsmallimagenav.h"

#include "../logic/model/smallImageNavScene/smallimagenav_customgraphicsscene.h"

#include <QDebug>

WindowSmallImageNav::WindowSmallImageNav(QWidget *parent) : QDialog(parent), ui(new Ui::WindowSmallImageNav) {

    ui->setupUi(this);

    connect(ui->graphicsView, &SmallImageNav_customGraphicsView::signal_zoomRequest_fromSmallImaveNav,
            this, &WindowSmallImageNav::signal_zoomRequest_fromSmallImaveNav); //direct forward

}

void WindowSmallImageNav::setCustomGraphicsScenePtr(SmallImageNav_CustomGraphicsScene *ptrSmallImageNav_CustomGraphicsScene) {
    _ptrCustomGraphicsScene = ptrSmallImageNav_CustomGraphicsScene;
    qDebug() << __FUNCTION__;
}

SmallImageNav_CustomGraphicsScene* WindowSmallImageNav::getCustomGraphicsScenePtr() {
    return(_ptrCustomGraphicsScene);
}

void WindowSmallImageNav::showEvent(QShowEvent *event) {
    resizeToMatchWithSmallImageNav();
}

void WindowSmallImageNav::resizeToMatchWithSmallImageNav() {
    qDebug() << __FUNCTION__;

    if (_ptrCustomGraphicsScene) {

        qDebug() << __FUNCTION__ << "_ptrCustomGraphicsScene->getSizeForWindowSmallImageNav() = " << _ptrCustomGraphicsScene->getSizeForWindowSmallImageNav();

        setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

        QSize sizeSmallImageNav = _ptrCustomGraphicsScene->getSizeForWindowSmallImageNav();

        qDebug() << __FUNCTION__ << "sizeSmallImageNav = " << sizeSmallImageNav;

        //considers no line as qframe border around qgraphicsview
        //but take into account the layout margin
        struct S_MarginLTRB {
            int _left;
            int _top;
            int _right;
            int _bottom;
        };
        S_MarginLTRB marginLTRB {4,4,4,4};

        qDebug() << __FUNCTION__ << "marginLTRB = " << marginLTRB._left << ", " << marginLTRB._top << ", " << marginLTRB._right << ", " << marginLTRB._bottom;

        QSize sizeWindowForSmallImageNav = { sizeSmallImageNav.width() + marginLTRB._left + marginLTRB._right,
                                             sizeSmallImageNav.height() + marginLTRB._top + marginLTRB._bottom };

        qDebug() << __FUNCTION__ << "sizeWindowForSmallImageNav = " << sizeWindowForSmallImageNav;

        resize(sizeWindowForSmallImageNav);

        qDebug() << __FUNCTION__ << "size() = " << size();

        setFixedSize(size());//avoid any user resize after small Image size fixed by code

    }
}

void WindowSmallImageNav::initConnectionModelViewSignalsSlots() {
    qDebug() << __FUNCTION__ << "(WindowSmallImageNav)";
    if (_ptrCustomGraphicsScene == nullptr) {
        qDebug() << __FUNCTION__ << "(WindowSmallImageNav)" << " _ptrCustomGraphicsScene is nullptr";
        return;
    }
    ui->graphicsView->setScene(_ptrCustomGraphicsScene);
}

WindowSmallImageNav::~WindowSmallImageNav() {
    qDebug() << __FUNCTION__;
    delete ui;
}

/*void WindowSmallImageNav::slot_debug_valueChanged(int v) {
    qDebug() << __FUNCTION__ << ": " << v;
}*/
