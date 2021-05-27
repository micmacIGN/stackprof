#ifndef MenuStates_HPP
#define MenuStates_HPP

#include <QObject>
#include <QSet>

//enable or disable menu on request
class MenuStates : public QObject {

    Q_OBJECT

    public:
       MenuStates(QObject *parent = nullptr);

       static constexpr int _cstExpr_jump_between_firstActionByMenu = 50;

       enum e_Menu {

           eMenu_none,

           eMenu_file,
           eMenu_preferences         = eMenu_file        + _cstExpr_jump_between_firstActionByMenu,
           eMenu_routeSet            = eMenu_preferences + _cstExpr_jump_between_firstActionByMenu,
           eMenu_routeInsideProject  = eMenu_routeSet    + _cstExpr_jump_between_firstActionByMenu,
           eMenu_stackedProfilBoxes  = eMenu_routeInsideProject + _cstExpr_jump_between_firstActionByMenu,
           eMenu_computation         = eMenu_stackedProfilBoxes + _cstExpr_jump_between_firstActionByMenu,
           eMenu_stackedProfils      = eMenu_computation    + _cstExpr_jump_between_firstActionByMenu,
           eMenu_view                = eMenu_stackedProfils + _cstExpr_jump_between_firstActionByMenu,

           eMenu_count_canBeDisabled = eMenu_view  + _cstExpr_jump_between_firstActionByMenu,

           eMenu_first = eMenu_file
        };

  //emit only for on
  void setState_on(std::initializer_list<e_Menu> initList_menu);
  //emit only for on
  void setState_off(std::initializer_list<e_Menu> initList_menu);

  //void setState_on_emitForAll(std::initializer_list<e_menuAction> initList_menuAction);

  signals:
    void signal_setEnabled(int ieMenu, bool enabled);

  private:
    QSet<e_Menu> _qsetMenu_on;

};

#endif


