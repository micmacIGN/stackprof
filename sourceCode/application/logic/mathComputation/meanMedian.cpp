
#include <stdlib.h>
#include <gsl/gsl_math.h>

#include <gsl/gsl_fit.h>

#include <gsl/gsl_ieee_utils.h>

#include <gsl/gsl_statistics_double.h>

#include <stdio.h>
#include <iostream>

#include "gsl/gsl_sort.h"

#include <numeric> //for accumulate

#include <vector>
using namespace std;

//#TagToFindAboutReleaseDeliveryOption

//this file have a unit tests which does not used Qt
//Moreover qDebug() considers only print double as float precision. Hence for double precision use cout with std::setprecision instead
//#define DONT_USE_QDEBUG__USE_COUT_INSTEAD

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

#include "meanMedian.h"

//#define DEF_cout_debug
void showVector(const vector<double>& vect, const string& strVectName) {    
#ifdef DEF_cout_debug
    debugOut << __FUNCTION__ << strVectName.c_str() << " =[" << debugEndl;
    if (!vect.size()) {
        debugOut << "]" << debugEndl;
        return;
    }
    auto iterV = vect.begin();
    for (; iterV != vect.end()-1; iterV++) {
        debugOut << *iterV << ", " << debugEndl;
    }
    debugOut << *iterV;
    debugOut << "]"<< debugEndl;
#endif
}


bool noWeight_mean_and_absdevAroundMean(const vector<double>& vectUnsortedValues,
                                        double& mean, double& absdevAroundMean, bool& bComputedValuesValid) {

    debugOut << __FUNCTION__ << debugEndl;
    showVector(vectUnsortedValues, "inputValues=");

    bComputedValuesValid = false;

    if (!vectUnsortedValues.size()) {
        debugOut << __FUNCTION__ << "empty vector!" << debugEndl;
        return(bComputedValuesValid);
    }

    //
    //Mean and Average Absolute Deviation
    //

    //Mean:
    mean = gsl_stats_mean(vectUnsortedValues.data(), 1, vectUnsortedValues.size());
    debugOut << __FUNCTION__ << "mean = " << mean << debugEndl;

    //Average Absolute Deviation
    //https://www.gnu.org/software/gsl/doc/html/statistics.html?#absolute-deviation
    absdevAroundMean = gsl_stats_absdev_m(
                vectUnsortedValues.data(), 1,
                vectUnsortedValues.size(), mean);
    debugOut << __FUNCTION__ << "absdevAroundMean     = " << absdevAroundMean << debugEndl;

    bComputedValuesValid = true;
    return(bComputedValuesValid);
}

#include <iomanip>

//@LP: the values vector is passed by copy to sort it in the function
bool noWeight_median_and_absDevAroundMedian(vector<double> vectUnsortedValues,
                                            double& median, double& absDevAroundMedian, bool& bComputedValuesValid) {


    debugOut << __FUNCTION__ << debugEndl;
    showVector(vectUnsortedValues, "inputValues=");

    bComputedValuesValid = false;
    if (!vectUnsortedValues.size()) {
        debugOut << __FUNCTION__ << "empty vector!" << debugEndl;
        return(false);
    }

    //
    //Median
    //
    //'This function returns the median value of data, a dataset of length n with stride stride.
    //   The median is found using the quickselect algorithm. The input array does not need to be sorted,
    //   but note that the algorithm rearranges the array and so the input is not preserved on output.
    median = gsl_stats_median(vectUnsortedValues.data(), 1, vectUnsortedValues.size());
    //qDebug() << "nWmaaDAM median= " << median;
    //cout << "cout nWmaaDAM median= " << std::setprecision(14) << median << "\n";

    /*{
        //@LP test-dev compare with gsl_stats_median_from_sorted_data:
        vector<double> vectSortedValues = vectUnsortedValues;  //@LP because vectUnsortedValues is sorted after gsl_stats_median above
        double median_from_sorted_data = gsl_stats_median_from_sorted_data(vectSortedValues.data(), 1, vectSortedValues.size());
        cout << "( " << "median_from_sorted_data = " << median_from_sorted_data << " )" << endl;
    }*/



    //
    //MeanAbsDevAroundMedian
    //
    //https://www.gnu.org/software/gsl/doc/html/statistics.html?#absolute-deviation
    absDevAroundMedian = gsl_stats_absdev_m(
                vectUnsortedValues.data(), 1,
                vectUnsortedValues.size(), median);
    //qDebug() << "absDevAroundMedian     = " << absDevAroundMedian;

    //MeanAbsDevAroundMedian computation implementation for comparison:
    //
    //anna maria equivalent: Average of abs Deviation from median:
    /*{
        vector<double> stdvect_x_deltaToMedian_fromImplementationTest = vectUnsortedValues;
        for(auto &iter: stdvect_x_deltaToMedian_fromImplementationTest) {
            iter = fabs(iter - median);
        }
        double sumDistancesToMedian = accumulate(stdvect_x_deltaToMedian_fromImplementationTest.cbegin(), stdvect_x_deltaToMedian_fromImplementationTest.cend(), .0);
        double AverageAbsDevToMedianFromImplementation = sumDistancesToMedian / stdvect_x_deltaToMedian_fromImplementationTest.size();
        //cout << "AverageAbsDevToMedianFromImplementation = " << AverageAbsDevToMedianFromImplementation << "  (computation implementation for comparison)" << endl;
    }*/

    bComputedValuesValid = true;
    return(bComputedValuesValid);
}


bool withWeight_wMean_wAbsdevAroundWMean(
        const vector<double>& vectValues,
        const vector<double>& vectNonNormalizedWeight,
        double powerOfWeight,
        double& wMean, double& wAbsdevAroundMean, bool& bComputedValuesValid) {

    debugOut << __FUNCTION__ << debugEndl;
    showVector(vectValues, "inputValues=");
    showVector(vectNonNormalizedWeight, "vectNonNormalizedWeight=");

    /*showVector(vectValues, "inputValues=");
    showVector(vectNonNormalizedWeight, "coef_correl=");*/

    debugOut << __FUNCTION__ << "powerOfWeight = " << powerOfWeight << debugEndl;

    bComputedValuesValid = false;

    if (vectValues.size() != vectNonNormalizedWeight.size()) {
        debugOut << __FUNCTION__ << "error: data vector and weight vector sizes does not match !" << debugEndl;
        return(false);
    }

    if (!vectValues.size()) {
        debugOut << __FUNCTION__ << "empty vector!" << debugEndl;
        return(false);
    }

    if (powerOfWeight < 1.0) {
        debugOut << __FUNCTION__ << "if (powerOfWeight < 1.0) {" << debugEndl;
        return(false);
    }

    //
    vector<double> stdvect_normalizedWeight = vectNonNormalizedWeight;

    for(auto& wi: stdvect_normalizedWeight) {
        wi = pow(wi, powerOfWeight);
    }

    debugOut << __FUNCTION__ << "after pow with powerOfWeight:" << debugEndl;
    for(auto wi : stdvect_normalizedWeight) {
        debugOut << __FUNCTION__ << "wi: " << wi << debugEndl;
    }


    //Normalize weights for futur usage
    //
    //
    //weight has to be >=0
    //normalize the weights (ie weight sum = 1.0)

    double weightSum = accumulate(stdvect_normalizedWeight.cbegin(), stdvect_normalizedWeight.cend(), .0);
    debugOut << __FUNCTION__ << "weightSum = " << weightSum << debugEndl;
    if (weightSum > 0.001) { //@LP avoiding division by zero; we consider that weights are not so small in every correct use cases
        for(auto& iter:stdvect_normalizedWeight) {
            iter/=weightSum;
        }
    } else {
        debugOut << __FUNCTION__ << "error: weights too small for normalization" << debugEndl;
        return(false);
    }
    weightSum = 1.0;




    //
    double weightNormalizedSum_check = .0;
    //for(auto wi:stdvect_normalizedWeight) { weightNormalizedSum_check+=wi; }
    weightNormalizedSum_check = accumulate(stdvect_normalizedWeight.cbegin(), stdvect_normalizedWeight.cend(), .0);
    /*cout << "weightNormalizedSum_check = " << weightNormalizedSum_check << endl;

    cout << "after normalization:" << endl;
    for(auto& wi:stdvect_normalizedWeight) {
        cout << wi << endl;
    }
    cout << "---" << endl;*/

    debugOut << __FUNCTION__ << "after normalization:" << debugEndl;
    for(auto wi : stdvect_normalizedWeight) {
        debugOut << __FUNCTION__ << "wi: " << wi << debugEndl;
    }

    //
    //Weighted Mean and Weighted Average Absolute Deviation Around Mean
    //

    //
    //Weighted Mean
    //
    wMean   = gsl_stats_wmean(stdvect_normalizedWeight.data(), 1, vectValues.data(), 1, vectValues.size());
    //qDebug() << "wMean = " << wMean;

    //
    //Weighted Average Absolute Deviation Around wMean
    //
    //
    wAbsdevAroundMean = gsl_stats_wabsdev_m(stdvect_normalizedWeight.data(), 1, vectValues.data(), 1, vectValues.size(), wMean);
    //qDebug() << "wAbsdevAroundMean = " << wAbsdevAroundMean;

    //Weighted Average Absolute Deviation Around wMean implementation for comparison:
    //
    //anna maria equivalent: Average of abs Deviation from mean:
    /*{
        vector<double> stdvect_x_deltaToWMean_fromImplementationTest = vectValues;
        int idx = 0;
        for(auto &iter: stdvect_x_deltaToWMean_fromImplementationTest) {
            iter = fabs(iter - wMean) * stdvect_normalizedWeight[idx];
            idx++;
        }
        double sumDistancesToWMean = accumulate(stdvect_x_deltaToWMean_fromImplementationTest.cbegin(), stdvect_x_deltaToWMean_fromImplementationTest.cend(), .0);
        double AverageAbsDevToWMeanFromImplementation = sumDistancesToWMean // / 1.0; //( divide by weights sum)
        cout << "AverageAbsDevToWMeanFromImplementation = " << AverageAbsDevToWMeanFromImplementation << "  (computation implementation for comparison)" << endl;
    }*/

    bComputedValuesValid = true;
    return(bComputedValuesValid);
}



//@LP: vectValue passed by copy: it will be sorted in this method
bool withWeight_wMedian_and_absDevAroundWMedian(
        vector<double> vectValues,
        const vector<double>& vectNonNormalizedWeight,
        double powerOfWeight,
        double& wMedian, double& absDevAroundWMedian, bool& bComputedValuesValid) {

    //
    //Weighted Median   ////////////////and Weighted MAD Median (Weighted Median Absolute Deviation aroun Median)
    //

    //Weighted median:

    debugOut << __FUNCTION__ << debugEndl;
    showVector(vectValues, " inputValues=");
    showVector(vectNonNormalizedWeight, " vectNonNormalizedWeight=");
    debugOut << " powerOfWeight = " << powerOfWeight << debugEndl;


    int vectValues_size = vectValues.size();

    /*showVector(vectValues, "inputValues=");
    showVector(vectNonNormalizedWeight, "coef_correl=");*/
    //cout << "powerOfWeight = " << powerOfWeight;

    bComputedValuesValid = false;

    if (!vectValues_size) {
        debugOut << " empty vector!" << debugEndl;
        return(false);
    }

    if (vectValues_size != vectNonNormalizedWeight.size()) {
        debugOut << " error: data vector and weight vector sizes does not match !" << debugEndl;
        return(false);
    }

    vector<double> stdvect_normalizedWeight = vectNonNormalizedWeight;

    for(auto& wi: stdvect_normalizedWeight) {
        wi = pow(wi, powerOfWeight);
    }

    debugOut << __FUNCTION__ << " after pow with powerOfWeight:" << debugEndl;
    for(auto wi : stdvect_normalizedWeight) {
        debugOut << __FUNCTION__ << " wi: " << wi << debugEndl;
    }

    /*cout << "after pow with powerOfWeight:" << endl;
    for(auto wi : stdvect_normalizedWeight) {
        cout << wi << endl;
    }
    cout << "---" << endl;*/

    //
    //
    //Normalize weights for futur usage
    //
    //
    //weight has to be >=0
    //normalize the weights (ie weight sum = 1.0)
    /*qDebug() << "before normalization:" <<
    for(auto& iter:stdvect_normalizedWeight) {
        qDebug() << iter << ", ";
    }
    qDebug() << */
    /*for(auto& iter:stdvect_normalizedWeight) {
        weightSum+=iter;
    }*/


    double weightSum = accumulate(stdvect_normalizedWeight.cbegin(), stdvect_normalizedWeight.cend(), .0);
    debugOut << __FUNCTION__ << " weightSum = " << weightSum << debugEndl;
    if (weightSum > 0.001) { //@LP avoiding division by zero; we consider that weights are not so small in every correct use cases
        for(auto& iter:stdvect_normalizedWeight) {
            iter/=weightSum;
        }
    } else {
        debugOut << __FUNCTION__ << " error: weights too small for normalization" << debugEndl;
        return(false);
    }
    weightSum = 1.0;


    //
    /*double weightNormalizedSum_check = .0;
    for(auto& iter:stdvect_normalizedWeight) {
        weightNormalizedSum_check+=iter;
    }*/
    //weightNormalizedSum_check = accumulate(stdvect_normalizedWeight.cbegin(), stdvect_normalizedWeight.cend(), .0);
    //qDebug() << "weightNormalizedSum_check = " << weightNormalizedSum_check <<
    /*
    qDebug() << "after normalization:" <<
    for(auto& iter:stdvect_normalizedWeight) {
        qDebug() << iter << ", ";
    }qDebug() <<
    */

    //
    //Weighted Median and Weighted MAD Median (Weighted Median Absolute Deviation aroun Median)
    //

    //Weighted median:

    //sort data in ascending order and sync the weights vector
    //vector<double> stdvect_x_sorted = stdvect_unsortedx;
    vector<double> stdvect_normalizedWeight_syncToSortedx = stdvect_normalizedWeight;

    /*cout << "before sort:" << endl;
    for (int i = 0; i < stdvect_normalizedWeight_syncToSortedx.size(); i++) {
        cout << vectValues[i] << ":: w=" << stdvect_normalizedWeight_syncToSortedx[i] << endl;
    }*/

    //'This function sorts the n elements of the array data1 with stride stride1 into ascending numerical order,
    // while making the same rearrangement of the array data2 with stride stride2, also of size n.'
    gsl_sort2(vectValues.data(), 1, stdvect_normalizedWeight_syncToSortedx.data(), 1, vectValues.size());

    debugOut << __FUNCTION__ << " stdvect_normalizedWeight_syncToSortedx (after sort:)" << debugEndl;
    for(int i = 0; i < vectValues.size(); i++) {
        debugOut << __FUNCTION__ << " " << vectValues[i] << " :: w= " << stdvect_normalizedWeight_syncToSortedx[i] << debugEndl;
    }

    if (vectValues_size == 1) {
        debugOut << " if (vectValues_size == 1) { return now with valid values" << debugEndl;
        wMedian = vectValues[0]; //@#LP
        absDevAroundWMedian = 0; //@#LP
        bComputedValuesValid = true;
        return(true);
    }

    double weightedMedian = .0;
    int kAboutLowerMedian = -1;
    //version 2 (stack overflow)
    {
        //lower median

        //double weightSum = std::accumulate(stdvect_normalizedWeight_syncToSortedx.cbegin(), stdvect_normalizedWeight_syncToSortedx.cend(), .0);
        unsigned int k = 0;
        double sum = weightSum - stdvect_normalizedWeight_syncToSortedx[0]; // sum is the total weight of all `x[i] > x[k]`

        //cout << "init sum = " << sum << endl;
        //cout << "weightSum/2.0 = " << weightSum/2.0 << endl;

        while(sum > weightSum/2.0) {

            //cout << "sum = " << sum << " > weightSum/2.0" << endl;

            k++;

            sum -= stdvect_normalizedWeight_syncToSortedx[k];

            //cout << "sum - stdvect_normalizedWeight_syncToSortedx[ " << k << "] = " << sum << endl;

        }
        weightedMedian = vectValues[k];
        debugOut << __FUNCTION__ << "weighted median: " << weightedMedian << endl;
        debugOut << __FUNCTION__ << " (k AboutLowerMedian = " << k << ")" << endl;

        kAboutLowerMedian = k;

        //@LP checking, showing the sum on the two sides
        /*double weightSumLeftSide  = .0;
        for (int ik = 0; ik < k; ik++) {
            weightSumLeftSide += stdvect_normalizedWeight_syncToSortedx[ik];
        }
        double weightSumRightSide = .0;
        for (int ik = k+1; ik < stdvect_normalizedWeight_syncToSortedx.size(); ik++) {
            weightSumRightSide+=stdvect_normalizedWeight_syncToSortedx[ik];
        }
        debugOut << __FUNCTION__ << " sum Left, right around pivot: " << weightSumLeftSide << ", " << weightSumRightSide << debugEndl;
        */

        //cout << "sum Left, right around pivot: " << weightSumLeftSide << ", " << weightSumRightSide << endl;
    }

    wMedian = weightedMedian;




    //upper weighted median: adapted from the way to find the lower weighted median
    double upperWeightedMedian  = .0;
    int kAboutUpperMedian = -1;
    {
        //upper median

        //double weightSum = std::accumulate(stdvect_normalizedWeight_syncToSortedx.cbegin(), stdvect_normalizedWeight_syncToSortedx.cend(), .0);
        unsigned int k = stdvect_normalizedWeight_syncToSortedx.size() - 1;
        double sum = weightSum - stdvect_normalizedWeight_syncToSortedx[k]; // sum is the total weight of all `x[i] < x[k]`

        //cout << "init sum = " << sum << endl;
        //cout << "weightSum/2.0 = " << weightSum/2.0 << endl;

        while(sum > weightSum/2.0) {

            //cout << "sum = " << sum << " > weightSum/2.0" << endl;

            k--;

            sum -= stdvect_normalizedWeight_syncToSortedx[k];

            //cout << "sum - stdvect_normalizedWeight_syncToSortedx[ " << k << "] = " << sum << endl;

        }
        upperWeightedMedian = vectValues[k];
        debugOut << __FUNCTION__ << "UpperWeightedMedian: " << upperWeightedMedian << endl;
        debugOut << __FUNCTION__ << " (k AboutUpperWeightedMedian = " << k << ")" << endl;

        kAboutUpperMedian = k;

        //@LP checking, showing the sum on the two sides
        /*double weightSumLeftSide  = .0;
        for (int ik = 0; ik < k; ik++) {
            weightSumLeftSide += stdvect_normalizedWeight_syncToSortedx[ik];
        }
        double weightSumRightSide = .0;
        for (int ik = k+1; ik < stdvect_normalizedWeight_syncToSortedx.size(); ik++) {
            weightSumRightSide+=stdvect_normalizedWeight_syncToSortedx[ik];
        }
        debugOut << __FUNCTION__ << " (UpperWeightedMedian) sum Left, right around pivot: " << weightSumLeftSide << ", " << weightSumRightSide << debugEndl;
        */
    }

    if  (kAboutLowerMedian == kAboutUpperMedian) {
        //cout << "kAboutLowerMedian and kAboutUpperMedian are the same : " << kAboutLowerMedian << endl;
    } else {
        //if  (kAboutLowerMedian != kAboutUpperMedian) {
        int deltaIndexLowerUpperMedian = abs(kAboutLowerMedian - kAboutUpperMedian);
        debugOut << __FUNCTION__ << " deltaIndexLowerUpperMedian = " << deltaIndexLowerUpperMedian << debugEndl;
        debugOut << __FUNCTION__ << " kAboutLowerMedian = " << kAboutLowerMedian << debugEndl;
        debugOut << __FUNCTION__ << " kAboutUpperMedian = " << kAboutUpperMedian << debugEndl;
        if (deltaIndexLowerUpperMedian != 1) {
            debugOut << __FUNCTION__ << " if (deltaIndexLowerUpperMedian != 1) { return false now" << debugEndl;
            debugOut << __FUNCTION__ << " dev error: deltaIndexLowerUpperMedian != 1 ; is :" << deltaIndexLowerUpperMedian << debugEndl;
            return(false);
        }

        wMedian = (weightedMedian + upperWeightedMedian) / 2.0;
        //cout << "wMedian set to mean (lower+upper)/2.0 = " << wMedian << endl;
    }

    //Anna Maria equ to (should be (see FDSC python source code)): Average of Abs Deviation from weighted median:
    {
        vector<double> stdvect_sorted_absDelta_XToWeightedMedian = vectValues;
        int idx = 0;
        for(auto& vi: stdvect_sorted_absDelta_XToWeightedMedian) {
            vi = fabs(vi - weightedMedian) * stdvect_normalizedWeight_syncToSortedx[idx];
            idx++;
        }
        double sumDistancesToMedian = accumulate(stdvect_sorted_absDelta_XToWeightedMedian.cbegin(), stdvect_sorted_absDelta_XToWeightedMedian.cend(), .0);
        //@LP weights are normalized, hence divider is 1.0
        double averageOfAbsDeviationAroundWeightedMedian = sumDistancesToMedian; //  / weightSum;
        absDevAroundWMedian = averageOfAbsDeviationAroundWeightedMedian;
    }

    bComputedValuesValid = true;

    debugOut << __FUNCTION__ << " ending with bComputedValuesValid = " << bComputedValuesValid << debugEndl;

    return(bComputedValuesValid);
}
