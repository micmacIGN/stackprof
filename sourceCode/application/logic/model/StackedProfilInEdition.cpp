#include "StackedProfilInEdition.h"

S_StackedProfilInEdition::S_StackedProfilInEdition():
    _stackedProfilWithMeasurementsPtr(nullptr),
    _boxId(-1) {
}

void S_StackedProfilInEdition::clear() {
    _stackedProfilWithMeasurementsPtr = nullptr;
    _boxId = -1;
    _measureIndex.clear();
}

