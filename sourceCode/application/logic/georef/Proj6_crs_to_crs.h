#ifndef PROJ6_CRS_TO_CRS_H
#define PROJ6_CRS_TO_CRS_H

#include <string>
using namespace std;

#include <proj.h>

//
//avoiding Qt QString object usage, prefer std::string here when used very close to proj6 API; to stay close as possible to proj6 API
//

#include "geo_basic_struct.h"
#include "geo_EPSGCodeData_struct.h"

class Proj6_crs_to_crs {

public:

    Proj6_crs_to_crs();

    ~Proj6_crs_to_crs();

    bool createContext(const char *strPathForProjDataBase);

    int create_crs_to_crs(const char *source_crs, const char *target_crs, bool bNormalizeForDisplay);

    //c_in.lpzt.lam : Longitude
    //c_in.lpzt.phi : Latitude
    bool trans(PJ_COORD pjc_cin, PJ_DIRECTION pjDirection, PJ_COORD &pjc_out) const;

    void clear_PJContext();
    void clear_PJ();    
    void clear_PJ_PJContext();


    //bool getDataForAllKnownEPSGCodes(QHash<QString, S_dataForAKnownEPSGCode_fromProjDB>& qhash_str_SdfakEPSGCode);
    bool getDataForAllKnownEPSGCodes(QHash<QString, S_dataForAKnownEPSGCode_fromProjDB>& qhash_str_SdfakEPSGCode,
                                     QMultiMap<QString, S_dataForAZoneName_fromProjDB>& qMMap_str_SdfaZoneName);

private:
    PJ_CONTEXT* _projContext;
    PJ *_P;
};

//---
PJ_COORD set_PJ_COORD_withLonLat(double lon, double lat);
PJ_COORD set_PJ_COORD_withXY(S_xy xy);

double xOf(PJ_COORD pjc);
double yOf(PJ_COORD pjc);

double lonOf(PJ_COORD pjc);
double latOf(PJ_COORD pjc);

#endif
