#include <QDebug>

#include <QString>

#include "SGraphicsAppSettings_baseStruct.h"

void S_Color_Thickness::clear() {
    _qColor = {128,128,128};
    _dThickness = 1.0;
}

void S_Color_Thickness_Antialiasing::clear() {
    _qColor = {128,128,128};
    _dThickness = 1.0;
    _bAntialiasing= false;
}

void S_Color_Antialiasing::clear() {
    _qColor = {128,128,128};
    _bAntialiasing= false;
}

void S_Thickness_Antialiasing::clear() {
    _dThickness = 1.0;
    _bAntialiasing = false;
}
