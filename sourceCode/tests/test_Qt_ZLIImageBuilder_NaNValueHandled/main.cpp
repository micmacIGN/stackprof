#include <QDebug>

#include "../../application/logic/io/ZoomOutLevelImageBuilder.h"

#include <QElapsedTimer> //for basic time spent measurement

int main(int argc, char* agrv[]) {

    QString qstrDirRootZLIStorage = "/home/laurent/tmp";

    //image with NaN here and there. The more interesting part is in the small white rectangular at middle, bottom.
    QString strInputImage = "/home/laurent/inputDataForTests/inputImages/test_PyramidImageBuild_withNan/test_nan5_big.tif";

    //image with NaN only at the bottom of a 10Kx10K pixels to check that the level image #4 is correclty rewrote after a first start writing level image #4 with the standard way
    //with the Nan count to detect to build differently
    //QString strInputImage = "/home/laurent/inputDataForTests/inputImages/test_PyramidImageBuild_withNan/float_withNanAtImageBottomOnly_10Kx10K.tif";


    ZoomOutLevelImageBuilder zoliBuilder(strInputImage, qstrDirRootZLIStorage);

    bool bReport = zoliBuilder.makeStorageDirectory(0);
    if (!bReport) {
        return(1);
    }

    QElapsedTimer timer;
    timer.start();

    bReport = true;
    int stepAmount_untilWidthOrHeightGreaterThan = 0;
    int stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel = 0;
    int stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_startFromZLI4 = 0;

    bReport &= zoliBuilder.computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan(
                1024, stepAmount_untilWidthOrHeightGreaterThan);
    bReport &= zoliBuilder.computeStepAmountForProgressBar_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel(
                1024,
                stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel,
                stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_startFromZLI4);

    qDebug() << __FUNCTION__ << "stepAmount_untilWidthOrHeightGreaterThan :"
             << stepAmount_untilWidthOrHeightGreaterThan;

    qDebug() << __FUNCTION__ << "stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel :"
             << stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel;

    qDebug() << __FUNCTION__ << "stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_startFromZLI4 :"
             << stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_startFromZLI4;

    bool bContainsNan = false;
    bReport = zoliBuilder.build_untilWidthOrHeightGreaterThan(1024, stepAmount_untilWidthOrHeightGreaterThan, bContainsNan);
    if (bContainsNan) {
        qDebug() << __FUNCTION__ << "restart from zoomLevel4 handling Nan";
        bReport = zoliBuilder.build_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_fromLevel(
                    4, 1024, stepAmount_untilWidthOrHeightGreaterThan_usingRootImageForEachLevel_startFromZLI4);
        if (!bReport) {
            return(1);
        }
    }

    qint64 elapsedTime = timer.elapsed();
    qDebug() << __FUNCTION__ << "took :" << elapsedTime << " ms ";

    return(0);
}
