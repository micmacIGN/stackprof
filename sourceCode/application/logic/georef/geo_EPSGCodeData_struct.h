#ifndef geo_EPSGCodeData_struct_h
#define geo_EPSGCodeData_struct_h

#include <QHash>
#include <QString>
struct S_dataForAKnownEPSGCode_fromProjDB {
    QString strName;//zoneName
    QString strAreaName;
    //QString strProjectionMethodName;
    //int type;
};


struct S_dataForAZoneName_fromProjDB {
    QString strEPSGCode;
    QString strAreaName;
    //QString strProjectionMethodName;
    //int type;
};

#endif // geo_EPSGCodeData_struct_h
