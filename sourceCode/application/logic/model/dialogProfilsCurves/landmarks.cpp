#include <QCoreApplication>
#include <QDebug>
#include <QVector>

#include <iostream>
using namespace std;
#include <math.h>

#include <qmath.h>

#include <vector>

#include "../../toolbox/toolbox_math.h"

#include "landmarks.h"

LandmarkInRange::LandmarkInRange():
    _min_of_range(.0),
    _max_of_range(1.0),
    _bComputed(false) {

}

void LandmarkInRange::set(double min_of_range, double max_of_range) {
    _min_of_range = min_of_range;
    _max_of_range = max_of_range;
}

bool LandmarkInRange::compute(double bigStepFraction, double smallStepFraction, bool bComputeFromZero) {

    _bComputed = false;
    _qvect_bigLandMarks.clear();
    _qvect_smallLandMarks.clear();

    if ( (bigStepFraction < 2.) || (smallStepFraction < 2.) ) {
        qDebug() << __FUNCTION__ << " error: bigStepFraction or smallStepFraction too small";
        return(false);
    }
    if ( (bigStepFraction > 50.) || (smallStepFraction > 50.) ) { //50 should be larger as limit for graphical axis landmark usage
        qDebug() << __FUNCTION__ << " error: bigStepFraction or smallStepFraction too big";
        return(false);
    }

    double range = qAbs(_max_of_range - _min_of_range);

    //if zero is in the range, then compute landmark from zero on each side (< 0 and >0)
    //@#LP remove this constraint, we have to permit to compute from 0 without zero in the range
    if (bComputeFromZero) {
        //add a check to avoid incorrect use from the developper
        if (!((_max_of_range > .0) && (_min_of_range < .0))) {
            bComputeFromZero = false;
            qDebug() << __FUNCTION__ << "warning: can not compute from zero without zero in the range";
        }
    }

    if (bComputeFromZero) {

        qDebug() << __FUNCTION__ << "bComputeFromZero: " << bComputeFromZero;

        //negative side
        double negMin = _min_of_range;
        double negMax = .0;

        double upperStepValue = negMax;
        double lowerStepValue = negMin;
        qDebug() << __FUNCTION__ << "upperStepValue: " << upperStepValue;
        qDebug() << __FUNCTION__ << "lowerStepValue: " << lowerStepValue;

        double tenPoweredDivBigStepFraction = qAbs(upperStepValue - lowerStepValue) / bigStepFraction;
        //qDebug() << "  tenPoweredDivStepFraction = " << tenPoweredDivStepFraction;


        for (double vSteppedBig = lowerStepValue;
             vSteppedBig <= upperStepValue;
             vSteppedBig += tenPoweredDivBigStepFraction ) {


              _qvect_bigLandMarks << vSteppedBig;
        }

        //positive side
        double posMin = .0;
        double posMax = _max_of_range;

        upperStepValue = posMax;
        lowerStepValue = posMin;
        qDebug() << __FUNCTION__ << "upperStepValue: " << upperStepValue;
        qDebug() << __FUNCTION__ << "lowerStepValue: " << lowerStepValue;

        tenPoweredDivBigStepFraction = qAbs(upperStepValue - lowerStepValue) / bigStepFraction;
        //qDebug() << "  tenPoweredDivStepFraction = " << tenPoweredDivStepFraction;


        for (double vSteppedBig = lowerStepValue+tenPoweredDivBigStepFraction; //+tenPoweredDivBigStepFraction to avoid insertion of a second zero
             vSteppedBig <= upperStepValue;
             vSteppedBig += tenPoweredDivBigStepFraction ) {


              _qvect_bigLandMarks << vSteppedBig;
        }

        qDebug() << "  _qvect_bigLandMarks = " << _qvect_bigLandMarks;

        double tenPoweredDivSmallStepFraction = tenPoweredDivBigStepFraction / smallStepFraction;

        for(auto iterBigStep = _qvect_bigLandMarks.begin(); iterBigStep < _qvect_bigLandMarks.end()-1; ++iterBigStep) {
            double vlow = *iterBigStep;
            double vhigh = *(iterBigStep+1);
            //qDebug() << "vlow  = " << vlow;
            //qDebug() << "vhigh = " << vhigh;

            for (auto smallStep = vlow+tenPoweredDivSmallStepFraction;
                smallStep < vhigh;
                smallStep+= tenPoweredDivSmallStepFraction) {
                _qvect_smallLandMarks << smallStep;
            }
        }

    } else {

        qDebug() << __FUNCTION__ << "bComputeFromZero: " << bComputeFromZero;
        qDebug() << __FUNCTION__;
        qDebug() << __FUNCTION__ << "min, max: " << _min_of_range << ", " << _max_of_range;
        qDebug() << __FUNCTION__ << " range = " << range;
        //qDebug() << __FUNCTION__ << " absPowerOfTen_range = " << absPowerOfTen_range;

        double _powerOfTen_ofRange = .0;

        _powerOfTen_ofRange = powerOfTen(range);
        qDebug() << __FUNCTION__ << "  powerOfTen_range: " << _powerOfTen_ofRange;

        double tenPowered_range = qPow(10,_powerOfTen_ofRange);
        qDebug() << __FUNCTION__ << "  tenPowered_range = " << tenPowered_range;

        /*qDebug() << __FUNCTION__ << "-----";
        for (double testpower = 0.000001; testpower < 1000000; testpower*=10) {
            qDebug() << __FUNCTION__ << "--- powerOfTen( " << testpower << " ) = " << powerOfTen(testpower);
        }
        qDebug() << __FUNCTION__ << "-----";*/

        double upperStepValue = findUpperStepValueFrom(_max_of_range, tenPowered_range, bigStepFraction);
        double lowerStepValue = findLowerStepValueFrom(_min_of_range, tenPowered_range, bigStepFraction);

        //trying without power range:

        qDebug() << __FUNCTION__ << "upperStepValue: " << upperStepValue;
        qDebug() << __FUNCTION__ << "lowerStepValue: " << lowerStepValue;

        double tenPoweredDivBigStepFraction = qAbs(upperStepValue - lowerStepValue) / bigStepFraction;
        qDebug() << "  tenPoweredDivBigStepFraction = " << tenPoweredDivBigStepFraction;


        for (double vSteppedBig = lowerStepValue;
             vSteppedBig   < upperStepValue + tenPoweredDivBigStepFraction;
             vSteppedBig += tenPoweredDivBigStepFraction ) {

            qDebug() << __FUNCTION__ << "looping with vSteppedBig : " << vSteppedBig;

            bool bInsertCloseToMin = false;
            bool bInsertCloseToMax = false;

            //@#LP TBC if useful or not:
            if ((!bInsertCloseToMin)&&(!bInsertCloseToMax)) {
                //@#LP check that these cases are possible to be reached with the new implementation:
                if (vSteppedBig < _min_of_range)  { qDebug() << __FUNCTION__ << "vSteppedBig < _min_of_range: "<< vSteppedBig << " < " << _min_of_range; continue;}
                if (vSteppedBig > _max_of_range)  { qDebug() << __FUNCTION__ << "vSteppedBig > _max_of_range: "<< vSteppedBig << " > " << _max_of_range; continue;}
            }

            if (bInsertCloseToMin) {
                qDebug() << __FUNCTION__ << "insert lowerStepValue (" << lowerStepValue;
                _qvect_bigLandMarks << lowerStepValue;
            } else {
                if (bInsertCloseToMax) {
                    qDebug() << __FUNCTION__ << "insert upperStepValue" << upperStepValue;
                    _qvect_bigLandMarks << upperStepValue;
                }
            }
            if ((!bInsertCloseToMin)&&(!bInsertCloseToMax)) {
                qDebug() << __FUNCTION__ << "insert vSteppedBig (" << vSteppedBig;
                _qvect_bigLandMarks << vSteppedBig;
            }
        }
        qDebug() << __FUNCTION__ << "_qvect_bigLandMarks = " << _qvect_bigLandMarks;

        double tenPoweredDivSmallStepFraction = tenPoweredDivBigStepFraction / smallStepFraction;

        for(auto iterBigStep = _qvect_bigLandMarks.begin(); iterBigStep < _qvect_bigLandMarks.end()-1; ++iterBigStep) {
            double vLow = *iterBigStep;
            double vHigh = *(iterBigStep+1);
            qDebug() << "vLow  = " << vLow;
            qDebug() << "vHigh = " << vHigh;

            for (auto smallStep = vLow+tenPoweredDivSmallStepFraction;
                smallStep < vHigh;
                smallStep+= tenPoweredDivSmallStepFraction) {
                qDebug() << "add smallStep: " << smallStep;
                _qvect_smallLandMarks << smallStep;
            }
        }

        if (_qvect_bigLandMarks.count()) {
            //add the smallLandmarks from upper to max if needs
            for(double vLow = _qvect_bigLandMarks.first() - tenPoweredDivSmallStepFraction;
                vLow > _min_of_range; vLow-=tenPoweredDivSmallStepFraction) {
                _qvect_smallLandMarks << vLow;
            }

            //add the smallLandmarks from big to max if needs
            for(double vLow = _qvect_bigLandMarks.last() + tenPoweredDivSmallStepFraction;
                vLow < _max_of_range; vLow+=tenPoweredDivSmallStepFraction) {
               _qvect_smallLandMarks << vLow;
            }
        }
    }

    qDebug() << __FUNCTION__ << "_max_of_range  = " << _max_of_range;
    qDebug() << __FUNCTION__ << " _min_of_range = " << _min_of_range;
    qDebug() << __FUNCTION__ << "_qvect_bigLandMarks   = " << _qvect_bigLandMarks;
    qDebug() << __FUNCTION__ << "_qvect_smallLandMarks = " << _qvect_smallLandMarks;

    _bComputed = true;
    return(true);
}



#include <OpenImageIO/fmath.h>
using namespace OIIO;

bool LandmarkInRange::compute_fixedSteps_bigDiv_2_100__smallDiv_5() {

    //@#LP add test about max = min, hence return false
    /*
    qDebug() << __FUNCTION__ << "-----";
    for (double testpower = 0.000001; testpower < 1000000; testpower*=10.0) {
        qDebug() << __FUNCTION__ << "--- powerOfTen( " << testpower << " ) = " << powerOfTen(testpower);
    }
    qDebug() << __FUNCTION__ << "-----";
    */

    double powerOfTen_of_range = powerOfTen(_max_of_range-_min_of_range);
    //qDebug() << __FUNCTION__ << "powerOfTen_of_range= " << powerOfTen_of_range;

    double powerOfTen_for_010search =  powerOfTen_of_range+1.0;
    //qDebug() << __FUNCTION__ << " powerOfTen_for_010search = " << powerOfTen_for_010search;

    QVector<double> qvect_stepDividerCandidates_forBigLandMarksSearch { 2.0,  4.0,  5.0, 10.0 };
    double maxTenMultiplyer_stepDividerCandidates = 10.0;

    bool bBigLandMarksSpreadFound = false;

    double bigLandMarkStep = 1.0; //1.0 as default invalid value, bur avoid 0 to avoid infinite loop later, in any case

    for (double tenMultiplyer_stepDividerCandidates = 1.0;
                tenMultiplyer_stepDividerCandidates <= maxTenMultiplyer_stepDividerCandidates;
                tenMultiplyer_stepDividerCandidates *= 10.0)  {

        //qDebug() << __FUNCTION__ << "try with tenMultiplyer_stepDividerCandidates = " << tenMultiplyer_stepDividerCandidates;

        for (auto stepDividerCandidate: qvect_stepDividerCandidates_forBigLandMarksSearch) {

            _bComputed = false;
            _qvect_bigLandMarks.clear();
            _qvect_smallLandMarks.clear();

            double dividerCandidate = stepDividerCandidate * tenMultiplyer_stepDividerCandidates;
            //qDebug() << __FUNCTION__ << " try with dividerCandidate = " << dividerCandidate;

            bigLandMarkStep =  qPow(10,powerOfTen_for_010search)/dividerCandidate;
            //qDebug() << __FUNCTION__ << " bigLandMarkStep = " << bigLandMarkStep;

            double lowerStepValue_inStepValue = findLowerStepValue(_min_of_range, bigLandMarkStep);
            double upperStepValue_inStepValue = findUpperStepValue(_max_of_range, bigLandMarkStep);
            //qDebug() << __FUNCTION__ << "lowerStepValue_inDividerStepValue = " << lowerStepValue_inStepValue;
            //qDebug() << __FUNCTION__ << "upperStepValue_inDividerStepValue = " << upperStepValue_inStepValue;

            for (double bigLandMark  = lowerStepValue_inStepValue; //lowerStepValue_in010plus1;
                        bigLandMark <= upperStepValue_inStepValue; //upperStepValue_inStepValue;
                        bigLandMark+=bigLandMarkStep) {
                //qDebug() << __FUNCTION__ << "bigLandMark = " << bigLandMark;
                if (bigLandMark >= _min_of_range) {
                    if (bigLandMark <= _max_of_range) {
                        if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(bigLandMark)) {
                            _qvect_bigLandMarks << .0;
                            //qDebug() << "bigLandMark value replaced by zero";
                        } else {
                            _qvect_bigLandMarks << bigLandMark;
                            //qDebug() << "bigLandMark value considered not very close to zero";
                        }
                    } else {
                         //qDebug() << "bigLandMark > _max_of_range => rejected";
                    }
                } else {
                    //qDebug() << "bigLandMark < _min_of_range => rejected";
                }
            }

            //qDebug() << __FUNCTION__ << "_qvect_bigLandMarks = " << _qvect_bigLandMarks;

            if (_qvect_bigLandMarks.count() >= 3) {
                //>3 considered as enough big landmarks for interesting big landmarks display
                bBigLandMarksSpreadFound = true;
                break;
            } else {
                //qDebug() << __FUNCTION__ << "considered as not enough";
            }
        }
        if (bBigLandMarksSpreadFound) {
            break;
        }
    }

    if (!bBigLandMarksSpreadFound) {
        //qDebug() << __FUNCTION__ << "warning: range too big to compute bigLandMarks";
        return(true);
    }

    //qDebug() << __FUNCTION__ << "*** final: _qvect_bigLandMarks = " << _qvect_bigLandMarks;

    double smallLandMarkStep = bigLandMarkStep / 5.0;
    //qDebug() << __FUNCTION__ << " smallLandMarkStep = " << smallLandMarkStep;

    for(auto iterBigStep = _qvect_bigLandMarks.begin(); iterBigStep < _qvect_bigLandMarks.end()-1; ++iterBigStep) {
        double vLow = *iterBigStep;
        double vHigh = *(iterBigStep+1);
        //qDebug() << "vLow  = " << vLow;
        //qDebug() << "vHigh = " << vHigh;

        for (auto smallStep = vLow+smallLandMarkStep;
            smallStep < vHigh;
            smallStep+= smallLandMarkStep) {
            //qDebug() << "add smallStep: " << smallStep;
            if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(smallStep-vHigh)) {
                //do not push the value if too close to vHigh
            } else {
                //fprintf(stdout, "[[%.14f]]\n", smallStep);
                _qvect_smallLandMarks << smallStep;
            }
        }
    }

    qDebug() << __FUNCTION__ << "*** without remaining: _qvect_smallLandMarks = " << _qvect_smallLandMarks;

    if (_qvect_bigLandMarks.count()) {
        //qDebug() << "adding remaining from upper to max:";
        //add the smallLandmarks from upper to max if needs
        for(double vLow = _qvect_bigLandMarks.first() - smallLandMarkStep;
            vLow > _min_of_range; vLow-=smallLandMarkStep) {
            _qvect_smallLandMarks << vLow;
             //qDebug() << "added as remaining: " << vLow;
        }

        //qDebug() << "adding remaining from lower to min:";
        //add the smallLandmarks from lower to min if needs
        for(double vLow = _qvect_bigLandMarks.last() + smallLandMarkStep;
            vLow < _max_of_range; vLow+=smallLandMarkStep) {
           _qvect_smallLandMarks << vLow;
           //qDebug() << "added as remaining: " << vLow;
        }
    }

    qDebug() << __FUNCTION__ << "*** final: _qvect_bigLandMarks = " << _qvect_bigLandMarks;
    qDebug() << __FUNCTION__ << "*** final: _qvect_smallLandMarks = " << _qvect_smallLandMarks;

    return(true);
}

//---------
//v2
bool LandmarkInRange::compute_fixedSteps_min0max_with_intBigDiv_2_3_4_5__intSmallDiv_v2() {

    /*
    qDebug() << __FUNCTION__ << "-----";
    for (double testpower = 0.000001; testpower < 1000000; testpower*=10.0) {
        qDebug() << __FUNCTION__ << "--- powerOfTen( " << testpower << " ) = " << powerOfTen(testpower);
    }
    qDebug() << __FUNCTION__ << "-----";
    */

    double powerOfTen_of_range = powerOfTen(_max_of_range-_min_of_range);
    //qDebug() << __FUNCTION__ << "powerOfTen_of_range= " << powerOfTen_of_range;

    double powerOfTen_for_010search =  powerOfTen_of_range+1.0;
    //qDebug() << __FUNCTION__ << " powerOfTen_for_010search = " << powerOfTen_for_010search;

    QVector<double> qvect_stepDividerCandidates_forBigLandMarksSearch { 2.0,  4.0,  5.0, 10.0 };
    double maxTenMultiplyer_stepDividerCandidates = 10.0;

    bool bBigLandMarksSpreadFound = false;

    double bigLandMarkStep = 1.0; //1.0 as default invalid value, bur avoid 0 to avoid infinite loop later, in any case

    for (double tenMultiplyer_stepDividerCandidates = 1.0;
                tenMultiplyer_stepDividerCandidates <= maxTenMultiplyer_stepDividerCandidates;
                tenMultiplyer_stepDividerCandidates *= 10.0)  {

        //qDebug() << __FUNCTION__ << "try with tenMultiplyer_stepDividerCandidates = " << tenMultiplyer_stepDividerCandidates;

        for (auto stepDividerCandidate: qvect_stepDividerCandidates_forBigLandMarksSearch) {

            _bComputed = false;
            _qvect_bigLandMarks.clear();
            _qvect_smallLandMarks.clear();

            double dividerCandidate = stepDividerCandidate * tenMultiplyer_stepDividerCandidates;
            //qDebug() << __FUNCTION__ << " try with dividerCandidate = " << dividerCandidate;

            bigLandMarkStep =  qPow(10,powerOfTen_for_010search)/dividerCandidate;
            //qDebug() << __FUNCTION__ << " bigLandMarkStep = " << bigLandMarkStep;

            int texel = 0;
            floorfrac(static_cast<float>(bigLandMarkStep), &texel);
            if (!texel) {
                //qDebug() << __FUNCTION__ << "bigLandMarkStep is zero integer, to small as candidate, and another better candidate not possible";
                break;
            } else {
                if (!valueIsCloseToZero_deltaLowerThan1ExpMinus14(static_cast<double>(texel) - bigLandMarkStep)) {
                    //qDebug() << __FUNCTION__ << "bigLandMarkStep not integer, continue looping";
                    continue;
                }
            }

            double lowerStepValue_inStepValue = findLowerStepValue(_min_of_range, bigLandMarkStep);
            double upperStepValue_inStepValue = findUpperStepValue(_max_of_range, bigLandMarkStep);
            //qDebug() << __FUNCTION__ << "lowerStepValue_inDividerStepValue = " << lowerStepValue_inStepValue;
            //qDebug() << __FUNCTION__ << "upperStepValue_inDividerStepValue = " << upperStepValue_inStepValue;

            for (double bigLandMark  = lowerStepValue_inStepValue; //lowerStepValue_in010plus1;
                        bigLandMark <= upperStepValue_inStepValue; //upperStepValue_inStepValue;
                        bigLandMark+=bigLandMarkStep) {
                //qDebug() << __FUNCTION__ << "loop with bigLandMark = " << bigLandMark;
                if (bigLandMark >= _min_of_range) {
                    if (bigLandMark <= _max_of_range) {
                        if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(bigLandMark)) {
                            _qvect_bigLandMarks << .0;
                            //qDebug() << "store bigLandMark as replaced by zero";
                        } else {
                            _qvect_bigLandMarks << bigLandMark;
                            //qDebug() << "store bigLandMark = " << bigLandMark;
                        }
                    } else {
                         //qDebug() << "bigLandMark > _max_of_range => rejected";
                    }
                } else {
                    //qDebug() << "bigLandMark < _min_of_range => rejected";
                }
            }

            //qDebug() << __FUNCTION__ << "_qvect_bigLandMarks = " << _qvect_bigLandMarks;

            if (_qvect_bigLandMarks.count() >/*=*/ 3) {
                //>3 considered as enough big landmarks for interesting big landmarks display
                bBigLandMarksSpreadFound = true;
                break;
            } else {
               //qDebug() << __FUNCTION__ << "considered as not enough";
            }
        }
        if (bBigLandMarksSpreadFound) {
            break;
        }
    }

    if (!bBigLandMarksSpreadFound) {
        //qDebug() << __FUNCTION__ << "warning: range too big to compute bigLandMarks";
        return(true);
    }

    //try to add intermediates if step/2 is integer
    if (_qvect_bigLandMarks.size()==3) {
        double oneStep = _qvect_bigLandMarks[1]-_qvect_bigLandMarks[0];
        if (qAbs(oneStep) > 1.0) { //if step not too small

            //qDebug() << __FUNCTION__ << "oneStep = " << oneStep;

            double firstIntermediate = (_qvect_bigLandMarks[1]+_qvect_bigLandMarks[0])/2.0;

            //qDebug() << __FUNCTION__ << "firstIntermediate = " << firstIntermediate;

            int texel = 0;
            floorfrac(static_cast<float>(firstIntermediate), &texel);
            if (!texel) {
                //qDebug() << __FUNCTION__ << "firstIntermediate is zero integer; to small";
            } else {
                if (!valueIsCloseToZero_deltaLowerThan1ExpMinus14(static_cast<double>(texel) - firstIntermediate)) {
                    //qDebug() << __FUNCTION__ << "firstIntermediate not integer";
                } else {
                    //qDebug() << __FUNCTION__ << "*** add 2 firstIntermediates ";
                    _qvect_bigLandMarks << firstIntermediate;
                    double secondIntermediate = (_qvect_bigLandMarks[1]+_qvect_bigLandMarks[2])/2.0;
                    _qvect_bigLandMarks << secondIntermediate;

                    //qDebug() << __FUNCTION__ << "secondIntermediate = " << secondIntermediate;

                }
            }
            std::sort(_qvect_bigLandMarks.begin(), _qvect_bigLandMarks.end());
        }
    }

    qDebug() << __FUNCTION__ << "*** final: _qvect_bigLandMarks = " << _qvect_bigLandMarks;

    double smallLandMarkStep = bigLandMarkStep / 5.0; //10.0; //5.0; //TBC method with 10 instead of trying to use firstIntermediate or secondIntermediate
    //qDebug() << __FUNCTION__ << " smallLandMarkStep = " << smallLandMarkStep;

    int texel = 0;
    floorfrac(static_cast<float>(smallLandMarkStep), &texel);
    if (!texel) {
        //qDebug() << __FUNCTION__ << "smallLandMarkStep is zero integer, to small as candidate";
    } else {
        if (!valueIsCloseToZero_deltaLowerThan1ExpMinus14(static_cast<double>(texel) - smallLandMarkStep)) {
            //qDebug() << __FUNCTION__ << "smallLandMarkStep not integer, not division by 5";
        } else {

            for(auto iterBigStep = _qvect_bigLandMarks.begin(); iterBigStep < _qvect_bigLandMarks.end()-1; ++iterBigStep) {
                double vLow = *iterBigStep;
                double vHigh = *(iterBigStep+1);
                //qDebug() << "vLow  = " << vLow;
                //qDebug() << "vHigh = " << vHigh;

                for (auto smallStep = vLow+smallLandMarkStep;
                    smallStep < vHigh;
                    smallStep+= smallLandMarkStep) {
                    //qDebug() << "add smallStep: " << smallStep;
                    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(smallStep-vHigh)) {
                        //do not push the value if too close to vHigh
                    } else {
                        //fprintf(stdout, "[[%.14f]]\n", smallStep);
                        _qvect_smallLandMarks << smallStep;
                    }
                }
            }

            //qDebug() << __FUNCTION__ << "*** without remaining: _qvect_smallLandMarks = " << _qvect_smallLandMarks;

            if (_qvect_bigLandMarks.count()) {
                //qDebug() << "adding remaining from upper to max:";
                //add the smallLandmarks from upper to max if needs
                for(double vLow = _qvect_bigLandMarks.first() - smallLandMarkStep;
                    vLow > _min_of_range; vLow-=smallLandMarkStep) {
                    _qvect_smallLandMarks << vLow;
                    //qDebug() << "added as remaining: " << vLow;
                }

                //qDebug() << "adding remaining from lower to min:";
                //add the smallLandmarks from lower to min if needs
                for(double vLow = _qvect_bigLandMarks.last() + smallLandMarkStep;
                    vLow < _max_of_range; vLow+=smallLandMarkStep) {
                   _qvect_smallLandMarks << vLow;
                   //qDebug() << "added as remaining: " << vLow;
                }
            }
        }
    }

    qDebug() << __FUNCTION__ << "*** final: _qvect_bigLandMarks = " << _qvect_bigLandMarks;
    qDebug() << __FUNCTION__ << "*** final: _qvect_smallLandMarks = " << _qvect_smallLandMarks;

    return(true);
}

const QVector<double>& LandmarkInRange::getBigLandMarksRef() {
    return(_qvect_bigLandMarks);
}

const QVector<double>& LandmarkInRange::getSmallLandMarksRef() {
    return(_qvect_smallLandMarks);
}

void countCharBeforeAndAfterDot(const QString& qstrv, int& countCharBeforeDot, int& countCharAfterDot) {

    int idxOfDot = qstrv.indexOf('.');
    //qDebug() << "idxOfDot = " << idxOfDot;

    if (idxOfDot == -1) { //no decimal

        QString qstrPartBeforeDot = qstrv.left(idxOfDot);
        countCharBeforeDot = qstrPartBeforeDot.size();
        countCharAfterDot = 0;

        qDebug() << "qstrPartBeforeDot = " << qstrPartBeforeDot;
        qDebug() << "qstrPartAfterDot = " << "not applicable";

    } else {
        QString qstrPartBeforeDot = qstrv.left(idxOfDot);
        countCharBeforeDot = qstrPartBeforeDot.size();

        QString qstrPartAfterDot  = qstrv.mid(idxOfDot+1);
        countCharAfterDot = qstrPartAfterDot.size();

        qDebug() << "qstrPartBeforeDot = " << qstrPartBeforeDot;
        qDebug() << "qstrPartAfterDot = " << qstrPartAfterDot;
    }

}

QVector<QString> valuesToPowerString_withUniformPower(const QVector<double> &qvectValuesSortedMinToMax) {

    QVector<QString> qvectqstrvaluesToPowerString;

    if (qvectValuesSortedMinToMax.isEmpty()) {
        return(qvectqstrvaluesToPowerString);
    }

    //uniform power values synced with the min value

    double minValue = qvectValuesSortedMinToMax.first();
    double powerOfTen_ofMinValue = powerOfTen(minValue);

    qDebug() << "minValue =" << minValue;
    qDebug() << "powerOfTen_ofMinValue =" << powerOfTen_ofMinValue;

    //find the common zero in the vector values, to find possible common power for all, as a beautifier method
    bool bZeroAtEndPermitToPowerThem_aboutPositivePowerOfTen = false;
    int nbZerosCommonToAllValues_aboutPositivePowerOfTen = 999;
    if (powerOfTen_ofMinValue >= 0) {
        for (auto v: qvectValuesSortedMinToMax) {
            if (!valueIsCloseToZero_deltaLowerThan1ExpMinus14(v)) {
                QString qstrv = QString::number(v, 'g', 14); //no exponent, we have to find the exponent
                int nbZerosAtEnd = 0;
                for (QString::const_reverse_iterator cstRevIter = qstrv.rbegin();
                    cstRevIter != qstrv.rend(); ++cstRevIter) {
                    if (*cstRevIter == '0') {
                        nbZerosAtEnd++;
                    }
                }
                //qDebug() << "qstrv =" << qstrv << "nbZerosAtEnd = " << nbZerosAtEnd;
                if (nbZerosAtEnd < nbZerosCommonToAllValues_aboutPositivePowerOfTen) {
                    nbZerosCommonToAllValues_aboutPositivePowerOfTen = nbZerosAtEnd;
                }
            }
        }
        if (nbZerosCommonToAllValues_aboutPositivePowerOfTen > 1) { //avoid to use power just for one 0
            bZeroAtEndPermitToPowerThem_aboutPositivePowerOfTen = true;
        }
    }

    qDebug() << "bZeroAtEndPermitToPowerThem_aboutPositivePowerOfTen =" << bZeroAtEndPermitToPowerThem_aboutPositivePowerOfTen;
    qDebug() << "nbZerosCommonToAllValues_aboutPositivePowerOfTen ="    << nbZerosCommonToAllValues_aboutPositivePowerOfTen;

    bool bZeroDecimalAtStartPermitToPowerThem_aboutNagativePowerOfTen = false;
    int nbZerosDecimalCommonToAllValues_aboutNegativePowerOfTen = 999;
    if (powerOfTen_ofMinValue < 0) {

        for (auto v:qvectValuesSortedMinToMax) {
            if (!valueIsCloseToZero_deltaLowerThan1ExpMinus14(v)) {
                QString qstrv = QString::number(v, 'g', 14); //no exponent, we have to find the exponent

                int idxOfDot = qstrv.indexOf('.');
                //qDebug() << "idxOfDot = " << idxOfDot;

                int nbZerosDecimalAtStart = 0;
                int qstrvSize = qstrv.size();
                for (int idx = idxOfDot+1; idx < qstrvSize; ++idx) {
                    if (qstrv[idx] == '0') {
                        nbZerosDecimalAtStart++;
                    }
                }
                //qDebug() << "qstrv =" << qstrv << "nbZerosDecimalAtStart = " << nbZerosDecimalAtStart;
                if (nbZerosDecimalAtStart < nbZerosDecimalCommonToAllValues_aboutNegativePowerOfTen) {
                    nbZerosDecimalCommonToAllValues_aboutNegativePowerOfTen = nbZerosDecimalAtStart;
                }
            }
        }
        if (nbZerosDecimalCommonToAllValues_aboutNegativePowerOfTen > 1) { //avoid to use power just for one 0
            bZeroDecimalAtStartPermitToPowerThem_aboutNagativePowerOfTen = true;
        }
    }
    qDebug() << "bZeroDecimalAtStartPermitToPowerThem_aboutNagativePowerOfTen =" << bZeroDecimalAtStartPermitToPowerThem_aboutNagativePowerOfTen;
    qDebug() << "nbZerosDecimalCommonToAllValues_aboutNegativePowerOfTen ="      << nbZerosDecimalCommonToAllValues_aboutNegativePowerOfTen;

    //second way for beautification: avoid too long string numeric value, before and after the dot '.'
    //and using this and the beautifier way above, find the newPowerOfV to apply to all values, to have the same power on them

    double newPowerOfV = .0;

    QString qstrvgofMinValue = QString::number(minValue, 'g', 14); //no exponent, we have to find the exponent
    qDebug() << __FUNCTION__ << "qstrvgofMinValue =" << qstrvgofMinValue;

    int countCharBeforeDot = -1;
    int countCharAfterDot = -1;

    countCharBeforeAndAfterDot(qstrvgofMinValue, countCharBeforeDot, countCharAfterDot);

    qDebug() << "countCharBeforeDot = " << countCharBeforeDot;
    qDebug() << "countCharAfterDot = " << countCharAfterDot;

    if (powerOfTen_ofMinValue >= 0) {

        int withPositivePowerOfTen_maxPermittedCharBeforeDot_lettingOriginaleExponentIfPresent = 4;
        int withPositivePowerOfTen_maxPermittedCharAfterDot_lettingOriginaleExponentIfPresent = 3;

        int withPositivePowerOfTen_powerOfTenForDisplayWithoutExponentDisplay =
            withPositivePowerOfTen_maxPermittedCharBeforeDot_lettingOriginaleExponentIfPresent -1;

        bool bNeedsDecimalCut = false;
        bool bNeedsReExponentiation = false;

        if (countCharBeforeDot <=  withPositivePowerOfTen_maxPermittedCharBeforeDot_lettingOriginaleExponentIfPresent) {  //examples: 1234.xxxxxxx  //1.234xxxxxxx

            qDebug() << "withPositivePowerOfTen_maxPermittedCharBeforeDot_lettingOriginaleExponentIfPresent = " << withPositivePowerOfTen_maxPermittedCharBeforeDot_lettingOriginaleExponentIfPresent;
            qDebug() << "withPositivePowerOfTen_maxPermittedCharAfterDot_lettingOriginaleExponentIfPresent  = " << withPositivePowerOfTen_maxPermittedCharAfterDot_lettingOriginaleExponentIfPresent;

            if (countCharAfterDot <=  withPositivePowerOfTen_maxPermittedCharAfterDot_lettingOriginaleExponentIfPresent) {//examples: 1234.123      //1.234
                //no adjustements
                qvectqstrvaluesToPowerString << qstrvgofMinValue;

            } else { //examples: 1234.xxxxxxx //1.234xxxxxxx
                bNeedsDecimalCut = true;
                qDebug() << "bNeedsDecimalCut set to " << bNeedsDecimalCut;
            }

        } else { //examples: 12345.xxxxxxx

            bNeedsReExponentiation = true;

        }

        if (bNeedsDecimalCut) {   // example: 12345.123        => 12345.12
                                  // example: 12345.xxxxxxxx   => 12345.xx
                                  //           1234.xxxxxxxx   =>  1234.xxx
                                  //              1.23456789   => 1.234567

            qDebug() << __FUNCTION__ << "warning: bNeedsDecimalCut is true; it should never occurs in our context";

            qvectqstrvaluesToPowerString << qstrvgofMinValue; //@#LP dbg log

        }

        if (bNeedsReExponentiation) {

            if (bZeroAtEndPermitToPowerThem_aboutPositivePowerOfTen) {
                newPowerOfV = nbZerosCommonToAllValues_aboutPositivePowerOfTen;
                //qDebug() << __FUNCTION__ << " ===>  withPositivePowerOfTen_powerOfTenForDisplayWithoutExponentDisplay = " << withPositivePowerOfTen_powerOfTenForDisplayWithoutExponentDisplay;
            } else {
                //let withPositivePowerOfTen_powerOfTenForDisplayWithoutExponentDisplay in peace
                //qDebug() << __FUNCTION__ << " let withPositivePowerOfTen_powerOfTenForDisplayWithoutExponentDisplay = " << withPositivePowerOfTen_powerOfTenForDisplayWithoutExponentDisplay;
                newPowerOfV = powerOfTen_ofMinValue - withPositivePowerOfTen_powerOfTenForDisplayWithoutExponentDisplay;
                if (newPowerOfV < 2.0) {
                    //qDebug() << __FUNCTION__ << " newPowerOfV < 2.0 (" << newPowerOfV << " ) => force to 2.0";
                    newPowerOfV = 2.0;
                }
            }

            if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(minValue)) {
                qvectqstrvaluesToPowerString << "0";

            } else {
                if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(newPowerOfV)) {
                    qvectqstrvaluesToPowerString << qstrvgofMinValue;
                } else {

                    double minValueNewPow = minValue/qPow(10, newPowerOfV);
                    //qDebug() << __FUNCTION__ << " minValueNewPow = " << minValueNewPow;
                    //qDebug() << __FUNCTION__ << " newPowerOfV = " << newPowerOfV;

                    QString qstrvgof_minValueNewPow = QString::number(minValueNewPow, 'g', 14);
                    //qDebug() << "qstrvgof_minValueNewPow =" << qstrvgof_minValueNewPow << "with newPowerOfV = " << newPowerOfV;


                    qvectqstrvaluesToPowerString << qstrvgof_minValueNewPow+QString("x10<sup>%2</sup>").arg(newPowerOfV);
                }
            }
        }

    } else { //powerOften < 0

        int withNegativePowerOfTen_maxPermittedCharAfterDot_lettingOriginaleAmountOfDecimale = 5;

        int withNegativePowerOfTen_maxPermittedCharAfterDot_ToLetLikeThat = 3;
        bool bLetLikeThat = false;


        //bool bNeedsDecimalCut = false;
        if (countCharAfterDot <= withNegativePowerOfTen_maxPermittedCharAfterDot_lettingOriginaleAmountOfDecimale) { //examples: 0.12345

            if (countCharAfterDot <= withNegativePowerOfTen_maxPermittedCharAfterDot_ToLetLikeThat) {
                bLetLikeThat = true;
            } /*else {
                // let a chance to bZeroDecimalAtStartPermitToPowerThem_aboutNagativePowerOfTen below
            }*/

        } else { //examples: 0.1234567
            //qDebug() << "warning: powerOften < 0 and case not handle (countCharAfterDot > ..._maxPermittedCharAfterDot_...)";

        }

        if (!bLetLikeThat) {
           if (bZeroDecimalAtStartPermitToPowerThem_aboutNagativePowerOfTen) {
                newPowerOfV = -nbZerosDecimalCommonToAllValues_aboutNegativePowerOfTen;
                //qDebug() << __FUNCTION__ << " ===>  bZeroDecimalAtStartPermitToPowerThem_aboutNegativePowerOfTen = " << nbZerosDecimalCommonToAllValues_aboutNegativePowerOfTen;
            } else {
                newPowerOfV = powerOfTen_ofMinValue - (-withNegativePowerOfTen_maxPermittedCharAfterDot_lettingOriginaleAmountOfDecimale);
                if (newPowerOfV > -2.0) {
                    //qDebug() << __FUNCTION__ << " newPowerOfV > -2.0 (" << newPowerOfV << " ) => force to -2.0";
                    newPowerOfV = -2.0;
                }
            }
        }

        if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(minValue)) {
            qvectqstrvaluesToPowerString << "0";

        } else {

            if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(newPowerOfV)) {
                qvectqstrvaluesToPowerString << qstrvgofMinValue;

            } else {

                double minValueNewPow = minValue/qPow(10, newPowerOfV);
                //qDebug() << __FUNCTION__ << " minValueNewPow = " << minValueNewPow;
                //qDebug() << __FUNCTION__ << " newPowerOfV = " << newPowerOfV;

                QString qstrvgof_minValueNewPow = QString::number(minValueNewPow, 'g', 14);
                //qDebug() << "qstrvgof_minValueNewPow =" << qstrvgof_minValueNewPow << "with newPowerOfV = " << newPowerOfV;

                qvectqstrvaluesToPowerString << qstrvgof_minValueNewPow+QString("x10<sup>%2</sup>").arg(newPowerOfV);
            }
        }
    }

    //now apply newPowerOfV on the remaing values (all except already pushed minValueNewPow
    for (auto vIter = qvectValuesSortedMinToMax.cbegin()+1;
              vIter != qvectValuesSortedMinToMax.cend();
              ++vIter) {

        if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(*vIter)) {
            qvectqstrvaluesToPowerString << "0";

        } else {

            if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(newPowerOfV)) {
                QString qstrvgof_valueNewPow = QString::number(*vIter, 'g', 14);
                qvectqstrvaluesToPowerString << qstrvgof_valueNewPow;//+"x10<sup>-9</sup>";

            } else {
                double valueNewPow = (*vIter)/qPow(10, newPowerOfV);
                //qDebug() << __FUNCTION__ << " valueNewPow = " << valueNewPow;

                QString qstrvgof_valueNewPow = QString::number(valueNewPow, 'g', 14); //no exponent, we have to find the exponent
                //qDebug() << "qstrvgof_valueNewPow =" << qstrvgof_valueNewPow << "with newPowerOfV = " << newPowerOfV;

                qvectqstrvaluesToPowerString << qstrvgof_valueNewPow+QString("x10<sup>%2</sup>").arg(newPowerOfV);
            }
        }
    }

    qDebug() << "qvectqstrvaluesToPowerString: " << qvectqstrvaluesToPowerString;

    return(qvectqstrvaluesToPowerString);
}


// basePowerForDisplay indicates how the value would be displayed if no cosmetic aspect used
// This basePowerForDisplay value is the same as the power of range, as this is the base to sync display of the different value in the range
// Example: value v = 123456.789 ; its basePowerForDisplay = 4 ; hence: v would be displayed as: 1234.56789x10(exponent)2
// valueToPoweredString uses the basePowerForDisplay to know how to beautify the value for final display
// and produces this string: 1234.56x10(exponent)2
// Another example: value v: 1234 ; its basePowerForDisplay = 3, hence: v would be displayed as: 1234x10(exponent)0
// valueToPoweredString use the basePowerForDisplay to know how to beautify the value for final display
// and produces this string: 1234

QString valueToPoweredString(double v, double smallest_basePowerForDisplay, double biggest_basePowerForDisplay) {

    qDebug() << __FUNCTION__ << "v = " << v;
    qDebug() << __FUNCTION__ << "smallest_basePowerForDisplay = " << smallest_basePowerForDisplay;
    qDebug() << __FUNCTION__ << "biggest_basePowerForDisplay  = " << biggest_basePowerForDisplay;

    //test_qstringformat("input:", v);

    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(v)) {
        qDebug() << __FUNCTION__ << " value is close to zero => return(\"0\")";
        return("0");
    }

    QString qstrFinal;

    //add negative sign
    if (v < .0) {
        qstrFinal+="-";
    }

    //remove sign from input value for next step
    v=qAbs(v);


    double powerOfV = powerOfTen(v);
    qDebug() << __FUNCTION__ << " powerOfV = " << powerOfV;

    return(qstrFinal);
}


void test_qstringformat(const QString& strDebug, double v) {

    //double a1 = 82348.12323;
    //double a2 = 234.2;
    //double a3 = 0.00001;

    QString vg = QString::number(v, 'g', 14);
    //QString s2g = QString::number(a2, 'g', 10);
    //QString s3g = QString::number(a3, 'g', 10);

    qDebug() << "vg:" << vg;
    //qDebug() << "s2g:" << s2g;
    //qDebug() << "s3g:" << s3g;

    QString vf = QString::number(v, 'f', 14);
    //QString s2f = QString::number(a2, 'f', 10);
    //QString s3f = QString::number(a3, 'f', 10);

    //qDebug() << strDebug << ": vg: " << vg << "; vf: " << vf << " ; powerOfTen: " << powerOfTen(v);
    //qDebug() << "s2f:" << s2f;
    qDebug() << "vf:" << vf;

    //s1: "82348.12323"
    //s2: "234.2"

    /*
    setting a1 = 0.00001 will output as "1e-05" using this code.
    consider using QString::number(a1, 'f', 10) instead – Stonewall_Jefferson Feb 17 '16 at 21:02
    */
}
