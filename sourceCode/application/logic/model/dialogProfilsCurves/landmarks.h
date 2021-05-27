#ifndef LANDMARKINRANGE_H
#define LANDMARKINRANGE_H

#include <QVector>

class LandmarkInRange {

public:
    LandmarkInRange();
    void set(double min_of_range, double max_of_range);

    //LandmarkInRange(double min_of_range, double max_of_range);

    bool compute(double bigStepFraction, double smallStepFraction, bool bComputeFromZero);
    
    //compute_fixedSteps_bigDiv_2_4_5_10_20__smallDiv_5() try different scale to compute big landmarks values
    //and it select the scale with enough big landmarks but not too much to let space for the small landmarks
    //the smallllandmark are sync with division by 5 of the biglandmarks
    bool compute_fixedSteps_bigDiv_2_100__smallDiv_5();

    bool compute_fixedSteps_min0max_with_intBigDiv_2_3_4_5__intSmallDiv_v2();

    const QVector<double>& getBigLandMarksRef();
    const QVector<double>& getSmallLandMarksRef();

private:

    double _min_of_range;
    double _max_of_range;

    bool _bComputed;

    QVector<double> _qvect_bigLandMarks;
    QVector<double> _qvect_smallLandMarks;

    QVector<double> makeLandMarksDividingRangesByFive(double min, double max, bool bKeepMinMaxInLandmarkOuput);

};

QString valueToPoweredString(double v, double smallest_basePowerForDisplay, double biggest_basePowerForDisplay);

QVector<QString> valuesToPowerString_withUniformPower(const QVector<double> &qvectValuesSortedMinToMax);

void test_qstringformat(const QString& strDebug, double v);

#endif // LANDMARKINRANGE_H
