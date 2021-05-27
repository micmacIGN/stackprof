#include <OpenImageIO/imageio.h>
using namespace OIIO;

#include <QDebug>

#include "InputFilesCheckerForComputation.h"

#include "../../io/InputImageFormatChecker.h"

#include "../../toolbox/toolbox_pathAndFile.h"

InputFilesForComputationMatchWithRequierement::InputFilesForComputationMatchWithRequierement(
        const S_InputFiles& inputFiles,
        e_mainComputationMode e_mainComputationMode,
        U_CorrelationScoreMapFileUseFlags correlationScoreUsageFlags):
        _inputFiles(inputFiles),
        _e_mainComputationMode(e_mainComputationMode),
        _correlationScoreUsageFlags(correlationScoreUsageFlags),
        _qvect_eSFC_PX1_PX2_DeltaZ{e_sFC_notSet, e_sFC_notSet, e_sFC_notSet},
        _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ {e_sFC_notSet, e_sFC_notSet, e_sFC_notSet} {

    qDebug() << __FUNCTION__<< "inputFiles:";
    qDebug() << __FUNCTION__<< inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__<< inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ;

    qDebug() << __FUNCTION__<< "_inputFiles:";
    qDebug() << __FUNCTION__<< _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__<< _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ;

}

bool InputFilesForComputationMatchWithRequierement::getLayerToUseForComputationFlag(e_LayersAcces eLA) {
    qDebug() << __FUNCTION__<< "_eSFC_PX1_PX2_DeltaZ[" << eLA << "] =" <<  _qvect_eSFC_PX1_PX2_DeltaZ[eLA];
    return(_qvect_eSFC_PX1_PX2_DeltaZ[eLA] == e_sFC_OK);
}


int InputFilesForComputationMatchWithRequierement::getQVectorLayersForComputationFlag(QVector<bool> &qvectb_LayersForComputationFlag) {
    qvectb_LayersForComputationFlag.clear();
    qvectb_LayersForComputationFlag.fill(false, eLA_LayersCount);
    int count_sFC_OK = 0;
    for (int iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        if (_qvect_eSFC_PX1_PX2_DeltaZ[iela] == e_sFC_OK) {
            qvectb_LayersForComputationFlag[iela] = true;
            count_sFC_OK++;
        }
    }
    return(count_sFC_OK);
}


int InputFilesForComputationMatchWithRequierement::getQVectorCorrelationScoreMapFilesAvailabilityFlag(QVector<bool> &qvectb_correlationScoreMapFilesAvailability) {

    qvectb_correlationScoreMapFilesAvailability.clear();
    qvectb_correlationScoreMapFilesAvailability.fill(false, eLA_LayersCount);

    //e_mCM_Typical_PX1PX2Together_DeltazAlone: optionnal but together: (PX1, PX2), optionnal and alone: DeltaZ
    if (_e_mainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        //strMsgErrorDetails = "Dev error 901# mainComputationMode not supported";
        return(false);
    }

    int count_OK = 0;
    qvectb_correlationScoreMapFilesAvailability[eLA_CorrelScoreForPX1PX2Together] =
            (_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] == e_sFC_OK);

    qvectb_correlationScoreMapFilesAvailability[eLA_CorrelScoreForDeltaZAlone] =
            (_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone] == e_sFC_OK);

    return(qvectb_correlationScoreMapFilesAvailability.count(true));
}


//routeset file check for importation is not considered in this method
bool InputFilesForComputationMatchWithRequierement::check_PX1PX2Together_DeltazAlone(QString& strMsgErrorDetails) {

    strMsgErrorDetails.clear();

    QString _tqstrDescName_image[3] {"PX1", "PX2", "DeltaZ or Other"};
    QString _tqstrDescName_correlScoreMap[3] {"(PX1,PX2)", "(PX1,PX2)", "DeltaZ or Other"};

    _qvect_eSFC_PX1_PX2_DeltaZ                     = {e_sFC_notSet, e_sFC_notSet, e_sFC_notSet};
    _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = {e_sFC_notSet, e_sFC_notSet, e_sFC_notSet};

    //input files (PX1, PX2, deltaZ):

    //e_mCM_Typical_PX1PX2Together_DeltazAlone: optionnal but together: (PX1, PX2), optionnal and alone: DeltaZ
    if (_e_mainComputationMode != e_mCM_Typical_PX1PX2Together_DeltazAlone) {
        strMsgErrorDetails = "Dev error #9 mainComputationMode not supported";
        return(false);
    }

    qDebug() << __FUNCTION__
              << __LINE__
              << "_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ = "
              << _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ;

    //test if (PX1, PX2) needs to be computed
    bool bPX1PX2_empty = false;
    e_statusFileCheck esFC_PX1PX2 = e_sFC_notSet;
    bool bPX1_empty = _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX1].isEmpty();
    bool bPX2_empty = _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX2].isEmpty();
    if (bPX1_empty && bPX2_empty) {
        _qvect_eSFC_PX1_PX2_DeltaZ[eLA_PX1] = e_sFC_notRequiered;
        _qvect_eSFC_PX1_PX2_DeltaZ[eLA_PX2] = e_sFC_notRequiered;
        esFC_PX1PX2 = e_sFC_notRequiered;
        bPX1PX2_empty = true;
    } else {
        if (bPX1_empty) {
            strMsgErrorDetails = "Missing mandatory non independant input file: " + _tqstrDescName_image[eLA_PX1];
            qDebug() << __FUNCTION__<< strMsgErrorDetails;
            return(false);
        }
        if (bPX2_empty) {
            strMsgErrorDetails = "Missing mandatory non independant input file: " + _tqstrDescName_image[eLA_PX2];
            qDebug() << __FUNCTION__<< strMsgErrorDetails;
            return(false);
        }
        _qvect_eSFC_PX1_PX2_DeltaZ[eLA_PX1] = e_sFC_checkInProgress;
        _qvect_eSFC_PX1_PX2_DeltaZ[eLA_PX2] = e_sFC_checkInProgress;
        esFC_PX1PX2 = e_sFC_checkInProgress;
    }

    //test if deltaZOther needs to be computed
    bool bdeltaZ_empty = _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_deltaZ].isEmpty();
    if (bdeltaZ_empty) {
        _qvect_eSFC_PX1_PX2_DeltaZ[eLA_deltaZ] = e_sFC_notRequiered;
    } else {
        _qvect_eSFC_PX1_PX2_DeltaZ[eLA_deltaZ] = e_sFC_checkInProgress;
    }

    if (bPX1PX2_empty && bdeltaZ_empty) {
        strMsgErrorDetails = "None input image file";
        return(false);
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;

    //check that input correlation score files are set if usage at true
    //PX1PX2
    if (!_correlationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onPX1PX2) {
        _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] = e_sFC_notRequiered;
    } else {
        bool bNoError = !_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].isEmpty();
        if (!bNoError) {
            strMsgErrorDetails = "Missing mandatory correlation score input file for PX1,PX2";
            return(false);
        }
        _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] = e_sFC_checkInProgress;
    }

    //DeltaZ
    if (!_correlationScoreUsageFlags._sCSF_PX1PX2Together_DeltazAlone._b_onDeltaZ) {
        _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone] = e_sFC_notRequiered;
    } else {
        bool bNoError = !_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone].isEmpty();
        if (!bNoError) {
            strMsgErrorDetails = "Missing mandatory correlation score input file for DeltaZ";
            return(false);
        }
        _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone] = e_sFC_checkInProgress;
    }

    if (esFC_PX1PX2 == e_sFC_checkInProgress) {
        //check that input files PX1 and PX2 are different when e_mCM_Typical_PX1PX2Together_DeltazAlone
        if (!_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX1].compare(
             _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX2],
             Qt::CaseInsensitive)) {
            strMsgErrorDetails = "Mandatory non independant input files have to be different";
            return(false);
        }
    }


    //check that files exists: PX1, PX2, DeltaZ
    for (uint iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        if (_qvect_eSFC_PX1_PX2_DeltaZ[iela] == e_sFC_checkInProgress) {
            if (!fileExists(_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela])) {
                _qvect_eSFC_PX1_PX2_DeltaZ[iela] = e_sFC_NotOK;
                strMsgErrorDetails = "Input file does not exist for " + _tqstrDescName_image[iela];
                return(false);
            }
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;


    QVector<uint> qvect_iela_forCorrelScoreMap = {eLA_CorrelScoreForPX1PX2Together, eLA_CorrelScoreForDeltaZAlone };

    //check that files exists: correlation score map for (PX1,PX2)
    for (auto iter_iela_forCorScoMap : qvect_iela_forCorrelScoreMap) {
        if (_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[iter_iela_forCorScoMap] == e_sFC_checkInProgress) {
            if (!fileExists(_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iter_iela_forCorScoMap])) {
                _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[iter_iela_forCorScoMap] = e_sFC_NotOK;
                strMsgErrorDetails = "(correlation score map) input file does not exist for " + _tqstrDescName_correlScoreMap[iter_iela_forCorScoMap];
                return(false);
            }
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;


    //check that PX1 and PX2 files format are valid for DisplacementMap file
    //added deltaZ here
    InputImageFormatChecker IIFC;
    ImageSpec imgSpec_PX1_PX2_deltaZ[eLA_LayersCount];
    for (uint iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        if (_qvect_eSFC_PX1_PX2_DeltaZ[iela] == e_sFC_checkInProgress) {
            bool bNoError = true;
            bNoError &= IIFC.getImageSpec(_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[iela].toStdString(), imgSpec_PX1_PX2_deltaZ[iela]);
            if (!bNoError) {
                strMsgErrorDetails = "(displacement map) failed to getImageSpec for file " + _tqstrDescName_image[iela];
               _qvect_eSFC_PX1_PX2_DeltaZ[iela]=e_sFC_NotOK;
                return(false);
            }
            if (!IIFC.formatIsAnAcceptedFormatForDisplacementMap(imgSpec_PX1_PX2_deltaZ[iela])) {
                _qvect_eSFC_PX1_PX2_DeltaZ[iela]=e_sFC_NotOK;
                strMsgErrorDetails = "(displacement map) file format not supported for file " + _tqstrDescName_image[iela];
                return(false);
            }
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;

    if (esFC_PX1PX2 == e_sFC_checkInProgress) {
        //check that PX1 and PX2 have the same size and typeDescBaseType
        if (!IIFC.checkSameWidthHeightBaseType(imgSpec_PX1_PX2_deltaZ[eLA_PX1],imgSpec_PX1_PX2_deltaZ[eLA_PX2], true)) {
            _qvect_eSFC_PX1_PX2_DeltaZ[eLA_PX1]=e_sFC_NotOK;
            _qvect_eSFC_PX1_PX2_DeltaZ[eLA_PX2]=e_sFC_NotOK;
            strMsgErrorDetails = _tqstrDescName_image[eLA_PX1]
                                 + " and "
                                 + _tqstrDescName_image[eLA_PX2]
                                 + " don't have the same width, height or basetype";
            return(false);
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;


    //check that correlation score map for (PX1,PX2) deltaZalone are valid for correlation score map
    ImageSpec imgSpec_CorScoMap_PX1PX2_PX1PX2_deltaZ[eLA_LayersCount];
    for (auto iter_iela_forCorScoMap : qvect_iela_forCorrelScoreMap) {
        if (_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[iter_iela_forCorScoMap] == e_sFC_checkInProgress) {
            bool bNoError = true;
            bNoError &= IIFC.getImageSpec(_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[iter_iela_forCorScoMap].toStdString(),
                                          imgSpec_CorScoMap_PX1PX2_PX1PX2_deltaZ[iter_iela_forCorScoMap]);
            if (!bNoError) {
                strMsgErrorDetails = "(correlation score map) failed to getImageSpec for file " + _tqstrDescName_correlScoreMap[iter_iela_forCorScoMap];
                _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[iter_iela_forCorScoMap] = e_sFC_NotOK;
                return(false);
            }

            if (!IIFC.formatIsAnAcceptedFormatForCorrelationScoreMap(imgSpec_CorScoMap_PX1PX2_PX1PX2_deltaZ[iter_iela_forCorScoMap])) {
                _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[iter_iela_forCorScoMap] = e_sFC_NotOK;
                strMsgErrorDetails = "(correlation score map) file format not supported for file " + _tqstrDescName_correlScoreMap[iter_iela_forCorScoMap];
                return(false);
            }
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;


    //check that all the input file have the same width and height
    //
    //. PX1 and PX2 together already check above about w/h/basetype

    //. check that deltaZ have same w/h than PX1
    //  do not check about basetype, it's ok to be different
    if (  (esFC_PX1PX2 == e_sFC_checkInProgress)
        &&(_qvect_eSFC_PX1_PX2_DeltaZ[eLA_deltaZ] == e_sFC_checkInProgress)) {
        if (!IIFC.checkSameWidthHeightBaseType(
                 imgSpec_PX1_PX2_deltaZ[eLA_deltaZ],
                 imgSpec_PX1_PX2_deltaZ[eLA_PX1],
                 false)) {
            _qvect_eSFC_PX1_PX2_DeltaZ[eLA_deltaZ] = e_sFC_NotOK;
           strMsgErrorDetails = _tqstrDescName_image[eLA_deltaZ]
                                  + " don't have the same width or height than "
                                  + _tqstrDescName_image[eLA_PX1]
                                  + " and "
                                  + _tqstrDescName_image[eLA_PX2];
            return(false);
        }
    }

    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;


    //the file to use for comparison needs to be adjusted, depending of what is provided as input files
    e_LayersAcces ela_forWHComparison = eLA_PX1;
    if (esFC_PX1PX2 == e_sFC_notRequiered) {
        ela_forWHComparison = eLA_deltaZ;
    }
    //check now about correlation score map
    for (auto iter_iela_forCorScoMap : qvect_iela_forCorrelScoreMap) {
        if (_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[iter_iela_forCorScoMap] == e_sFC_checkInProgress) {
           if (!IIFC.checkSameWidthHeightBaseType(imgSpec_CorScoMap_PX1PX2_PX1PX2_deltaZ[iter_iela_forCorScoMap],
                                                  imgSpec_PX1_PX2_deltaZ[ela_forWHComparison],
                                                  false)) {
                _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[iter_iela_forCorScoMap] = e_sFC_NotOK;
                strMsgErrorDetails = _tqstrDescName_correlScoreMap[iter_iela_forCorScoMap]
                                      + "don't have the same width or height than the other";
                return(false);
            }
        }
    }


    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << __LINE__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;


    for (uint iela = eLA_PX1; iela <= eLA_deltaZ; iela++) {
        _qvect_eSFC_PX1_PX2_DeltaZ[iela] = convertInProgressToFinalValue(_qvect_eSFC_PX1_PX2_DeltaZ[iela]);
    }

    qDebug() << __FUNCTION__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] = " <<
             _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together];

    qDebug() << __FUNCTION__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone] = " <<
             _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone];

    _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] = convertInProgressToFinalValue(
    _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] );

    _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone] = convertInProgressToFinalValue(
    _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]);

    qDebug() << __FUNCTION__ << "_qvect_eSFC_PX1_PX2_DeltaZ = " << _qvect_eSFC_PX1_PX2_DeltaZ;
    qDebug() << __FUNCTION__ << "_qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ = " << _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;

    return(true);
}

InputFilesForComputationMatchWithRequierement::e_statusFileCheck InputFilesForComputationMatchWithRequierement::convertInProgressToFinalValue(e_statusFileCheck esFC) {
    if (esFC == e_sFC_checkInProgress) {
        return(e_sFC_OK);
    } else {
        return(esFC);
    }
}

