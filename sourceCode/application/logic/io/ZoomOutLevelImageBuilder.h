#ifndef ZOOMOUTLEVELIMAGEBUILDER_H
#define ZOOMOUTLEVELIMAGEBUILDER_H

#include <QString>
#include <QObject>

#include "../vecttype.h"

using namespace std;

#include <OpenImageIO/imagebuf.h>
using namespace OIIO;

class VectorType;

class ZoomOutLevelImageBuilder : public QObject {
	
    Q_OBJECT

    public:
        ZoomOutLevelImageBuilder(const QString& qstrRootImage, const QString& qstrDirRootZLIStorage, QObject *parent = nullptr);

        bool makeStorageDirectory(qlonglong idForDir);

        bool absoluteDirNameForRootImageAndID(const QString& qstrRootImage, qlonglong idForDir, QString& qStrFullPathDirnameForZLI);

        bool evaluateContentStateOfZLIStoragePath(bool& bNeedCleanDirectory);
        bool cleanAndInitDirectory();

        //bool build_untilWidthOrHeightGreaterThan(int32_t whsize, int stepAmount);
        bool build_untilWidthOrHeightGreaterThan(int32_t whsize, int stepAmount, bool &bNanDetectedInZoomLevelFour);
        bool pixStats_NanCount(const ImageBuf ImageBufForNanCount, bool& bContainsNan);
        //build_untilWidthOrHeightGreaterThan send signal when a new step is reached when building images
        //but it does not know how steps is involved
        //computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan compute that separetaly
        bool computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan(int32_t whsize, int& stepAmount);
        bool computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel(int32_t whsize,
                                                                                                      int& stepAmount,
                                                                                                      int& stepAmount_startAtZLI4);
        bool build_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_fromLevel(int firstZLIToBuild, int32_t whsize, int stepAmount);

        struct S_InfosAboutZoolLevel {
            uint32_t _zoomLevel;
            uint32_t _imageWidth;
            uint32_t _imageHeight;
		};

        S_InfosAboutZoolLevel getInfosAboutHigherZoomLevel();
	
        QString getZLIStoragePath() const;

        int nearestPow2(int v); //@#LP: go to a 'math tools set"

    signals:
        void signal_stepProgressionForOutside(int value);

    private:
        //int nearestPow2(int v); //@#LP: go to a 'math tools set"

    private:
        QString _strInputImageInfos_jsonFile;

        QString _qstrRootImage;
        QString _qstrExtension; //of rootImage, used to create ZLI with the same extension
        QString _qstrDirRootZLIStorage;
        QString _qStrFullPathDirnameForZLI;

        S_InfosAboutZoolLevel _infosAboutHigherZoomLevel;

};

#endif
