#include <QString>

#include <QDebug>

#include "../statusForFuturStateSwitch.h"

#include "Project_contentState.h"

Project_contentState::Project_contentState():
    _ePrj_routeContentState{e_Project_routeContentState::e_P_routeCS_noneRoute},
    _ePrj_boxesContentState{e_Project_boxesContentState::e_P_boxesCS_noneBox},
    _ePrj_computationParametersState{e_Project_computationParametersState::e_P_computationParametersS_notSet},
    _ePrj_stackedProfilComputedState{e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_noneComputed},
    _ePrj_stackedProfilEditedState{e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable},
    _ePrj_spatialResolutionState{e_Project_spatialResolutionState::e_P_stackedProfilEditedS_set}
{

}

void Project_contentState::clear() {
    _ePrj_routeContentState = e_Project_routeContentState::e_P_routeCS_noneRoute;
    _ePrj_boxesContentState = e_Project_boxesContentState::e_P_boxesCS_noneBox;
    _ePrj_computationParametersState = e_Project_computationParametersState::e_P_computationParametersS_notSet;
    _ePrj_stackedProfilComputedState = e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_noneComputed;
    _ePrj_stackedProfilEditedState = e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable;
    _ePrj_spatialResolutionState = e_Project_spatialResolutionState::e_P_stackedProfilEditedS_set;

}

void Project_contentState::setState_route(e_Project_routeContentState eAMrouteS) { _ePrj_routeContentState = eAMrouteS; }

void Project_contentState::setState_boxes(e_Project_boxesContentState eAMboxesS) { _ePrj_boxesContentState = eAMboxesS; }

void Project_contentState::setState_computationParameters(e_Project_computationParametersState eAMcomputationParametersS) { _ePrj_computationParametersState = eAMcomputationParametersS; }

void Project_contentState::setState_stackedProfilComputed(e_Project_stackedProfilComputedState eAMstackedProfilComputedS) { _ePrj_stackedProfilComputedState = eAMstackedProfilComputedS; }

void Project_contentState::setState_stackedProfilEdited(e_Project_stackedProfilEditedState eAMstackedProfilEditedS) { _ePrj_stackedProfilEditedState = eAMstackedProfilEditedS; }

void Project_contentState::setState_spatialResolution(e_Project_spatialResolutionState e_Prj_spatialResolutionState) { _ePrj_spatialResolutionState = e_Prj_spatialResolutionState; }


e_Project_routeContentState Project_contentState::getState_route() const { return(_ePrj_routeContentState); }

e_Project_boxesContentState Project_contentState::getState_boxes() const { return(_ePrj_boxesContentState); }

e_Project_computationParametersState Project_contentState::getState_computationParameters() const { return(_ePrj_computationParametersState); }

e_Project_stackedProfilComputedState Project_contentState::getState_stackedProfilComputed() const { return(_ePrj_stackedProfilComputedState); }

e_Project_stackedProfilEditedState Project_contentState::getState_stackedProfilEdited() const { return(_ePrj_stackedProfilEditedState); }

e_Project_spatialResolutionState Project_contentState::getState_spatialResolution() const { return(_ePrj_spatialResolutionState); }


S_StatusForFuturStateSwitch_withMessage Project_contentState::getStatusForFuturStateSwitch_withMessage(e_Project_routeContentState ePrj_routeS) const {

    S_StatusForFuturStateSwitch_withMessage sFFSSwMsg; //at e_SFFSS_rejected_noMsgForUser by default

    switch(ePrj_routeS) {
        case e_Project_routeContentState::e_P_routeCS_noneRoute:
        case e_Project_routeContentState::e_P_routeCS_oneRouteButInvalid:
        case e_Project_routeContentState::e_P_routeCS_oneRouteValid:
            break;

        case e_Project_routeContentState::e_P_routeCS_alteringRoute:
            if (_ePrj_boxesContentState == e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes) { //some boxes in project

                if (_ePrj_stackedProfilEditedState == e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession) {
                    sFFSSwMsg._qstrMessage = "Entering in trace edition will remove any existing boxes and edited stacked profils.";
                    sFFSSwMsg._qstrQuestion = "Do you confirm to enter in trace edition ?";
                    sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_butWarning_QuestionOKCancel;

                } else {
                    sFFSSwMsg._qstrMessage = "Entering in trace edition will remove any existing boxes.";
                    sFFSSwMsg._qstrQuestion = "Do you confirm to enter in trace edition ?";
                    sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_butWarning_QuestionOKCancel;
                }
            } else {
                sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_noMsgForUser;
            }
            break;
    }

    return(sFFSSwMsg);
}

S_StatusForFuturStateSwitch_withMessage Project_contentState::getStatusForFuturStateSwitch_withMessage(e_Project_boxesContentState ePrj_boxesS) const {

    S_StatusForFuturStateSwitch_withMessage sFFSSwMsg; //at e_SFFSS_rejected_noMsgForUser by default

    switch(ePrj_boxesS) {
        case e_Project_boxesContentState::e_P_boxesCS_noneBox: //if we request to go to nonebox; hence it's about removing all boxes

               if (_ePrj_boxesContentState == e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes) { //some boxes in project

                   if (  (_ePrj_stackedProfilComputedState == e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_mixedComputedAndNotComputed) //and some or all were computed
                       ||(_ePrj_stackedProfilComputedState == e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_allComputed)) {

                        sFFSSwMsg._qstrMessage = "Remove all boxes will remove all stacked profils.";
                        sFFSSwMsg._qstrQuestion = "Do you confirm this remove operation ?";
                        sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_butWarning_QuestionYesNo;

                   } else {
                       sFFSSwMsg._qstrMessage = "Remove all boxes: ";
                       sFFSSwMsg._qstrQuestion = "Do you confirm this remove operation ?";
                       sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_butWarning_QuestionYesNo;
                   }

               } else { //none box
                   sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_noMsgForUser;
               }
            break;

        case e_Project_boxesContentState::e_P_boxesCS_oneOrMoreBoxes:
            sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_noMsgForUser;
            break;
        }

    return(sFFSSwMsg);
}


S_StatusForFuturStateSwitch_withMessage Project_contentState::getStatusForFuturStateSwitch_withMessage(
        e_Project_computationParametersState ePrj_computationParametersS) const {

    S_StatusForFuturStateSwitch_withMessage sFFSSwMsg; //at e_SFFSS_rejected_noMsgForUser by default

    switch(ePrj_computationParametersS) {
        case e_Project_computationParametersState::e_P_computationParametersS_notSet: //this is an error to use this state here
        case e_Project_computationParametersState::e_P_computationParametersS_set: //this is an error to use this state here
            break;

        case e_Project_computationParametersState::e_P_computationParametersS_changed: //consider we reach here when there is at least one box (e_P_boxesCS_noneBox not applicable)

            if (_ePrj_stackedProfilEditedState == e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession) {
                sFFSSwMsg._qstrMessage = "Some stacked profiles can have edited values for linear regression model.";
                sFFSSwMsg._qstrQuestion = "Do you want to keep current values or reset them to default ?";
                sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_butWarning_QuestionResetKeepCancel;

            } else {
                if (_ePrj_stackedProfilEditedState == e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues/*e_P_stackedProfilEditedS_noneEdited_allSetAsDefaultValues*/) {
                    sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_noMsgForUser;
                } else { // e_P_stackedProfilEditedS_notApplicable
                    //#LP is a dev error ?
                }
            }
            break;
    }
    return(sFFSSwMsg);
}


S_StatusForFuturStateSwitch_withMessage Project_contentState::getStatusForFuturStateSwitch_withMessage(
        e_Project_stackedProfilComputedState ePrj_stackedProfilComputedS) const {

    S_StatusForFuturStateSwitch_withMessage sFFSSwMsg; //at e_SFFSS_rejected_noMsgForUser by default

    switch(ePrj_stackedProfilComputedS) {
        case e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_noneComputed://this is an error to use this state here
        case e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_mixedComputedAndNotComputed://this is an error to use this state here
        case e_Project_stackedProfilComputedState::e_P_stackedProfilComputedS_allComputed: //this is an error to use this state here
        break;
    }

    return(sFFSSwMsg);
}

S_StatusForFuturStateSwitch_withMessage Project_contentState::getStatusForFuturStateSwitch_withMessage(
        e_Project_stackedProfilEditedState ePrj_stackedProfilEditedS) const {

    S_StatusForFuturStateSwitch_withMessage sFFSSwMsg; //at e_SFFSS_rejected_noMsgForUser by default

    switch(ePrj_stackedProfilEditedS) {
        case e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable://this is an error to use this state here
        case e_Project_stackedProfilEditedState::/*e_P_stackedProfilEditedS_noneEdited_allSetAsDefaultValues*/e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues://this is an error to use this state here
        case e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession://this is an error to use this state here
        break;
    }

    return(sFFSSwMsg);
}

S_StatusForFuturStateSwitch_withMessage Project_contentState::getStatusForFuturStateSwitch_withMessage(e_Project_spatialResolutionState e_Prj_spatialResolutionState) const {

    S_StatusForFuturStateSwitch_withMessage sFFSSwMsg; //at e_SFFSS_rejected_noMsgForUser by default

    switch(e_Prj_spatialResolutionState) {
        case e_Project_spatialResolutionState::e_P_stackedProfilEditedS_notSet://this is an error to use this state here
        case e_Project_spatialResolutionState::e_P_stackedProfilEditedS_set://this is an error to use this state here
            break;

        case e_Project_spatialResolutionState::e_P_stackedProfilEditedS_changed:
            switch(_ePrj_stackedProfilEditedState) {
                case e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_notApplicable://this is an error to use this state here
                    break;
                case e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues:
                case e_Project_stackedProfilEditedState::e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession:
                    //sFFSSwMsg._qstrMessage = "";
                    //sFFSSwMsg._qstrQuestion = "";
                    //sFFSSwMsg._eStatusForFuturStateSwitch = e_SFFSS_acceptable_butWarning_QuestionOKCancel;
                    break;
            }
    }
    return(sFFSSwMsg);
}

void Project_contentState::showDebug_states() const {
    qDebug() << __FUNCTION__ << "_ePrj_routeContentState = " << static_cast<int>(_ePrj_routeContentState);
    qDebug() << __FUNCTION__ << "_ePrj_boxesContentState = " << static_cast<int>(_ePrj_boxesContentState);
    qDebug() << __FUNCTION__ << "_ePrj_computationParametersState = " << static_cast<int>(_ePrj_computationParametersState);
    qDebug() << __FUNCTION__ << "_ePrj_stackedProfilComputedState = " << static_cast<int>(_ePrj_stackedProfilComputedState);
    qDebug() << __FUNCTION__ << "_ePrj_stackedProfilEditedState = " << static_cast<int>(_ePrj_stackedProfilEditedState);
    qDebug() << __FUNCTION__ << "_ePrj_spatialResolutionState   = " << static_cast<int>(_ePrj_spatialResolutionState);
}
