
#include <QDateTime>

#include "IDGenerator.h"

//msec from 2010/01/01
qint64 IDGenerator::generateABoxesSetID() {

    QDateTime qdateTime_1970({1970,1,1},{0,0,0,0});
    QDateTime qdateTime_2010({2010,1,1},{0,0,0,0});

    qint64 msecFrom1970_to2010 = qdateTime_1970.msecsTo(qdateTime_2010);

    qint64 msecFrom1970_toCurrent = QDateTime::currentMSecsSinceEpoch();

    return(msecFrom1970_toCurrent - msecFrom1970_to2010);
}

