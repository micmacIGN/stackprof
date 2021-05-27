#ifndef VALUEDEQUANTIZATION_HPP
#define VALUEDEQUANTIZATION_HPP

#include "ComputationCore_structures.h" //@#LP just for S_FloatWithValidityFlag

class ValueDequantization { //@#LP rename class

public:
    ValueDequantization();
    void clear();

    void set_micmacStep(float fMicmacStep);
    void set_spatialResolution(float fSpatialResolution);

    bool isDequantValid() const;
    float dequantize(float fValue);

    float get_micmacStep() const;
    float get_spatialResolution() const;

    float get_dequant() const;

    bool isMicmacStepValid() const;
    bool isSpatialResolutionValid() const;

    bool toQJsonObject(QJsonObject& qjsonObj) const;
    bool fromQJsonObject(const QJsonObject& qjsonObj);

private:    

    void updateDequantValidityAndValue();

    S_FloatWithValidityFlag _SFwvf_micmacStep;
    S_FloatWithValidityFlag _SFwvf_spatialResolution;

    S_FloatWithValidityFlag _SFwvf_dequant;

};

#endif // VALUEDEQUANTIZATION_HPP
