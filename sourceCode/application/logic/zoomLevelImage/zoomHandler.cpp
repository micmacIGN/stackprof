#include "math.h"

#include <QDebug>
#include <QtGlobal>

#include "zoomHandler.h"

#include <vector>
using namespace std;

//This is used to avoid to wait that the requiered tiles are loaded at the right ZLI (which can takes times sometimes).
//Hence, instead of waiting the load at the right ZLI, the idea is to scale tiles at a value got from container build by this method.
//And when the tiles will be loaded and ready to be displayed, the current ZLI with it's according scale will be applyed.
//At use the full scale for each ZLI should be not used; as the user will probably want to see a global zoom factor not so far
//from the starting global zoom factor. But by design, it's not forbidden.
//@#LP: TBC app behavior with usage of large and small scales view values.
void ZoomHandler::buildViewSideFullScalesForEachZLIWhenNoZLISwitch() {

    _qhash_ZLI_extendedScalesForView.clear();

    qreal maxZoomLevelImage = _vectZoomParameters.last()._zoomLevelImage_forSceneSide;

    for (int ZLI = 1; ZLI <= maxZoomLevelImage; ZLI*=2) {

        QVector<double> qvect_extendedScalesForView(_vectZoomParameters.size(), 1.0);

        for (int vectIdx = 0; vectIdx < _vectZoomParameters.size(); vectIdx++) {
            qvect_extendedScalesForView[vectIdx] = ZLI * (1.0 / _vectZoomParameters[vectIdx]._globalZoomFactor);
        }

        _qhash_ZLI_extendedScalesForView.insert(ZLI, qvect_extendedScalesForView);

        qDebug() << __FUNCTION__ << "ZLI: " << ZLI << " qvect_extendedScalesForView: " << qvect_extendedScalesForView;
        qDebug() << __FUNCTION__ << "--------";
    }

}

qreal ZoomHandler::getViewScaleForExtendedScale(int ZLI) {
    return(_qhash_ZLI_extendedScalesForView.value(ZLI).at(_idxZoom)); //@LP this acces MUST NEVER FAIL.
}

//The zoomIn/zoomOut factor increases adding a percent of the previous zoomIn/zoomOut factor
//the progression value is non linear (but the visual effect is linear)
void ZoomHandler::initZoomRange_nonLinear_increasePreviousValueByXXXPercent(int maximumZoomLevelImageAvailable) {
    double zoomFactorStep = 0.275; //0.5 to help identify bugs
    //0.275; //0.275; //0.15; //0.275; //0.15;  //percent of the previous zoom factor to add

    qDebug() << __FUNCTION__ << "maximumZoomLevelImageAvailable to use: " << maximumZoomLevelImageAvailable;
    //[..., 1[ :  explore zooming/dezooming in level 1
    //]  1, 8] :  explore zooming/dezooming in level 2,4,8
    qreal zoomFactorMin { 1.0/256.0 };  // 1/64.0 };
    qreal zoomFactorMax_withZoomLevelImage { static_cast<qreal>(maximumZoomLevelImageAvailable) /*64.0*/ /*8.0*/};
    qreal zoomFactorMax_overLastZoomLevelImage { zoomFactorMax_withZoomLevelImage*8.0 /*64.0*/ };

    int currentZoomLevelImage = 1;

    qreal nextValueWhichIsPowerOfTwo = -1; //-1 as invalid value
    qreal nextValueWichIsATranstionToUpperZoomLevelImage = -1; //-1 as invalid value

    //first pass with the globalZoomFactor and ZoomLevelImage


    //@LP incorrect location of any item when reached "big" pixel loc:
    //the scale applyed to customGraphicsItem when zoom inside the image is not perfect when the zoom value present some decimal which does not permit to be easily accurate.
    //This is visible when showing a grid of 1x1 pixel on a image with scale like : 42.00112
    //For this reason for zoom inside the original image, we stick on zoom value only on multiple of 2 for now
    QVector<S_zoomParameters> vectZoomParameters_1toMin;
    for (qreal globalZoomFactor = 1.0; globalZoomFactor >= zoomFactorMin; globalZoomFactor/=2.0) {
        S_zoomParameters zoomParameters;
        zoomParameters._globalZoomFactor = globalZoomFactor;
        zoomParameters._zoomLevelImage_forSceneSide = 1;
        zoomParameters._scale_forViewSide = 0; //set as 'not set to useful value' in this loop
        vectZoomParameters_1toMin.push_back(zoomParameters);
    }

    //auto reverseIterator = vectZoomParameters_1toMin.rbegin();
    QVector<S_zoomParameters>::reverse_iterator reverseIterator = vectZoomParameters_1toMin.rbegin();
    for (;
         reverseIterator != vectZoomParameters_1toMin.rend();
         ++reverseIterator) {
        _vectZoomParameters.push_back(*reverseIterator);
    }

    for(auto iter: _vectZoomParameters) {
        qDebug() << "min to 1:  globalZF, zLvlImage :" << iter._globalZoomFactor << ", " << iter._zoomLevelImage_forSceneSide;
    }

    _idxForZoomLevelImage1Scale1 = _vectZoomParameters.size()-1;

    nextValueWhichIsPowerOfTwo = 2.0;
    nextValueWichIsATranstionToUpperZoomLevelImage = (1.0 + nextValueWhichIsPowerOfTwo)/2.0;

    bool bGlobalZoomFactorReachedPowerOfTwoOrOver = false;
    int vectIdx =_idxForZoomLevelImage1Scale1+1;
    //range 1 to max
    for (qreal globalZoomFactor = 1.0+1.0*zoomFactorStep;
         globalZoomFactor <= zoomFactorMax_overLastZoomLevelImage;
         globalZoomFactor+=globalZoomFactor*zoomFactorStep) {

        qDebug() << "globalZoomFactor loop: globalZoomFactor =" << globalZoomFactor;
        qDebug() << "globalZoomFactor loop:  nextValueWichIsATranstionToUpperZoomLevelImage =" << nextValueWichIsATranstionToUpperZoomLevelImage;

        S_zoomParameters zoomParameters;

        bool bIsPowerOfTwo = false;
        float adjustedValuePowerOfTwo = 0.0;

        //qDebug() << "> globalZoomFactor before adjustement =" << globalZoomFactor;
        qDebug() << "   adjust if around delta = " << 0.0075*globalZoomFactor;

        bIsPowerOfTwo = valueIsCloseToPowerOfTwo(globalZoomFactor, 0.0075*globalZoomFactor, adjustedValuePowerOfTwo);
        if (bIsPowerOfTwo) {
           globalZoomFactor = adjustedValuePowerOfTwo;
           qDebug() << "   \-> adjusted to value power of two:" << globalZoomFactor;

           if (qAbs(globalZoomFactor - nextValueWhichIsPowerOfTwo) < 0.00000001) {
               bGlobalZoomFactorReachedPowerOfTwoOrOver = true;
           }
        }
        zoomParameters._globalZoomFactor = globalZoomFactor;

        bool bSwitchToNextnextValueWhichIsPowerOfTwo_done = false;
        if (globalZoomFactor >= nextValueWichIsATranstionToUpperZoomLevelImage) {

            qDebug() << "if (globalZoomFactor >= nextValueWichIsATranstionToUpperZoomLevelImage) #1 ";

            if (globalZoomFactor <= zoomFactorMax_withZoomLevelImage) {

                qDebug() << "if (globalZoomFactor <= zoomFactorMax_withZoomLevelImage) {  #2 ";

                currentZoomLevelImage *=2;

                nextValueWichIsATranstionToUpperZoomLevelImage = (nextValueWhichIsPowerOfTwo + nextValueWhichIsPowerOfTwo*2.0)/2.0;
                nextValueWhichIsPowerOfTwo = nextValueWhichIsPowerOfTwo*2.0;

                bSwitchToNextnextValueWhichIsPowerOfTwo_done = true;

                qDebug() << " #2  => nextValueWichIsATranstionToUpperZoomLevelImage => " << nextValueWichIsATranstionToUpperZoomLevelImage;
                qDebug() << " #2 => bSwitchToNextnextValueWhichIsPowerOfTwo_done => " << bSwitchToNextnextValueWhichIsPowerOfTwo_done;
                qDebug() << " #2 => nextValueWhichIsPowerOfTwo => " << nextValueWhichIsPowerOfTwo;

                //the next reachable and valid value will be set below on 'if (bIsPowerOfTwo) {'
            }
        }

        zoomParameters._zoomLevelImage_forSceneSide = currentZoomLevelImage;

        if (globalZoomFactor <= zoomFactorMax_withZoomLevelImage) {

            qDebug() << "if (globalZoomFactor <= zoomFactorMax_withZoomLevelImage) #3";

            if (!bSwitchToNextnextValueWhichIsPowerOfTwo_done) {

                qDebug() << "if (!bSwitchToNextnextValueWhichIsPowerOfTwo_done) { #4";

                if (!bGlobalZoomFactorReachedPowerOfTwoOrOver) {

                    qDebug() << "if (!bGlobalZoomFactorReachedPowerOfTwoOrOver) { #5";

                    if (globalZoomFactor > nextValueWhichIsPowerOfTwo) {

                        qDebug() << "if (globalZoomFactor > nextValueWhichIsPowerOfTwo) { #6";

                        bGlobalZoomFactorReachedPowerOfTwoOrOver = true;

                        qDebug() << "#6 bGlobalZoomFactorReachedPowerOfTwoOrOver =>" << bGlobalZoomFactorReachedPowerOfTwoOrOver;

                    }
                }
                if (bGlobalZoomFactorReachedPowerOfTwoOrOver) {

                    qDebug() << "if (bGlobalZoomFactorReachedPowerOfTwoOrOver) { #7";

                    nextValueWichIsATranstionToUpperZoomLevelImage = (nextValueWhichIsPowerOfTwo + nextValueWhichIsPowerOfTwo*2.0)/2.0;
                    nextValueWhichIsPowerOfTwo = nextValueWhichIsPowerOfTwo*2.0;

                    bGlobalZoomFactorReachedPowerOfTwoOrOver = false;

                    qDebug() << " #7 => nextValueWichIsATranstionToUpperZoomLevelImage => " << nextValueWichIsATranstionToUpperZoomLevelImage;
                    qDebug() << " #7 => nextValueWhichIsPowerOfTwo => " << nextValueWhichIsPowerOfTwo;
                    qDebug() << " #7 => bGlobalZoomFactorReachedPowerOfTwoOrOver => " << bGlobalZoomFactorReachedPowerOfTwoOrOver;

                }
            }
        }

        zoomParameters._scale_forViewSide = 0; //set as 'not set to useful value' in this loop

        _vectZoomParameters.push_back(zoomParameters);

        vectIdx++;
    }

    qDebug() << "_vectZoomParameters: #196";
    int dbg_idx = 0;
    for(auto iter: _vectZoomParameters) {
        qDebug() << dbg_idx << "] globalZF, zLvlImage :" << iter._globalZoomFactor << ", " << iter._zoomLevelImage_forSceneSide;
        dbg_idx++;
    }

    /*qDebug() << "qvect_idxOfLvlTransition:";
    dbg_idx = 0;
    for(auto iter: qvect_idxOfLvlTransition) {
        qDebug() << dbg_idx << "] idxOfLvlTransitions :" << iter;
        dbg_idx++;
    }*/

    /*qDebug() << "qvect_idxOfPowerOfTwo:";
    dbg_idx = 0;
    for(auto iter: qvect_idxOfPowerOfTwo) {
        qDebug() << dbg_idx << "] qvect_idxOfPowerOfTwo :" << iter;
        dbg_idx++;
    }*/

    for (auto& iter: _vectZoomParameters) {
        if (iter._globalZoomFactor <= 1.0) {
            iter._scale_forViewSide = 1.0/iter._globalZoomFactor;
        }
    }

    qDebug() << "_vectZoomParameters:";
    dbg_idx = 0;
    for(auto iter: _vectZoomParameters) {
        qDebug() << dbg_idx << ": globalZF, zLvlImage :" << iter._globalZoomFactor << ", " << iter._zoomLevelImage_forSceneSide << ", " << iter._scale_forViewSide;
        dbg_idx++;
    }

    //qDebug() << "qvect_idxOfPowerOfTwo.size() =" << qvect_idxOfPowerOfTwo.size();
    //qDebug() << "qvect_idxOfLvlTransition.size() =" << qvect_idxOfLvlTransition.size();


    int currentZLI = 1;
    for (vectIdx = _idxForZoomLevelImage1Scale1; vectIdx < _vectZoomParameters.size(); vectIdx++) {
        if (_vectZoomParameters[vectIdx]._globalZoomFactor >= zoomFactorMax_withZoomLevelImage) {
            break;            
        }

        if (_vectZoomParameters[vectIdx]._zoomLevelImage_forSceneSide != currentZLI) {  //transition to upper
            currentZLI = _vectZoomParameters[vectIdx]._zoomLevelImage_forSceneSide;
        }

        _vectZoomParameters[vectIdx]._scale_forViewSide = currentZLI * (1.0 / _vectZoomParameters[vectIdx]._globalZoomFactor);

    }

    dbg_idx = 0;
    for(auto iter: _vectZoomParameters) {
        qDebug() << dbg_idx << ": globalZF, zLvlImage :" << iter._globalZoomFactor << ", " << iter._zoomLevelImage_forSceneSide << ", " << iter._scale_forViewSide;
        dbg_idx++;
    }

    //"remaining zoom over the last zoomlevelImage:"
    for (; vectIdx < _vectZoomParameters.size(); vectIdx++) {
        _vectZoomParameters[vectIdx]._scale_forViewSide = currentZLI * (1.0 / _vectZoomParameters[vectIdx]._globalZoomFactor);
    }

    dbg_idx = 0;
    qDebug() << "final _vectZoomParameters:";
    dbg_idx = 0;
    for(auto iter: _vectZoomParameters) {
        qDebug() << dbg_idx << ": globalZF, zLvlImage :" << iter._globalZoomFactor << ", " << iter._zoomLevelImage_forSceneSide << ", " << iter._scale_forViewSide;
        dbg_idx++;
    }

}



ZoomHandler::ZoomHandler():
    _idxZoom(-1),   //-1 as invalid value
    _idxForZoomLevelImage1Scale1(0), //0 as invalid value
    _zoomLevelImageTransition(e_zLIT_none) {
}

void ZoomHandler::clear() {
    _vectZoomParameters.clear();
    _idxZoom = -1;   //-1 as invalid value
    _idxForZoomLevelImage1Scale1 = 0; //0 as invalid value
    _zoomLevelImageTransition = e_zLIT_none;
    _qhash_ZLI_extendedScalesForView.clear();
}

void ZoomHandler::setToZoomLevelImage1_scale1() {
    _idxZoom = _idxForZoomLevelImage1Scale1;
    _zoomLevelImageTransition = e_zLIT_none;
}

double ZoomHandler::getCurrent_globalZoomFactor() {
    return(_vectZoomParameters[_idxZoom]._globalZoomFactor);
}

qreal ZoomHandler::getCurrent_scale_forViewSide() {
return(_vectZoomParameters[_idxZoom]._scale_forViewSide);
}

int ZoomHandler::getCurrent_zoomLevelImage_forSceneSide() {
    return(_vectZoomParameters[_idxZoom]._zoomLevelImage_forSceneSide);
}

ZoomHandler::e_zoomLevelImageTransition ZoomHandler::getCurrent_zoomLevelImageTransition() {
    return(_zoomLevelImageTransition);
}

int ZoomHandler::findClosestZoomIndexOfZoomValueForGlobalZoomFactor(double globalZoomFactor) {

    if (_vectZoomParameters.size() == 1) {
        qDebug() << __FUNCTION__ << "";
        return(0);
    }

    //dichotomic search
    int indexLow = 0;
    int indexHigh = _vectZoomParameters.size()-1;

    while(1) {
        qDebug() << __FUNCTION__ << "indexLow = " << indexLow;
        qDebug() << __FUNCTION__ << "indexHigh =" << indexHigh;

        if (indexLow+1 == indexHigh) {
            qDebug() << __FUNCTION__ << "";
            return(indexHigh);
        }

        int middleindex = (indexHigh + indexLow)/2;
        qDebug() << __FUNCTION__ << "middleindex =" << middleindex;

        if (_vectZoomParameters[middleindex]._globalZoomFactor > globalZoomFactor) {
            qDebug() << __FUNCTION__ << _vectZoomParameters[middleindex]._globalZoomFactor << " > " << globalZoomFactor;
            indexHigh = middleindex;
        } else {
            if (_vectZoomParameters[middleindex]._globalZoomFactor < globalZoomFactor) {
                qDebug() << __FUNCTION__ << _vectZoomParameters[middleindex]._globalZoomFactor << " < " << globalZoomFactor;
                indexLow = middleindex;
            } else {
                qDebug() << __FUNCTION__ << "middle value match";
                return(middleindex);
            }
        }
    }
    return(0); //default value, code line never reached
}

//return true is the zoom was applyed (max limit not already reached)
bool ZoomHandler::zoomOut(bool& bMaxOutLevelReached) {
    int zLI_before = _vectZoomParameters[_idxZoom]._zoomLevelImage_forSceneSide;
    qDebug() << __FUNCTION__ << "_idxZoom before =" << _idxZoom;
    _idxZoom++;

    if (limitToMaxIfTooHigh(bMaxOutLevelReached)) {
        update_zoomLevelImageTransition_withIdxZoom(zLI_before);
        return(false);
    }

    bool bZLIchanged_first = update_zoomLevelImageTransition_withIdxZoom(zLI_before);
    return(true);
}

//return true is the zoom was applyed (min limit not already reached)
bool ZoomHandler::zoomIn(bool& bMinInLevelReached) {
    int zLI_before = _vectZoomParameters[_idxZoom]._zoomLevelImage_forSceneSide;
    qDebug() << __FUNCTION__ << "_idxZoom before =" << _idxZoom;
    _idxZoom--;

    if (limitToMinIfTooLow(bMinInLevelReached)) {
        update_zoomLevelImageTransition_withIdxZoom(zLI_before);
        return(false);
    }

    bool bZLIchanged_first = update_zoomLevelImageTransition_withIdxZoom(zLI_before);
    return(true);
}

//valid with zoomIn() and zoomOut() which increment or decrecement by 1 step
bool ZoomHandler::limitToMinIfTooLow(bool& bMinInLevelReached) {
    bMinInLevelReached = (_idxZoom == 0);
    if (_idxZoom < 0) {
         qDebug() << __FUNCTION__ << "min reached";
        _idxZoom = 0;
        return(true);
    }
    return(false);
}

bool ZoomHandler::limitToMaxIfTooHigh(bool& bMaxOutLevelReached) {
    bMaxOutLevelReached = (_idxZoom == _vectZoomParameters.size() - 1); //@#LP will fail if size is zero, considered as a very minor use case
    if (_idxZoom >= _vectZoomParameters.size()) {
        qDebug() << __FUNCTION__ << "max reached";
        _idxZoom = _vectZoomParameters.size()-1;
        return(true);
    }
    return(false);
}

//return true if there is a zoomLevelImage transition when going from the current zoom factor to the new zoom factor
bool ZoomHandler::update_zoomLevelImageTransition_withIdxZoom(int ZLI_beforeIdxZoomUpdate) {
    int newZLI = _vectZoomParameters[_idxZoom]._zoomLevelImage_forSceneSide;
    if (ZLI_beforeIdxZoomUpdate == newZLI) {
        qDebug() << __FUNCTION__ << "ZoomHandler::none";
        _zoomLevelImageTransition = ZoomHandler::e_zLIT_none;
        return(false);
    } else {
        if (newZLI > ZLI_beforeIdxZoomUpdate) {
            qDebug() << __FUNCTION__ << "ZoomHandler::zoomOut";
            _zoomLevelImageTransition = ZoomHandler::e_zLIT_zoomOut;
        } else { // (newZLI < before) {
            qDebug() << __FUNCTION__ << "ZoomHandler::zoomIn";
            _zoomLevelImageTransition = ZoomHandler::e_zLIT_zoomIn;
        }
    }
    return(true);
}

//@#LP move to toolbox math
bool ZoomHandler::valueIsPowerOfTwo(double v) {
    double dRoundV = round(v);
    bool bIsWholeNumber = (fabs(v - dRoundV) < 0.000001);
    if (bIsWholeNumber) {
        int iRound =  dRoundV;
        return ((iRound != 0) && ((iRound &(iRound - 1)) == 0));
    }
    return(false);
}

//@#LP move to toolbox math
bool ZoomHandler::valueIsCloseToPowerOfTwo(float v, float deltaAround, float& adjustedV) {
    if (v < 1.0) {
        //qDebug() << "#1";
        return(false);
    }

    double dRoundV = round(v);

    qDebug() << "v = " << v;
    qDebug() << "dRoundV = " << dRoundV;

    if (dRoundV > v ) {  //rounded to upper value (typical example: 128.727 => 129)
        dRoundV -= 1.0;
        qDebug() << "#2 dRoundV was round to upper value => " << dRoundV;
    }
    double absDelta = fabs(v - dRoundV);

    qDebug() << "#3 v = " << v;
    qDebug() << "#3 deltaAround = " << deltaAround;
    qDebug() << "#3 dRoundV  = " << dRoundV;
    qDebug() << "#3 absDelta = " << absDelta;

    bool bIsCloseToWholeNumber = absDelta < deltaAround;
    if (bIsCloseToWholeNumber) {
        qDebug() << "#4 bIsCloseToWholeNumber";
        int iRound =  dRoundV;        
        adjustedV = dRoundV;        

        qDebug() << "#4 iRound = " << iRound;
        qDebug() << "#4 adjustedV =" << adjustedV;

        bool bIsPowerOfTwo = (iRound != 0) && ((iRound &(iRound - 1)) == 0);
        return(bIsPowerOfTwo);
    }
    //qDebug() << "#5";
    //dRoundV = 888; //set to an arbitrary invalid value
    return(false);
}

int ZoomHandler::getIdxMax() {
    return(_vectZoomParameters.size()-1);
}

//idx has to exist in the possible index range
bool ZoomHandler::setToZoomFromIdx(int idx) {
    qDebug() << __FUNCTION__ << "idx =" << idx;
    qDebug() << __FUNCTION__ << "_idxZoom =" << _idxZoom;

    int ZLI_beforeIdxZoomUpdate = _vectZoomParameters[_idxZoom]._zoomLevelImage_forSceneSide;
    _idxZoom = idx;
    if (_idxZoom < 0) {
        qDebug() << __FUNCTION__ << "min reach";
        _idxZoom = 0;
        _zoomLevelImageTransition = ZoomHandler::e_zLIT_none;
        return(false);
     }

     if (_idxZoom >= _vectZoomParameters.size()) {
         qDebug() << __FUNCTION__ << "max reach";
         _idxZoom = _vectZoomParameters.size()-1;
         _zoomLevelImageTransition = ZoomHandler::e_zLIT_none;
         return(false);
     }
     qDebug() << __FUNCTION__ << "_idxZoom =" << _idxZoom;
     qDebug() << __FUNCTION__ << "ZLI_beforeIdxZoomUpdate =" << ZLI_beforeIdxZoomUpdate;

     update_zoomLevelImageTransition_withIdxZoom(ZLI_beforeIdxZoomUpdate);
     return(true);
}
