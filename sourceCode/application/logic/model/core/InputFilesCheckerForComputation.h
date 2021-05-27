#ifndef CoreComputation_inputFilesChecker_H
#define CoreComputation_inputFilesChecker_H

#include "ComputationCore_structures.h"

class InputFilesForComputationMatchWithRequierement {

public:
  InputFilesForComputationMatchWithRequierement(
    const S_InputFiles& inputFiles,
    e_mainComputationMode e_mainComputationMode,
    U_CorrelationScoreMapFileUseFlags correlationScoreUsageFlags);

  bool check_PX1PX2Together_DeltazAlone(QString& strMsgErrorDetails);
  bool getLayerToUseForComputationFlag(e_LayersAcces eLA);

  enum e_statusFileCheck { e_sFC_notSet, //notSet means default value never set, but in this case we use also to indicates that the according file is empty ('not set')
                           e_sFC_checkInProgress,
                           e_sFC_OK,
                           e_sFC_NotOK,
                           e_sFC_notRequiered };

  //int getQVectorLayersToComputeFlag(QVector<e_statusFileCheck> &qvect_e_statusFileCheck);
  int getQVectorLayersForComputationFlag(QVector<bool> &qvectb_LayersForComputationFlag);

  int getQVectorCorrelationScoreMapFilesAvailabilityFlag(QVector<bool> &qvectb_correlationScoreMapFilesAvailability);

private:

  InputFilesForComputationMatchWithRequierement::e_statusFileCheck convertInProgressToFinalValue(e_statusFileCheck esFC);

  S_InputFiles _inputFiles;
  e_mainComputationMode _e_mainComputationMode;
  U_CorrelationScoreMapFileUseFlags _correlationScoreUsageFlags;

  QVector<e_statusFileCheck> _qvect_eSFC_PX1_PX2_DeltaZ;
  QVector<e_statusFileCheck> _qvect_eSFC_correlScoreFilesFor_PX1_PX2_DeltaZ;

  //QString _strMsgErrorDetails;
};

#endif
