#include <QDebug>

#include "../../application/logic/model/dialogProfilsCurves/landmarks.h"

void test_landmarkComputation();

int main(int argc, char *argv[]) {

    test_landmarkComputation();

}

void test_landmarkComputation() {

    double yLandmark_rangeTest[9][2]= {
        {    0         ,   100        },
        {   -0.080e-2  ,     0.0280e-2},
        {  -66.6632   ,    -60.4132   },
        {  -66.6632e-3,    -60.4132e-5},
        {   66.6632e1 ,     60.4132e4 },
        {   10.0      ,  13000.0      },
        {    1.23     ,     14.5      },
        {12345        , 123456        },
        {123456       , 1234567       }
    };

    for (int i = 0; i < 9; i++) {
        LandmarkInRange yLM;

        qDebug() << "______________________";
        qDebug() << "y range: " << yLandmark_rangeTest[i][0] << ", " << yLandmark_rangeTest[i][1];

        yLM.set(yLandmark_rangeTest[i][0], yLandmark_rangeTest[i][1]);

        yLM.compute_fixedSteps_bigDiv_2_100__smallDiv_5();

        QVector<double> qvectYBigLandMarks = yLM.getBigLandMarksRef();//###LP check that no copy constructor used
        if (!qvectYBigLandMarks.isEmpty()) {

            QVector<QString> qvect_qstrYBigLm = valuesToPowerString_withUniformPower(qvectYBigLandMarks);

            //
            //double powerOfTen_ofBigLandMark_minValue = powerOfTen(qvectYBigLandMarks.first());
            //double powerOfTen_ofBigLandMark_maxValue = powerOfTen(qvectYBigLandMarks.last());
            //
            //for(auto bigLm: qvectYBigLandMarks) {
            //    QString strYBigLm = valueToPoweredString(bigLm,
            //                                             powerOfTen_ofBigLandMark_minValue,
            //                                            powerOfTen_ofBigLandMark_maxValue);
            //    qDebug() << strYBigLm;
            //}
            //
        }
    }


    double xLandmark_rangeTest[60][2]= {
        {     -1,    1   },
        {     -2,    2   },
        {     -3,    3   },
        {     -4,    4   },
        {     -5,    5   },
        {     -6,    6   },
        {     -7,    7   },
        {     -8,    8   },
        {     -9,    9   },
        {    -10,   10   },
        {    -12,   12   },
        {    -15,   15   },
        {    -21,   21   },
        {    -22,   22   },
        {    -50,   50   },
        {    -75,   75   },
        {   -100,  100   },
        {   -102,  102   },
        {   -136,  136   },
        {   -200,  200   },
        {   -236,  236   },
        {   -250,  250   },
        {   -254,  254   },
        {   -255,  255   },
        {  -5000,  5000  },
        {  -7000,  7000  },
        {  -8500,  8500  },
        { -10000, 10000  },

        {     -2,    0   },
        {     -1,    0   },
        {     -3,    1   },
        {     -4,   -2   },
        {     -5,   -1   },
        {     -6,    8   },
        {     -7,    1   },
        {     -8,    1   },
        {     -9,    1   },
        {    -10,   -3   },
        {    -12,    1   },
        {    -15,    5   },
        {    -21,    1   },
        {    -22,    2   },
        {    -50,    0   },
        {    -75,    5   },
        {   -100,   10   },
        {   -102,   12   },
        {   -136,   216  },
        {   -200,   420  },
        {   -236,  -126  },
        {   -250,  -20   },
        {   -254,  -242  },
        {   -255,   25   },
        {  -5000,  500   },
        {  -7000,-5000   },
        {  -8500,    0   },
        { -10000, 8000   },

        { -175,   175   },
        { -319,   319   },
        { -3333,  3333  },
        { -175,   175   }
    };

    for (int i = 0; i < 60; i++) {
        qDebug() << "______________________";
        qDebug() << "x range: " << xLandmark_rangeTest[i][0] << ", " << xLandmark_rangeTest[i][1];
        LandmarkInRange xLM;
        xLM.set(xLandmark_rangeTest[i][0], xLandmark_rangeTest[i][1]);
        xLM.compute_fixedSteps_min0max_with_intBigDiv_2_3_4_5__intSmallDiv_v2();

        QVector<double> qvectXBigLandMarks = xLM.getBigLandMarksRef();//###LP check that no copy constructor used
        if (!qvectXBigLandMarks.isEmpty()) {
            QVector<QString> qvect_qstrXBigLm = valuesToPowerString_withUniformPower(qvectXBigLandMarks);
        }
    }



    /*
    _yLM.set(0.005552, .15383);
    //_yLM.compute(5.0, 5.0,false);
    _yLM.compute_fixedStep();

    _yLM.set(0.5552, 15.383);
    _yLM.compute_fixedStep();
    */

    /*_yLM.set(0, 8);
    _yLM.compute_fixedStep();

    _yLM.set(0, 9);
    _yLM.compute_fixedStep();

    _yLM.set(0, 10);
    _yLM.compute_fixedStep();

    _yLM.set(5, 15);
    _yLM.compute_fixedStep();

    _yLM.set(100, 108);
    _yLM.compute_fixedStep();

    _yLM.set(100, 118);
    _yLM.compute_fixedStep();

    _yLM.set(-8, +4);
    _yLM.compute_fixedStep();
    */


}
