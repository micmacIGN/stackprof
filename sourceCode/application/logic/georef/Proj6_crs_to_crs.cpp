#include <stdio.h>
#include <math.h>  //for HUGE_VAL

#include <proj.h>


#include <string.h>

#include "Proj6_crs_to_crs.h"

//this file have a unit tests which does not used Qt
//Moreover qDebug() considers only print double as float precision. Hence for double precision use cout with std::setprecision instead
#ifndef DONT_USE_QDEBUG__USE_COUT_INSTEAD
#define useQDebug
#endif
#ifdef useQDebug
    #include <QDebug>
    #define debugOut qDebug()
    #define debugEndl ""
#else
    #define debugOut cout
    #define debugEndl endl;
#endif


//https://proj.org/development/reference/functions.html#c.proj_create_crs_to_crs
//https://proj.org/development/migration.html

Proj6_crs_to_crs::Proj6_crs_to_crs(): _projContext(nullptr), _P(nullptr) {

}

Proj6_crs_to_crs::~Proj6_crs_to_crs() {
    clear_PJ_PJContext();
}

bool Proj6_crs_to_crs::createContext(const char *strPathForProjDataBase) {  //@#LP limited to one path for now (will take only the first of tstrPathForProjDataBase)

    qDebug() << __FUNCTION__ << strPathForProjDataBase;

    //#define DEF_MAX_SIZE_FOR_PROJDB_PATH 127 //127 for windows should be ok also
    #define DEF_MAX_SIZE_FOR_PROJDB_PATH 250 //@#LP 250 char for windows should be ok also
    //https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file#:~:text=In%20the%20Windows%20API%20(with,and%20a%20terminating%20null%20character.
    //Enable Long Paths in Windows 10, Version 1607, and Later
    //Starting in Windows 10, version 1607, MAX_PATH limitations have been removed from common Win32 file and directory functions. However, you must opt-in to the new behavior.

    int strlen_strPathForProjDataBase = strlen(strPathForProjDataBase);
    if (strlen_strPathForProjDataBase > DEF_MAX_SIZE_FOR_PROJDB_PATH) {
        qDebug() << __FUNCTION__ << "error: strPathForProjDataBase is too long (= " << strlen_strPathForProjDataBase << " )";
        qDebug() << __FUNCTION__ << "Max size is: " << DEF_MAX_SIZE_FOR_PROJDB_PATH;
        return(false);
    }

    char *cProjDatabasePath[1] = { nullptr };
    cProjDatabasePath[0] = new char[DEF_MAX_SIZE_FOR_PROJDB_PATH+1];
    if (!cProjDatabasePath[0]) {
        qDebug() << __FUNCTION__ << "error: allocating cProjDatabasePath[0]";
        return(false);
    }
    memset (cProjDatabasePath[0], 0, DEF_MAX_SIZE_FOR_PROJDB_PATH+1);
    strncpy(cProjDatabasePath[0], strPathForProjDataBase, strlen(strPathForProjDataBase));
    debugOut << __FUNCTION__ << "(1) : cProjDatabasePath[0] = " << cProjDatabasePath[0] << debugEndl;

    if (_projContext) {
        qDebug() << __FUNCTION__ << "error: _projContext not nullptr";
        delete[] cProjDatabasePath[0];
        return(false);
    }

    _projContext = proj_context_create();
    if (!_projContext) {
        //@#LP add error msg here
        debugOut << __FUNCTION__ << "error: failed to proj_context_create" << debugEndl;
        delete[] cProjDatabasePath[0];
        return(false);
    }

    //https://github.com/OSGeo/PROJ/blob/2eb2798b1ef31d04c717856d82acd4ccb7aac78a/src/open_lib.cpp#L204
    /*const char* const cProjDatabasePath[1] = {"./projdb/" }; //"/home/laurent/work/dev_techtest_sandbox/georef/test_proj6/"};
    proj_context_set_search_paths(projContext, 1, cProjDatabasePath);

    PJ_INFO projInfo = proj_info();
    fprintf(stdout, "proj searchpath = %s\n", projInfo.searchpath);
    */

    //fprintf(stdout, "(2) %s : cProjDatabasePath[0] = %s\n", __FUNCTION__ , cProjDatabasePath[0]);

    qDebug() << __FUNCTION__ << "cProjDatabasePath[0] = " << cProjDatabasePath[0];

    //@#LP limited to one path for now
    //const char * /*const*/ cProjDatabasePath_[1] = {"./projdb/" }; //"/home/laurent/work/dev_techtest_sandbox/georef/test_proj6/"};
    proj_context_set_search_paths(_projContext, 1, cProjDatabasePath    /*cProjDatabasePath_*/);

    //@#LP added path is taken into account, but not visible with  proj_info().searchpath (?)
    //PJ_INFO projInfo = proj_info();
    //fprintf(stdout, "proj searchpath = %s\n", projInfo.searchpath);

    delete[] cProjDatabasePath[0];
    return(true);
}

int Proj6_crs_to_crs::create_crs_to_crs(const char *source_crs, const char *target_crs, bool bNormalizeForDisplay) {
    //@#LP unsync behavior in case of aready allocated, an API choice has to be done

    if (!_projContext) {
        debugOut << __FUNCTION__ << "error: _projContext is nullptr" << debugEndl;
        return(1);
    }

    if (_P) {
        debugOut << __FUNCTION__ << "error: _P not nullptr" << debugEndl;
        return(2);
    }

    // NOTE: the use of PROJ strings to describe CRS is strongly discouraged
    // in PROJ 6, as PROJ strings are a poor way of describing a CRS, and
    // more precise its geodetic datum.
    // Use of codes provided by authorities (such as "EPSG:4326", etc...)
    // or WKT strings will bring the full power of the "transformation
    // engine" used by PROJ to determine the best transformation(s) between
    // two CRS.

    debugOut << __FUNCTION__ << "source_crs codeEPSG: " << source_crs << debugEndl;
    debugOut << __FUNCTION__ << "target_crs codeEPSG: " << target_crs << debugEndl;

    _P = proj_create_crs_to_crs(_projContext,
                               source_crs,
                               target_crs,
                               nullptr);
    if (!_P) {
        debugOut << __FUNCTION__ << "error: failed to proj_create_crs_to_crs" << debugEndl;

        int proj_contect_error_status =  proj_context_errno(_projContext);
        if (proj_contect_error_status) {
            debugOut << __FUNCTION__ << "proj_contect_error_status = " << proj_contect_error_status
                                     << " ( string: " << proj_errno_string(proj_contect_error_status) << debugEndl;
        }
        clear_PJContext();
        return(3);
    }

    debugOut << __FUNCTION__ << "bNormalizeForDisplay is " << bNormalizeForDisplay << debugEndl;

    if (bNormalizeForDisplay) {

        // Original comment with proj string used instead of EPSG codes:
        // '// For that particular use case, this is not needed.
        //  // proj_normalize_for_visualization() ensures that the coordinate
        //  // order expected and returned by proj_trans() will be longitude,
        //  // latitude for geographic CRS, and easting, northing for projected
        //  // CRS. If instead of using PROJ strings as above, "EPSG:XXXX" codes
        //  // had been used, this might had been necessary.'

        /*
        https://proj.org/development/reference/functions.html
        '
        PJ *proj_normalize_for_visualization(PJ_CONTEXT *ctx, const PJ* obj)
        New in version 6.1.0.

        Returns a PJ* object whose axis order is the one expected for visualization purposes.

        The input object must be a coordinate operation, that has been created with proj_create_crs_to_crs().
        If the axis order of its source or target CRS is northing,easting, then an axis swap operation will be inserted.
        The returned PJ-pointer should be deallocated with proj_destroy().'
        */

        PJ* P_for_GIS = proj_normalize_for_visualization(_projContext, _P);
        if( nullptr == P_for_GIS )  {
            debugOut << __FUNCTION__ << "error: failed to proj_normalize_for_visualization" << debugEndl;
            clear_PJ();
            return(4);
        }
        int proj_error_status =  proj_errno(P_for_GIS);
        if (proj_error_status) {
            //fprintf(stdout, "proj_errno = %d  string: %s\n", proj_error_status, proj_errno_string(proj_error_status));
            debugOut << __FUNCTION__ << "proj_contect_error_status = " << proj_error_status
                                     << " ( string: " << proj_errno_string(proj_error_status) << debugEndl;
            proj_errno_reset(P_for_GIS);
            proj_destroy(P_for_GIS);
            clear_PJ();
            return(5);
        }
        clear_PJ();
        _P = P_for_GIS;
    }

    return(0);
}

bool Proj6_crs_to_crs::trans(PJ_COORD pjc_cin, PJ_DIRECTION pjDirection, PJ_COORD &pjc_out) const {

    if (!_P) {
        //@#LP improve error msg
        debugOut << __FUNCTION__ << "error: if (!_P) {" << debugEndl;
        return(false);
    }

    // For reliable geographic <--> geocentric conversions, z shall not
    // be some random value. Also t shall be initialized to HUGE_VAL to
    // allow for proper selection of time-dependent operations if one of
    // the CRS is dynamic.
    pjc_cin.lpzt.z = 0.0; //Vertical component
    pjc_cin.lpzt.t = HUGE_VAL; //Time component

    //@LP No need to convert to radian

    //fprintf(stdout, "sizeof(PJ_COORD)=%ld\n", sizeof(PJ_COORD));
    memset(&pjc_out, 0, sizeof(PJ_COORD));

    pjc_out = proj_trans(_P, pjDirection, pjc_cin);

    int proj_error_status =  proj_errno(_P);
    if (proj_error_status) {
        //fprintf(stdout, "proj_errno = %d  string: %s\n", proj_error_status, proj_errno_string(proj_error_status));
        debugOut << __FUNCTION__ << "proj_contect_error_status = " << proj_error_status
                                 << " ( string: " << proj_errno_string(proj_error_status) << debugEndl;
        proj_errno_reset(_P);
        return(false);
    }
    return(true);
}

void Proj6_crs_to_crs::clear_PJ() {
    if (_P) {
        proj_destroy(_P);
        _P = nullptr;
    }
}

void Proj6_crs_to_crs::clear_PJContext() {
    if (_projContext) {
        proj_context_destroy(_projContext);
        _projContext = nullptr;
    }
}

void Proj6_crs_to_crs::clear_PJ_PJContext() {
    clear_PJ();
    clear_PJContext();
}

PJ_COORD set_PJ_COORD_withLonLat(double lon, double lat) {

    /*
    PJ_COORD pjc_LamPhi;
    pjc_LamPhi.lpzt.lam = lon;
    pjc_LamPhi.lpzt.phi = lat;
    return(pjc_LamPhi);
    */

    PJ_COORD pjc_uv;    
    pjc_uv.uv.u = lon;
    pjc_uv.uv.v = lat;
    return(pjc_uv);
}

PJ_COORD set_PJ_COORD_withXY(S_xy xy) {
//PJ_COORD set_PJ_COORD_withXY(double x, double y) {

    /*
    PJ_COORD pjc_LamPhi;
    pjc_LamPhi.xy.x = xy._x;
    pjc_LamPhi.xy.y = xy._y;
    return(pjc_LamPhi);
    */

    PJ_COORD pjc_uv;
    pjc_uv.uv.u = xy._x;
    pjc_uv.uv.v = xy._y;
    return(pjc_uv);
}

double xOf(PJ_COORD pjc) { return(/*pjc.xy.x*/pjc.uv.u); }
double yOf(PJ_COORD pjc) { return(/*pjc.xy.y*/pjc.uv.v); }

double lonOf(PJ_COORD pjc) { return(/*pjc.lpzt.lam*/pjc.uv.u); }
double latOf(PJ_COORD pjc) { return(/*pjc.lpzt.phi*/pjc.uv.v); }

#include <QMap>

bool Proj6_crs_to_crs::getDataForAllKnownEPSGCodes(QHash<QString, S_dataForAKnownEPSGCode_fromProjDB>& qhash_str_SdfakEPSGCode,
                                                   QMultiMap<QString, S_dataForAZoneName_fromProjDB>& qMMap_str_SdfaZoneName) {

    /*https://proj.org/development/reference/functions.html?highlight=proj_get_crs_info_list_from_database#_CPPv436proj_get_crs_info_list_from_databaseP10PJ_CONTEXTPKcPK24PROJ_CRS_LIST_PARAMETERSPi
    ' Return
      an array of PROJ_CRS_INFO* pointers to be freed with proj_crs_info_list_destroy(), or NULL in case of error.

      Parameters
      ctx: PROJ context, or NULL for default context

      auth_name: Authority name, used to restrict the search. Or NULL for all authorities.

      params: Additional criteria, or NULL. If not-NULL, params SHOULD have been allocated by proj_get_crs_list_parameters_create(),
      as the PROJ_CRS_LIST_PARAMETERS structure might grow over time.

      out_result_count: Output parameter pointing to an integer to receive the size of the result list. Might be NULL

      (...) '
    */


    qhash_str_SdfakEPSGCode.clear();
    qMMap_str_SdfaZoneName.clear();

    if (!_projContext) {
        return(false);
    }

    PROJ_CRS_INFO **PROJ_CRS_INFO_Array = nullptr;
    int out_result_count = 0;
    PROJ_CRS_INFO_Array = proj_get_crs_info_list_from_database(_projContext, "EPSG", NULL, &out_result_count);

    qDebug() << "out_result_count = " << out_result_count;

    if (!out_result_count) {
        return(false);
    }

    for (int i = 0; i < out_result_count; i++) {
        qhash_str_SdfakEPSGCode.insert(
            PROJ_CRS_INFO_Array[i]->code,
            { PROJ_CRS_INFO_Array[i]->name,
              PROJ_CRS_INFO_Array[i]->area_name
            }
        );

        /*QString strZoneName = PROJ_CRS_INFO_Array[i]->name;
        if (qMMap_str_SdfaZoneName.find(strZoneName) != qMMap_str_SdfaZoneName.end()) {
            //strZoneName.append("(alternative?)");
        }*/

        qMMap_str_SdfaZoneName.insert(
            PROJ_CRS_INFO_Array[i]->name,//strZoneName,
            { PROJ_CRS_INFO_Array[i]->code,
              PROJ_CRS_INFO_Array[i]->area_name
            }
        );
    }


    /*
    for (auto iter_map = qMMap_str_SdfaZoneName.cbegin(); iter_map != qMMap_str_SdfaZoneName.cend(); ++iter_map) {
        qDebug() << " zoneName:   " <<iter_map.key();
        qDebug() << " code:      " << iter_map.value().strEPSGCode;
        qDebug() << " area_name: " << iter_map.value().strAreaName;
        qDebug();
    }
    */

#ifdef false
    qDebug() <<
       "i loop" << ";" <<
        "auth_name" << ";" <<
        "code"      << ";" <<
        "name"      << ";" <<
        "area_name" << ";" <<
        "projection_method_name" << ";" <<
        "type";

    for (int i = 0; i < out_result_count; i++) {
        qDebug() << i << ";" <<
        /*qDebug() << " auth_name: " <<*/ PROJ_CRS_INFO_Array[i]->auth_name << ";" <<
        /*qDebug() << " code:      " <<*/ PROJ_CRS_INFO_Array[i]->code      << ";" <<
        /*qDebug() << " name:      " <<*/ PROJ_CRS_INFO_Array[i]->name      << ";" <<
        /*qDebug() << " area_name: " <<*/ PROJ_CRS_INFO_Array[i]->area_name << ";" <<
        /*qDebug() << " projection_method_name: " <<*/ PROJ_CRS_INFO_Array[i]->projection_method_name << ";" <<
        /*qDebug() << " type: " <<*/ PROJ_CRS_INFO_Array[i]->type;
    }
#endif

    proj_crs_info_list_destroy(PROJ_CRS_INFO_Array);

    return(true);
}

