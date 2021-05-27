#ifndef ApplicationModel_contentState_h
#define ApplicationModel_contentState_h

#include <QString>

#include "../statusForFuturStateSwitch.h"

enum class e_Project_routeContentState {
    e_P_routeCS_noneRoute,
    e_P_routeCS_oneRouteButInvalid,
    e_P_routeCS_oneRouteValid,
    e_P_routeCS_alteringRoute,
};

enum class e_Project_boxesContentState {
    e_P_boxesCS_noneBox,
    e_P_boxesCS_oneOrMoreBoxes,
};

enum class e_Project_computationParametersState {
    e_P_computationParametersS_notSet,
    e_P_computationParametersS_set,
    e_P_computationParametersS_changed,
};

enum class e_Project_stackedProfilComputedState {
    e_P_stackedProfilComputedS_noneComputed,
    e_P_stackedProfilComputedS_mixedComputedAndNotComputed,
    e_P_stackedProfilComputedS_allComputed,
};

enum class e_Project_stackedProfilEditedState {
    e_P_stackedProfilEditedS_notApplicable,
    e_P_stackedProfilEditedS_mixedEditedAndNotEdited_butAtLeastSetToDefaultValues,
    e_P_stackedProfilEditedS_loadedFromFile_or_editedInSession,
};

enum class e_Project_spatialResolutionState {
    e_P_stackedProfilEditedS_notSet,
    e_P_stackedProfilEditedS_set,
    e_P_stackedProfilEditedS_changed,
};

class Project_contentState {

public:

    Project_contentState();
    void clear();

    void setState_route(e_Project_routeContentState ePrj_routeS);
    void setState_boxes(e_Project_boxesContentState ePrj_boxesS);
    void setState_computationParameters(e_Project_computationParametersState ePrj_computationParametersS);
    void setState_stackedProfilComputed(e_Project_stackedProfilComputedState ePrj_stackedProfilComputedS);
    void setState_stackedProfilEdited(e_Project_stackedProfilEditedState ePrj_stackedProfilEditedS);
    void setState_spatialResolution(e_Project_spatialResolutionState e_Prj_spatialResolutionState);

    e_Project_routeContentState getState_route() const;
    e_Project_boxesContentState getState_boxes() const;
    e_Project_computationParametersState getState_computationParameters() const;
    e_Project_stackedProfilComputedState getState_stackedProfilComputed() const;
    e_Project_stackedProfilEditedState getState_stackedProfilEdited() const;
    e_Project_spatialResolutionState getState_spatialResolution() const;

    S_StatusForFuturStateSwitch_withMessage getStatusForFuturStateSwitch_withMessage(e_Project_routeContentState ePrj_routeS) const;
    S_StatusForFuturStateSwitch_withMessage getStatusForFuturStateSwitch_withMessage(e_Project_boxesContentState ePrj_boxesS) const;
    S_StatusForFuturStateSwitch_withMessage getStatusForFuturStateSwitch_withMessage(e_Project_computationParametersState ePrj_computationParametersS) const;
    S_StatusForFuturStateSwitch_withMessage getStatusForFuturStateSwitch_withMessage(e_Project_stackedProfilComputedState ePrj_stackedProfilComputedS) const;
    S_StatusForFuturStateSwitch_withMessage getStatusForFuturStateSwitch_withMessage(e_Project_stackedProfilEditedState ePrj_stackedProfilEditedS) const;
    S_StatusForFuturStateSwitch_withMessage getStatusForFuturStateSwitch_withMessage(e_Project_spatialResolutionState e_Prj_spatialResolutionState) const;
    void showDebug_states() const;

private:

    e_Project_routeContentState _ePrj_routeContentState;

    e_Project_boxesContentState _ePrj_boxesContentState;

    e_Project_computationParametersState _ePrj_computationParametersState;

    e_Project_stackedProfilComputedState _ePrj_stackedProfilComputedState;

    e_Project_stackedProfilEditedState _ePrj_stackedProfilEditedState;

    e_Project_spatialResolutionState _ePrj_spatialResolutionState;
};

#endif //ApplicationModel_contentState_h

