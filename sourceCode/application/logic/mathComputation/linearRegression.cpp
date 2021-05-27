#include <QDebug>

#include <stdlib.h>
#include <gsl/gsl_math.h>

#include <gsl/gsl_fit.h>

#include <gsl/gsl_ieee_utils.h>

#include <gsl/gsl_statistics_double.h>

#include <string.h>
#include <stdio.h>

#include <iostream>
using namespace std;

#include "linearRegression.h"
#include "../toolbox/toolbox_math.h"

#include "../model/core/exportResult.h"

#include "../toolbox/toolbox_conversion.h"

S_MathComp_GSLComputedLinearRegressionParameters::S_MathComp_GSLComputedLinearRegressionParameters() {
    clear();
}

void S_MathComp_GSLComputedLinearRegressionParameters::clear() {
    _bComputed = false;

    _c0_b_intercept = .0;
    _c1_a_slope = .0;

    _cov00 = .0;
    _cov01 = .0;
    _cov11 = .0;
    _sumsq = .0;
}

S_MathComp_LinearRegression_modelParametersErrors::S_MathComp_LinearRegression_modelParametersErrors() {
    clear();
}

void S_MathComp_LinearRegression_modelParametersErrors::clear() {

    _b_stdErrorOfTheRegression_computed = false;
    _b_stdErrorsOfSlopeAndIntercept_computed = false;

    _stdErrorOfTheRegression = .0;
    _stdErrorOfTheSlope = .0;
    _stdErrorOfIntercept = .0;
}

S_MathComp_LinearRegressionParameters::S_MathComp_LinearRegressionParameters() {
    clear();
}

void S_MathComp_LinearRegression_modelParametersErrors::showContent() const {
    qDebug() << __FUNCTION__ << "(S_MathComp_LinearRegressionParameters)";
    qDebug() << __FUNCTION__ << "  _b_stdErrorOfTheRegression_computed : "      << _b_stdErrorOfTheRegression_computed;
    qDebug() << __FUNCTION__ << "  _b_stdErrorsOfSlopeAndIntercept_computed : " << _b_stdErrorsOfSlopeAndIntercept_computed;
    qDebug() << __FUNCTION__ << "  _stdErrorOfTheRegression : " <<  _stdErrorOfTheRegression;
    qDebug() << __FUNCTION__ << "  _stdErrorOfTheSlope : "      << _stdErrorOfTheSlope;
    qDebug() << __FUNCTION__ << "  _stdErrorOfIntercept : "     << _stdErrorOfIntercept;
}

void S_MathComp_GSLComputedLinearRegressionParameters::showContent() const {
    qDebug() << __FUNCTION__ << "(S_MathComp_GSLComputedLinearRegressionParameters)";

    qDebug() << __FUNCTION__ << "  _bComputed : " << _bComputed;

    qDebug() << __FUNCTION__ << "  _c0_b_intercept: " << _c0_b_intercept;
    qDebug() << __FUNCTION__ << "  _c1_a_slope: " << _c1_a_slope;

    qDebug() << __FUNCTION__ << "  _cov00: " << _cov00;
    qDebug() << __FUNCTION__ << "  _cov01: " << _cov01;
    qDebug() << __FUNCTION__ << "  _cov11: " << _cov11;
    qDebug() << __FUNCTION__ << "  _sumsq: " << _sumsq;
}


void S_MathComp_LinearRegressionParameters::clear() {
    _GSLCompLinRegParameters.clear();
    _modelParametersErrors.clear();
}

void S_MathComp_LinearRegressionParameters::showContent() const {
    _GSLCompLinRegParameters.showContent();
    _modelParametersErrors.showContent();
}

bool S_MathComp_LinearRegressionParameters::linearRegrModel_toQJsonObject(QJsonObject& qjsonObj) const {

    if (!_GSLCompLinRegParameters._bComputed) {
        qjsonObj.insert("bComputed", false);
        qDebug() << __FUNCTION__ << " _GSLCompLinRegParameters._bComputed: " << _GSLCompLinRegParameters._bComputed;
        return(false);
    }
    qjsonObj.insert("bComputed", true);

    //y = a * x + b
    qjsonObj.insert("slope"    , _GSLCompLinRegParameters._c1_a_slope); //@#LP add fixedprecision
    qjsonObj.insert("intercept", _GSLCompLinRegParameters._c0_b_intercept); //@#LP add fixedprecision

    qjsonObj.insert("sumsq"    , _GSLCompLinRegParameters._sumsq); //@#LP add fixedprecision

    if (_modelParametersErrors._b_stdErrorsOfSlopeAndIntercept_computed) {
        qjsonObj.insert("stdErrorOfSlope"    , _modelParametersErrors._stdErrorOfTheSlope); //@#LP add fixedprecision
        qjsonObj.insert("stdErrorOfIntercept", _modelParametersErrors._stdErrorOfIntercept); //@#LP add fixedprecision
    } else {
        qjsonObj.insert("stdErrorOfSlope"    , "notComputed");
        qjsonObj.insert("stdErrorOfIntercept", "notComputed");
    }

    if (_modelParametersErrors._b_stdErrorOfTheRegression_computed) {
        qjsonObj.insert("stdErrorOfTheRegression", _modelParametersErrors._stdErrorOfTheRegression); //@#LP add fixedprecision
    } else {
        qjsonObj.insert("stdErrorOfTheRegression", "notComputed");
    }

    return(true);
}

bool S_MathComp_LinearRegressionParameters::linearRegrModel_toASciiStringlist(
        bool bSetLinearRegressionData_asEmpty,
        QStringList& qstrList_sideOfLinearRegressionModel) const {

    if (bSetLinearRegressionData_asEmpty) {
        qstrList_sideOfLinearRegressionModel.push_back("");//_GSLCompLinRegParameters._bComputed as empty
        for(int i=0; i < 6; i++) {
            qstrList_sideOfLinearRegressionModel.push_back(""); //empty fields from leftSide_a_slope to leftSide_sumsq
        }
        return(true);
    }

    qstrList_sideOfLinearRegressionModel.push_back(boolToAsciiBoolString(_GSLCompLinRegParameters._bComputed));
    if (!_GSLCompLinRegParameters._bComputed) {
        for(int i=0; i < 6; i++) {
            qstrList_sideOfLinearRegressionModel.push_back(""); //empty fields from leftSide_a_slope to leftSide_sumsq
        }
        return(true);
    }

    //y = a * x + b
    qstrList_sideOfLinearRegressionModel.push_back(doubleToQStringPrecision_f_amountOfDecimal(_GSLCompLinRegParameters._c1_a_slope, 14));
    qstrList_sideOfLinearRegressionModel.push_back(doubleToQStringPrecision_f_amountOfDecimal(_GSLCompLinRegParameters._c0_b_intercept, 14));

    if (_modelParametersErrors._b_stdErrorsOfSlopeAndIntercept_computed) {
        qstrList_sideOfLinearRegressionModel.push_back(doubleToQStringPrecision_f_amountOfDecimal(_modelParametersErrors._stdErrorOfTheSlope, 14));
        qstrList_sideOfLinearRegressionModel.push_back(doubleToQStringPrecision_f_amountOfDecimal(_modelParametersErrors._stdErrorOfIntercept, 14));
    } else {
        qstrList_sideOfLinearRegressionModel.push_back("");
        qstrList_sideOfLinearRegressionModel.push_back("");
    }

    if (_modelParametersErrors._b_stdErrorOfTheRegression_computed) {
        qstrList_sideOfLinearRegressionModel.push_back(doubleToQStringPrecision_f_amountOfDecimal(_modelParametersErrors._stdErrorOfTheRegression, 14));
    } else {
        qstrList_sideOfLinearRegressionModel.push_back("");
    }

    qstrList_sideOfLinearRegressionModel.push_back(doubleToQStringPrecision_f_amountOfDecimal(_GSLCompLinRegParameters._sumsq, 14));

    return(true);
}


bool computeLinearRegressionParametersFor(const vector<double>& vectX,
                                          const vector<double>& vectY,
                                          S_MathComp_LinearRegressionParameters &linearRegressionParameters) {


    unsigned long int vectXsize = vectX.size();
    unsigned long int vectYsize = vectY.size();

    linearRegressionParameters.clear();

    if (vectXsize != vectYsize) {
        qDebug() << __FUNCTION__ << "error: if (vectXsize != vectYsize) {";
        return(false);
    }

    if (vectXsize < 2) {
        qDebug() << __FUNCTION__ << "warning: not enough point to compute any line equation: ( " << vectXsize << ")";
        return(false);
    }

    /*fprintf(stdout, "%s: vect:\n", __FUNCTION__);
    for (unsigned long int i = 0; i < vectXsize; i++) {
        fprintf(stdout, "vectX[%ld] = %f\n", i, vectX[i]);
    }*/

    gsl_fit_linear (vectX.data(), 1,
                    vectY.data(), 1,
                    vectX.size(),
                    &linearRegressionParameters._GSLCompLinRegParameters._c0_b_intercept,
                    &linearRegressionParameters._GSLCompLinRegParameters._c1_a_slope,
                    &linearRegressionParameters._GSLCompLinRegParameters._cov00,
                    &linearRegressionParameters._GSLCompLinRegParameters._cov01,
                    &linearRegressionParameters._GSLCompLinRegParameters._cov11,
                    &linearRegressionParameters._GSLCompLinRegParameters._sumsq);

    linearRegressionParameters._GSLCompLinRegParameters._bComputed = true;

    qDebug() << __FUNCTION__ << "y = a*x + b <=>" <<
            linearRegressionParameters._GSLCompLinRegParameters._c1_a_slope << " * x + " <<
            linearRegressionParameters._GSLCompLinRegParameters._c0_b_intercept;


    //https://people.duke.edu/~rnau/mathreg.htm :
    //'The standard error of the model (denoted again by s) is usually referred to as the standard error of the regression
    //(or sometimes the "standard error of the estimate") in this context, and it is equal to the square root of
    //{the sum of squared errors divided by n-2}, or equivalently, the standard deviation of the errors multiplied by
    //the square root of (n-1)/(n-2), where the latter factor is a number slightly larger than 1:'

    if (vectXsize < 3) {

        qDebug() << __FUNCTION__ << "warning: Size of data avectors to small to 'compute' errors of the regression";

        linearRegressionParameters._modelParametersErrors._b_stdErrorOfTheRegression_computed      = false;
        linearRegressionParameters._modelParametersErrors._b_stdErrorsOfSlopeAndIntercept_computed = false;

        linearRegressionParameters._modelParametersErrors._stdErrorOfTheRegression = .0;
        linearRegressionParameters._modelParametersErrors._stdErrorOfTheSlope = .0;
        linearRegressionParameters._modelParametersErrors._stdErrorOfIntercept = .0;

        return(true);
    }

    //qDebug() << __FUNCTION__ << "_ fprintf formated .14f:";

    double sumsq = linearRegressionParameters._GSLCompLinRegParameters._sumsq;
    double stdErrorOfTheRegression =  sqrt( (1.0/(vectX.size()-2)) * sumsq);

    linearRegressionParameters._modelParametersErrors._stdErrorOfTheRegression = stdErrorOfTheRegression;
    linearRegressionParameters._modelParametersErrors._b_stdErrorOfTheRegression_computed = true;

    //fprintf(stdout, "  stdErrorOfTheRegression:%.14f\n", linearRegressionParameters._modelParametersErrors._stdErrorOfTheRegression);

    //'Each of the two model parameters, the slope and intercept, has its own standard error, which is the
    //estimated standard deviation of the error in estimating it. (In general, the term "standard error"
    //means "standard deviation of the error" in whatever is being estimated.)'
    double xMean     = gsl_stats_mean    (vectX.data(), 1, vectX.size());
    double xVariance = gsl_stats_variance(vectX.data(), 1, vectX.size());

    //fprintf(stdout, "  xMean = %.14f\n", xMean);
    //fprintf(stdout, "  xVariance = %.14f\n", xVariance);

    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(xVariance)) {

        qDebug() << __FUNCTION__ << "warning: xVariance is tool small!";

        linearRegressionParameters._modelParametersErrors._b_stdErrorsOfSlopeAndIntercept_computed = false;
        linearRegressionParameters._modelParametersErrors._stdErrorOfTheSlope = .0;
        linearRegressionParameters._modelParametersErrors._stdErrorOfIntercept = .0;

    } else { //if (xVariance > 0.00000000000001) { //@LP avoid divide by zero or over range value

      double stdErrorOfIntercept = (stdErrorOfTheRegression / sqrt(vectX.size())) * sqrt ( 1 + ( (xMean*xMean) / xVariance ));

      double xStdDev = sqrt(xVariance);
      double stdErrorOfTheSlope = (stdErrorOfTheRegression / sqrt(vectX.size())) * (1.0/xStdDev);

      //fprintf(stdout, "  stdErrorOfTheRegression = %.14f\n", stdErrorOfTheRegression);
      //fprintf(stdout, "  stdErrorOfTheSlope (a)  = %.14f\n", stdErrorOfTheSlope);
      //fprintf(stdout, "  stdErrorOfIntercept (b) = %.14f\n", stdErrorOfIntercept);

      linearRegressionParameters._modelParametersErrors._b_stdErrorsOfSlopeAndIntercept_computed = true;
      linearRegressionParameters._modelParametersErrors._stdErrorOfTheSlope  = stdErrorOfTheSlope;
      linearRegressionParameters._modelParametersErrors._stdErrorOfIntercept = stdErrorOfIntercept;
    }

    return(true);
}

//use all GSL math parameters
bool computeYForGivenX_usingComputedLinearRegressionParameters_GSLMethod(
        const S_MathComp_GSLComputedLinearRegressionParameters &GSLComputedLinearRegressionParameters,
        double x, double& computedY) {

    if (!GSLComputedLinearRegressionParameters._bComputed) {
        qDebug() << __FUNCTION__ << "if (!GSLComputedLinearRegressionParameters._bComputed) {";
        return(false);
    }

    /*
    //https://www.gnu.org/software/gsl/doc/html/lls.html?highlight=gsl_fit_linear_est#c.gsl_fit_linear_est

    //'This function uses the best-fit linear regression coefficients c0, c1 and
    //their covariance cov00, cov01, cov11 to compute the fitted function y and
    //its standard deviation y_err for the model Y = c_0 + c_1 X at the point x.'

    int gsl_fit_linear_est(double x, double c0, double c1, double cov00, double cov01, double cov11, double * y, double * y_err)

    @LP: looking at the source code of this method, it always returns GSL_SUCCESS
    */

    double yErr = 0.0;
    gsl_fit_linear_est(x,
                       GSLComputedLinearRegressionParameters._c0_b_intercept,
                       GSLComputedLinearRegressionParameters._c1_a_slope,
                       GSLComputedLinearRegressionParameters._cov00,
                       GSLComputedLinearRegressionParameters._cov01,
                       GSLComputedLinearRegressionParameters._cov11,
                       &computedY,
                       &yErr);

    //fprintf(stdout, "%s: yComputed for x= %f : %f\n", __FUNCTION__, x, computedY );
    //fprintf(stdout, "%s: yErr = %f\n", __FUNCTION__, yErr);

    return(true);
}

//use simpy y = a*x+b
//y = (slope * x) + intercept
bool computeYForGivenX_usingComputedLinearRegressionParameters_slopeInterceptMethod(
        const S_MathComp_GSLComputedLinearRegressionParameters &GSLComputedLinearRegressionParameters,
        double x, double& computedY) {

    if (!GSLComputedLinearRegressionParameters._bComputed) {
        qDebug() << __FUNCTION__ << "if (!GSLComputedLinearRegressionParameters._bComputed) {";
        return(false);
    }

    computedY = (GSLComputedLinearRegressionParameters._c1_a_slope * x) + GSLComputedLinearRegressionParameters._c0_b_intercept;
    return(true);
}

bool computeXForGivenY_usingComputedLinearRegressionParameters_slopeInterceptMethod(
        const S_MathComp_GSLComputedLinearRegressionParameters &GSLComputedLinearRegressionParameters,
        double y, double& computedX) {

    if (!GSLComputedLinearRegressionParameters._bComputed) {
        qDebug() << __FUNCTION__ << "if (!GSLComputedLinearRegressionParameters._bComputed) {";
        return(false);
    }

    if (valueIsCloseToZero_deltaLowerThan1ExpMinus14(GSLComputedLinearRegressionParameters._c1_a_slope)) {
        qDebug() << __FUNCTION__ << "warning: _c1_a_slope is very close to zero; the line is considered horizontal";
        computedX = GSLComputedLinearRegressionParameters._c0_b_intercept;
        return(true);
    }

    computedX = (y - GSLComputedLinearRegressionParameters._c0_b_intercept) / GSLComputedLinearRegressionParameters._c1_a_slope;

    return(true);
}

