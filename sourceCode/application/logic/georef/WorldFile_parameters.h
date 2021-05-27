#ifndef WORLDFILE_PARAMETERS_H
#define WORLDFILE_PARAMETERS_H

#include <QString>
#include <QVector>

#include <QJsonObject>

//each json keys contain: array index, matching array letter and meaning:
/*
0_A_xScale
1_D_rotationTerms_1
2_B_rotationTerms_2
3_E_yScale
4_C_xTranslationTerm
5_F_yTranslationTerm
*/

/*
    20.17541308822119 - A
    0.00000000000000 - D
    0.00000000000000 - B
    -20.17541308822119 - E
    424178.11472601280548 - C
    4313415.90726399607956 - F

    tfw:
    coeff[6]=
    [0]:A : x-scale; dimension of a pixel in map units in x direction
    [1]:D : rotation terms
    [2]:B : rotation terms
    [3]:E : negative of y-scale; dimension of a pixel in map units in y direction
    [4]:C : translation terms; x,y map coordinates of the center of the upper left pixel
    [5]:F : translation terms; x,y map coordinates of the center of the upper left pixel

    C and F: center of upper left pixel => 0.5, 0.5
*/
union U_WFparameters {
    struct {
        double _xScale;
        double _rotationTerms_1;
        double _rotationTerms_2;
        double _yScale;
        double _xTranslationTerm_centerOfUpperLeftPixel;
        double _yTranslationTerm_centerOfUpperLeftPixel;
    } _parameters;
    struct {
        double _doubleArray[6];
    } _raw;
};

/*
    For reliability and convenience, we use a struct of boolean flags indicating
    which field was zero (0 or 0.000 or 0000.00, etc) from the text string .TFW file
    avoiding detecting zero from numeric value.
    The reason is to avoid to consider very small value like 0.0000001 as a zero value.
*/
union U_WFparameters_seenAsZero {
    struct {
        bool _b_xScale;
        bool _b_rotationTerms_1;
        bool _b_rotationTerms_2;
        bool _b_yScale;
        bool _b_xTranslationTerm_centerOfUpperLeftPixel;
        bool _b_yTranslationTerm_centerOfUpperLeftPixel;
    } _b;
    struct {
        bool _boolArray[6];
    } _raw;
};

class WorldFile_parameters {

    public:
        WorldFile_parameters();

        bool findWorldFilesFromAssociatedImagePathAndFilename(QString strImagePathAndFilename, QVector<QString>& qvectqtr_worlfFileFound, QString& qstrWorldFilePreset);
        bool loadFromFile(QString strWorldFile);

        bool toQJsonObject(QJsonObject& qjsonObj) const;
        bool fromQJsonObject(const QJsonObject &qjsonObj);

        //void setCodeEPSG(QString qstrCodeEPSG);

        const U_WFparameters& getConstRef_WFparameters() const;
        //QString get_qstrCodeEPSG() const;

        // bool isReady();

        void showContent() const;
        void show_WFParameters() const;

        bool checkSame(const WorldFile_parameters& WFparamForCheck) const;

        bool canBeUsed() const;
        //bool geoImageXYComputable() const;

        //void replaceWFparametersAndAssociatedFielsBy(const WorldFile_parameters& WFparam);

        bool get_xScale(double& xScale) const;
        bool get_yScale(double& yScale) const;

        void setDefaultAsNoGeorefUsed();

    private:

        bool loadPresetWorldFile();
        bool loadWFParametersFromWorldFile(QString qstrFilename, U_WFparameters &WFparameters, U_WFparameters_seenAsZero &WFparameters_seenAsZero);

        void computeFlag_bWFparametersCanBeUsed();

        U_WFparameters _WFparameters;
        U_WFparameters_seenAsZero _WFparameters_seenAsZero;
        bool _bWFparametersCanBeUsed;
        QString _qstrWorldFilePathFilename;

        //QString _qstrCodeEPSG;

        QString _qstrAssociatedImagePathFilename;        
};


#endif //WORLDFILE_PARAMETERS_H
