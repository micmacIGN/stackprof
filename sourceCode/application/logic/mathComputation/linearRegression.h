#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include <vector>
using namespace std;

#include <QJsonObject>

struct S_MathComp_GSLComputedLinearRegressionParameters {

    bool _bComputed;

    //y = c1 * x + c0
    //y = slope * x + intercept
    //y = a * x + b
    double _c0_b_intercept;
    double _c1_a_slope;

    double _cov00;
    double _cov01;
    double _cov11;
    double _sumsq;

    S_MathComp_GSLComputedLinearRegressionParameters();
    void clear();
    void showContent() const;

};

struct S_MathComp_LinearRegression_modelParametersErrors {
   double _stdErrorOfTheRegression;
   double _stdErrorOfTheSlope;
   double _stdErrorOfIntercept;

   bool _b_stdErrorOfTheRegression_computed;
   bool _b_stdErrorsOfSlopeAndIntercept_computed;

   S_MathComp_LinearRegression_modelParametersErrors();
   void clear();
   void showContent() const;
};

struct S_MathComp_LinearRegressionParameters {
    S_MathComp_GSLComputedLinearRegressionParameters _GSLCompLinRegParameters;
    S_MathComp_LinearRegression_modelParametersErrors _modelParametersErrors;

    S_MathComp_LinearRegressionParameters();
    void clear();
    void showContent() const;
    bool linearRegrModel_toQJsonObject(QJsonObject& qjsonObj) const;
    bool linearRegrModel_toASciiStringlist(bool bSetLinearRegressionData_asEmpty,
                                           QStringList& qstrList_sideOfLinearRegressionModel) const;

};

bool computeLinearRegressionParametersFor(const vector<double>& vectX, const vector<double>& vectY,
                                          S_MathComp_LinearRegressionParameters &linearRegressionParameters);

//use all GSL math parameters
bool computeYForGivenX_usingComputedLinearRegressionParameters_GSLMethod(
        const S_MathComp_GSLComputedLinearRegressionParameters &GSLComputedLinearRegressionParameters,
        double x, double& computedY);

//use simpy y = a*x+b
//y = (slope * x) + intercept
bool computeYForGivenX_usingComputedLinearRegressionParameters_slopeInterceptMethod(
        const S_MathComp_GSLComputedLinearRegressionParameters &GSLComputedLinearRegressionParameters,
        double x, double& computedY);

//use simpy y = a*x+b
//------ compute here two points per computed line to display the computed lines in the qt coordinate system (aka qcs)
//line equation considered applicable; filtering of invalid line (typically vertical line) has to be made before calling this method
// y = slope*x + intercept
//
// slope*x = y - intercept
// x = (y - intercept) / slope;

bool computeXForGivenY_usingComputedLinearRegressionParameters_slopeInterceptMethod(
        const S_MathComp_GSLComputedLinearRegressionParameters &GSLComputedLinearRegressionParameters,
        double y, double& computedX);

#endif
