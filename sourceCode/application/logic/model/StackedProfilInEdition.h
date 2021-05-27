#ifndef STACKEDPROFILINEDITION_H
#define STACKEDPROFILINEDITION_H

#include "core/ComputationCore_structures.h"

struct S_StackedProfilWithMeasurements;

struct S_StackedProfilInEdition {
    S_StackedProfilWithMeasurements *_stackedProfilWithMeasurementsPtr;
    int _boxId;
    S_MeasureIndex _measureIndex;
    S_StackedProfilInEdition();
    void clear();
};

struct S_GraphicTitleAndUnits {
    QString _strTitle;
    QString _strUnits;
};

#endif // STACKEDPROFILINEDITION_H
