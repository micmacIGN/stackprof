#include <QDebug>

#include "MenuStates.hpp"

#include <initializer_list>

MenuStates::MenuStates(QObject *parent): QObject(parent) {

}

void MenuStates::setState_on(std::initializer_list<e_Menu> initList_Menu) {
    for (auto iter: initList_Menu) {
        _qsetMenu_on.insert(iter);
        emit signal_setEnabled(iter, true);
    }
}

void MenuStates::setState_off(std::initializer_list<e_Menu> initList_Menu) {
    for (auto iter: initList_Menu) {
        //@LP prevent developper error about trying to disable the file Menu (we always needs the "exit" action enabled)
        if (iter == e_Menu::eMenu_file) {
            continue;
        }
        _qsetMenu_on.remove(iter);
        emit signal_setEnabled(iter, false);
    }
}
