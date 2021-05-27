#include <QDebug>

#include "MenuActionStates.hpp"

#include <initializer_list>

MenuActionStates::MenuActionStates(QObject *parent): QObject(parent) {

}

//@LP API not nice; using many parameters to limit the range. Considered ok for the app goal.
void MenuActionStates::setState_on_emitForAll(MenuStates::e_Menu eMenu,
                                              std::initializer_list<e_menuAction> initList_menuAction,
                                              e_menuAction eMenuActionLastAction) {

    QSet<e_menuAction> qsetMenuAction_on;
    //void MenuActionStates::setState_on_emitForAll(std::initializer_list<e_menuAction> initList_menuAction) {
    for (auto iter: initList_menuAction) {
        qDebug() << __FUNCTION__ << "iter on initList_menuAction: insert now:" << iter;
        qsetMenuAction_on.insert(iter);
        qDebug() << __FUNCTION__ << "iter on initList_menuAction: emit now: signal_setMenuActionEnabled( " << iter << ", true)";
        emit signal_setMenuActionEnabled(iter, true);
    }

    for (int ieMA = eMenu; ieMA <= eMenuActionLastAction; ieMA++) {

            qDebug() << __FUNCTION__ << "check qsetMenuAction_on.contains(e_menuAction: " << ieMA;
        if (!qsetMenuAction_on.contains(static_cast<e_menuAction>(ieMA))) {
            qDebug() << __FUNCTION__ << "  yes qsetMenuAction_on.contains(e_menuAction: " << ieMA;;
            qDebug() << __FUNCTION__ << "  => emit now: signal_setMenuActionEnabled    (" << ieMA << ", false)";
            emit signal_setMenuActionEnabled(ieMA, false);
        } else {
            qDebug() << __FUNCTION__ << "  doest not contains e_menuAction: " << ieMA;
        }
    }
}

//@LP API not nice; using many parameters to limit the range. Considered ok for the app goal.
void MenuActionStates::setState_off_emitForAll(MenuStates::e_Menu eMenu,
                                              std::initializer_list<e_menuAction> initList_menuAction,
                                              e_menuAction eMenuActionLastAction) {
    QSet<e_menuAction> qsetMenuAction_on;
    for (auto iter: initList_menuAction) {
        qsetMenuAction_on.remove(iter);
        emit signal_setMenuActionEnabled(iter, false);
    }

    for (int ieMA = eMenu; ieMA <= eMenuActionLastAction; ieMA++) {
        if (qsetMenuAction_on.contains(static_cast<e_menuAction>(ieMA))) {
            emit signal_setMenuActionEnabled(ieMA, true);
        }
    }

}

void MenuActionStates::setState_on(std::initializer_list<e_menuAction> initList_menuAction) {
    for (auto iter: initList_menuAction) {
        emit signal_setMenuActionEnabled(iter, true);
    }
}

void MenuActionStates::setState_off(std::initializer_list<e_menuAction> initList_menuAction) {
    for (auto iter: initList_menuAction) {
        emit signal_setMenuActionEnabled(iter, false);
    }
}
