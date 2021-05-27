
#include <QVector>
#include "../application/logic/model/core/ComputationCore_structures.h"

#include "../application/logic/mathComputation/initGSL.h"

int main(int argc, char *argv[]) {

    //---------------------------
    initGSL();
    //---------------------------

    bool bReport = false;

    S_MathComp_LinearRegressionParameters QMCLIRP_result_L;
    bReport = computeLinearRegressionParametersFor(
        {-2, -1,0},
        {.0, 1.5, 3.0},
        QMCLIRP_result_L);
    QMCLIRP_result_L.showContent();

    S_MathComp_LinearRegressionParameters QMCLIRP_result_R;
    bReport = computeLinearRegressionParametersFor(
        {0, 1, 2},
        {3.0, 4.5, 6.0},
        QMCLIRP_result_R);
    QMCLIRP_result_R.showContent();


    //use all GSL math parameters
    double computedY_forx2 = .0;
    bReport = computeYForGivenX_usingComputedLinearRegressionParameters_GSLMethod(
                QMCLIRP_result_L._GSLCompLinRegParameters, 2.0, computedY_forx2);

    //use simpy y = a*x+b
    //y = (slope * x) + intercept
    bReport = computeYForGivenX_usingComputedLinearRegressionParameters_slopeInterceptMethod(
            QMCLIRP_result_L._GSLCompLinRegParameters, 2.0, computedY_forx2);

    //use simpy y = a*x+b
    //------ compute here two points per computed line to display the computed lines in the qt coordinate system (aka qcs)
    //line equation considered applicable; filtering of invalid line (typically vertical line) has to be made before calling this method
    // y = slope*x + intercept
    //
    // slope*x = y - intercept
    // x = (y - intercept) / slope;
    bReport = computeXForGivenY_usingComputedLinearRegressionParameters_slopeInterceptMethod(
            QMCLIRP_result_L._GSLCompLinRegParameters, 2.0, computedY_forx2);


    //---------------------------------------------------------------
    S_MathComp_LinearRegressionParameters QMCLIRP_result_LWithNoise;
    bReport = computeLinearRegressionParametersFor(
        {-2,-1,   -0.5,   0},
        {.0, 1.5,  2.0,   3.0},
        QMCLIRP_result_L);
    QMCLIRP_result_L.showContent();

    S_MathComp_LinearRegressionParameters QMCLIRP_result_RWithNoise;
    bReport = computeLinearRegressionParametersFor(
        {  0,   1,   1.5,  2},
        {3.0, 4.5,   5.0,  6.0},
        QMCLIRP_result_R);
    QMCLIRP_result_R.showContent();


    //use all GSL math parameters
    double computedY_forx2WithNoise = .0;
    bReport = computeYForGivenX_usingComputedLinearRegressionParameters_GSLMethod(
                QMCLIRP_result_LWithNoise._GSLCompLinRegParameters, 2.0, computedY_forx2WithNoise);

    //use simpy y = a*x+b
    //y = (slope * x) + intercept
    bReport = computeYForGivenX_usingComputedLinearRegressionParameters_slopeInterceptMethod(
            QMCLIRP_result_LWithNoise._GSLCompLinRegParameters, 2.0, computedY_forx2WithNoise);

    //use simpy y = a*x+b
    //------ compute here two points per computed line to display the computed lines in the qt coordinate system (aka qcs)
    //line equation considered applicable; filtering of invalid line (typically vertical line) has to be made before calling this method
    // y = slope*x + intercept
    //
    // slope*x = y - intercept
    // x = (y - intercept) / slope;
    bReport = computeXForGivenY_usingComputedLinearRegressionParameters_slopeInterceptMethod(
            QMCLIRP_result_LWithNoise._GSLCompLinRegParameters, 2.0, computedY_forx2WithNoise);

    return(0);

}
