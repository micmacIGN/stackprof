#include <stdlib.h>

#include <stdio.h>
#include <iostream>

#include <vector>
using namespace std;

#include "initGSL.h"
#include "meanMedian.h"

//mean
//weighted mean

//median
//weigthed median

//takes into account invalid value in dataset

#include <stdlib.h>

struct S_MeanResult {
    double _mean;
    double _absdevAroundMean;
    bool _bComputedValuesValid;
    void clear();
    void show();
};

void S_MeanResult::clear() {
    _mean = .0;
    _absdevAroundMean = .0;
    _bComputedValuesValid = false;
}

void S_MeanResult::show() {
    if (!_bComputedValuesValid) {
        cout << "computed values not valid" << endl << endl;
    } else {
        cout << "=> mean     = " << _mean << endl;
        cout << "=> sigma    = " << _absdevAroundMean << endl;
    }
}

struct S_MedianResult {
    double _median;
    double _absDevAroundMedian;
    bool _bComputedValuesValid;
    void clear();
    void show();
};

void S_MedianResult::clear() {
    _median = .0;
    _absDevAroundMedian = .0;
    _bComputedValuesValid = false;
}

void S_MedianResult::show() {
    if (!_bComputedValuesValid) {
        cout << "computed values not valid" << endl << endl;
    } else {
        cout <<"=> median      = " << _median << endl;
        cout <<"=> sigma       = " << _absDevAroundMedian << endl;
    }
}

struct S_WMeanResult {
    double _wMean;
    double _wAbsdevAroundMean;
    bool _bComputedValuesValid;
    void clear();
    void show();
};

void S_WMeanResult::clear() {
    _wMean = .0;
    _wAbsdevAroundMean = .0;
    _bComputedValuesValid = false;
}

void S_WMeanResult::show() {
    if (!_bComputedValuesValid) {
        cout << "computed values not valid" << endl << endl;
    } else {
        cout <<"=> mean_pond = " << _wMean << endl;
        cout <<"=> sigma     = " << _wAbsdevAroundMean << endl;
    }
}

struct S_WMedianResult {
    double _wMedian;
    double _absDevAroundWMedian;
    bool _bComputedValuesValid;
    void clear();
    void show();
};

void S_WMedianResult::clear() {
    _wMedian = .0;
    _absDevAroundWMedian = .0;
    _bComputedValuesValid = false;
}

void S_WMedianResult::show() {
    if (!_bComputedValuesValid) {
        cout << "computed values not valid" << endl << endl;
    } else {
        cout <<"=> median_pond = " << _wMedian << endl;
        cout <<"=> sigma       = " << _absDevAroundWMedian << endl;
    }
}


int main(void) {

    initGSL();

    bool bTestNoWeight = true;
    bool bTestWithWeight = true;

    if (bTestNoWeight) {

        cout << "-- Tests No Weight (cpp):" << endl;

        S_MeanResult meanResult;
        S_MedianResult medianResult;

        //no weights:
        //Unsorted data before call:
        {
            vector<double> unsortedx1 = {3.0,  1.0, 5.0, 2.0, 4.0};
            cout << "--No Weight-------- unsortedx1:" << endl;

            //showVector(unsortedx1, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            meanResult.clear();
            medianResult.clear();
            noWeight_mean_and_absdevAroundMean(unsortedx1, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx1, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx1, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();


        }

        {
            vector<double> unsortedx2 = {2.0, 50.0, 1.0, 1.0, 1.0};
            cout << "--No Weight-------- unsortedx2:" << endl;

            //showVector(unsortedx2, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            meanResult.clear();
            medianResult.clear();
            noWeight_mean_and_absdevAroundMean(unsortedx2, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx2, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx2, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();
        }

        {
            //https://www.shelovesmath.com/pre-algebra/introduction-to-statistics/
            //vector<double> sortedx3 = { 1, 2, 3, 6, 7, 8, 8,10,12,12,14,14,15,17,20,22,23,28,32,42}; //median: 13 =((12+14)/2)
            vector<double> unsortedx4 = {10, 2,15,28, 1,32,12,14, 8,17,22, 6,42, 3,14, 7,12,23,20, 8}; //mean:14.8  MAD=8.11
            cout << "--No Weight-------- unsortedx4:" << endl;

            //showVector(unsortedx4, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            meanResult.clear();
            medianResult.clear();
            noWeight_mean_and_absdevAroundMean(unsortedx4, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx4, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx4, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();
        }

        {
            vector<double> unsortedx10_empty;
            vector<double> unsortedx11 = { .0};
            vector<double> unsortedx12 = { .0, .0};
            vector<double> unsortedx13 = { .0, .0, .0};

            vector<double> unsortedx14 = { 1.0};
            vector<double> unsortedx15 = {  .0, 1.0 };
            vector<double> unsortedx16 = {  .0,  .0, 1.0};
            vector<double> unsortedx17 = {  .0,  .0, .0, 1.0};
            vector<double> unsortedx18 = {10.0,  .0, .0, 0.0};

            vector<double> unsortedx19 = {1.00    , 1.00    , 1.00    , 1.00    , 1.00 };
            vector<double> unsortedx20 = {1.00    , 2.00    , 3.00    , 4.00    , 5.00 };

            vector<double> unsortedx21 = {-10.00  , +21.00  ,  3.00   , -4.00   ,  0.00 };

            vector<double> unsortedx22 = {-10.00  , -21.00  , -3.00   , -4.00   , -7.00 };

            vector<double> unsortedx25_oneValue = { 5.0 };

            cout << "--No Weight-------- unsortedx10_empty:" << endl;

            //showVector(unsortedx10_empty, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            meanResult.clear();
            medianResult.clear();
            noWeight_mean_and_absdevAroundMean    (unsortedx10_empty, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx10_empty, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx10_empty, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx11:" << endl;

            //showVector(unsortedx11, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            meanResult.clear();
            medianResult.clear();
            noWeight_mean_and_absdevAroundMean    (unsortedx11, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx11, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx11, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx12:" << endl;

            //showVector(unsortedx12, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            meanResult.clear();
            medianResult.clear();
            noWeight_mean_and_absdevAroundMean    (unsortedx12, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx12, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx12, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx13:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx13, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx13, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx13, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx13, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx14:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx14, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx14, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx14, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx14, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx15:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx15, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx15, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx15, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx15, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx16:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx16, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx16, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx16, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx16, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx17:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx17, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx17, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx17, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx17, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx18:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx18, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx18, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx18, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx18, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx19:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx19, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx19, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx19, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx19, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx20:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx20, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx20, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx20, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx20, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx21:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx21, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx21, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx21, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx21, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();

            cout << "--No Weight-------- unsortedx22:" << endl;
            meanResult.clear();
            medianResult.clear();

            //showVector(unsortedx22, "inputValues=");
            //showVector({}, "coef_correl=");
            //showVector({}, "puiss_poids=");

            noWeight_mean_and_absdevAroundMean    (unsortedx22, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx22, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx22, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();
            
            
            cout << "--No Weight-------- unsortedx25:" << endl;
            meanResult.clear();
            medianResult.clear();
            
            noWeight_mean_and_absdevAroundMean    (unsortedx25_oneValue, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx25_oneValue, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx25_oneValue, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();


            cout << "--No Weight-------- unsortedx26:" << endl;
            meanResult.clear();
            medianResult.clear();

            vector<double> unsortedx2666 = {12.0, 5.0, 6.0, 89.0, 5.0, 1.0};
            cout << "--No Weight-------- unsortedx2666:" << endl;
            
            noWeight_mean_and_absdevAroundMean    (unsortedx2666, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx2666, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();

            //noWeight_median_and_absDevAroundMedian_sorting(unsortedx2666, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            //medianResult.show();


          
        }
    }

    if (bTestWithWeight) {
     
        cout << "-- Tests With Weight (cpp):" << endl;
        /*with weights:
        - weigthed mean,
        - absolute deviation around the weigthed mean

        - weigthed median,
        - absolute deviation around the weigthed median
        */

        double powerWeight = 1.0;

        S_WMeanResult WMeanResult;
        S_WMedianResult WMedianResult;

        //test samples from Jack Peterson
        //https://gist.github.com/tinybike weighted_median.py
        //( https://gist.github.com/tinybike/d9ff1dad515b66cc0d87 )

        {
            vector<double> jpwm1_unsorted1         = {7.0,     1.0,     2.0,     4.0, 10.0};
            vector<double> jpwm1_unsorted1_weights = {1.0, 1.0/3.0, 1.0/3.0, 1.0/3.0,  1.0};
            cout << "--With Weight-------- jpwm1_unsorted1:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (jpwm1_unsorted1, jpwm1_unsorted1_weights, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(jpwm1_unsorted1, jpwm1_unsorted1_weights, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> jpwm1_unsorted2         = {7.0, 1.0, 2.0, 4.0, 10.0};
            vector<double> jpwm1_unsorted2_weights = {1.0, 1.0, 1.0, 1.0,  1.0};
            cout << "--With Weight-------- jpwm1_unsorted2:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (jpwm1_unsorted2, jpwm1_unsorted2_weights, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(jpwm1_unsorted2, jpwm1_unsorted2_weights, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> jpwm1_unsorted3         = {7.0,     1.0,     2.0,     4.0, 10.0, 15.0};
            vector<double> jpwm1_unsorted3_weights = {1.0, 1.0/3.0, 1.0/3.0, 1.0/3.0,  1.0,  1.0};
            cout << "--With Weight-------- jpwm1_unsorted3:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (jpwm1_unsorted3, jpwm1_unsorted3_weights, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(jpwm1_unsorted3, jpwm1_unsorted3_weights, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> jpwm1_unsorted4         = {    1.0,     2.0,     4.0, 7.0, 10.0, 15.0};
            vector<double> jpwm1_unsorted4_weights = {1.0/3.0, 1.0/3.0, 1.0/3.0, 1.0,  1.0,  1.0};
            cout << "--With Weight-------- jpwm1_unsorted4:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (jpwm1_unsorted4, jpwm1_unsorted4_weights, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(jpwm1_unsorted4, jpwm1_unsorted4_weights, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> jpwm1_unsorted5         = { 0.0,  10.0, 20.0, 30.0};
            vector<double> jpwm1_unsorted5_weights = {30.0, 191.0,  9.0,  0.0};
            cout << "--With Weight-------- jpwm1_unsorted5:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (jpwm1_unsorted5, jpwm1_unsorted5_weights, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(jpwm1_unsorted5, jpwm1_unsorted5_weights, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> jpwm1_unsorted6         = { 1.0, 2.0, 3.0, 4.0, 5.0};
            vector<double> jpwm1_unsorted6_weights = {10.0, 1.0, 1.0, 1.0, 9.0};
            cout << "--With Weight-------- jpwm1_unsorted6:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (jpwm1_unsorted6, jpwm1_unsorted6_weights, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(jpwm1_unsorted6, jpwm1_unsorted6_weights, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> jpwm1_unsorted7         = {30.0, 40.0, 50.0, 60.0, 35.0};
            vector<double> jpwm1_unsorted7_weights = { 1.0,  3.0,  5.0,  4.0,  2.0};
            cout << "--With Weight-------- jpwm1_unsorted7:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (jpwm1_unsorted7, jpwm1_unsorted7_weights, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(jpwm1_unsorted7, jpwm1_unsorted7_weights, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> jpwm1_unsorted8         = {2.0, 0.6, 1.3, 0.3, 0.3, 1.7, 0.7, 1.7, 0.4};
            vector<double> jpwm1_unsorted8_weights = {2.0, 2.0, 0.0, 1.0, 2.0, 2.0, 1.0, 6.0, 0.0};
            cout << "--With Weight-------- jpwm1_unsorted8:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (jpwm1_unsorted8, jpwm1_unsorted8_weights, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(jpwm1_unsorted8, jpwm1_unsorted8_weights, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> sortedx1                                = {1.00    , 2.00    , 3.00    , 4.00    , 5.00    };
            vector<double> normalizedWeights_syncWithSortedx1      = {0.15    , 0.10    , 0.20    , 0.30    , 0.25    };
            //vector<double> notNormalizedWeights = {0.15*810, 0.10*810, 0.20*810, 0.30*810, 0.25*810};

            cout << "--With Weight-------- sortedx1:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (sortedx1, normalizedWeights_syncWithSortedx1, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(sortedx1, normalizedWeights_syncWithSortedx1, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> unsortedx1                              = {3.00    , 1.00    , 5.00    , 2.00    , 4.00    };
            vector<double> notNormalizedWeights_syncWithUnsortedx1 = {0.20*810, 0.15*810, 0.25*810, 0.10*810, 0.30*810};
            cout << "--With Weight-------- unsortedx1:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (unsortedx1, notNormalizedWeights_syncWithUnsortedx1, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(unsortedx1, notNormalizedWeights_syncWithUnsortedx1, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> sortedx2             = {1.00    , 2.00    , 3.00    , 4.00    , 5.00    };
            vector<double> normalizedWeightsx2  = {1.0/5.0 , 1.0/5.0 , 1.0/5.0 , 1.0/5.0 , 1.0/5.0 };
            cout << "--With Weight-------- sortedx2:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (sortedx2, normalizedWeightsx2, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(sortedx2, normalizedWeightsx2, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        //wikipedia cases
        //https://en.wikipedia.org/wiki/Weighted_median
        //####LP see source code provided by wiki; and compare with robutstats also
        {
            vector<double> sorted_wiki1             = {1.00 , 2.00 , 3.00 , 4.00 , 5.00 };
            vector<double> normalizedWeights_wiki1  = {0.15 , 0.10 , 0.20 , 0.30 , 0.25 };
            //median : 3
            //wmedian: 4
            cout << "--With Weight-------- sorted_wiki1:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (sorted_wiki1, normalizedWeights_wiki1, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(sorted_wiki1, normalizedWeights_wiki1, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> sorted_wiki2                = {1.00 , 2.00 , 3.00 , 4.00 };
            vector<double> nonNormalizedWeights_wiki2  = {0.25 , 0.25 , 0.25 , 0.25 };
            //median : 2.5
            //lower wmedian: 2
            //upper wmedian: 3
            //wmedian:2.5
            cout << "--With Weight-------- sorted_wiki2:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (sorted_wiki2, nonNormalizedWeights_wiki2, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(sorted_wiki2, nonNormalizedWeights_wiki2, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> sorted_wiki3                = {1.00 , 2.00 , 3.00 , 4.00 };
            vector<double> nonNormalizedWeights_wiki3  = {0.49 , 0.01 , 0.25 , 0.25 };
            //median: 2.5
            //lower wmedian: 2
            //upper wmedian: 3
            //wmedian: 2 (lower) or better: mean of lower wmedian and upper wmedian : 2.5
            cout << "--With Weight-------- sorted_wiki3:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (sorted_wiki3, nonNormalizedWeights_wiki3, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(sorted_wiki3, nonNormalizedWeights_wiki3, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> unsorted_4             = { 0.1, 0.35, 0.05, 0.1, 0.15, 0.05, 0.2};
            vector<double> nonNormalizedWeights_4 = { 0.1, 0.35, 0.05, 0.1, 0.15, 0.05, 0.2};
            cout << "--With Weight-------- unsorted_4:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (unsorted_4, nonNormalizedWeights_4, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(unsorted_4, nonNormalizedWeights_4, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> unsortedx21             = {-10.00  , +21.00    ,  3.00    , -4.00  ,  0.00 };
            vector<double> nonNormalizedWeights_21 = { 20     ,   0.35    , 30.51    , 12     , 80    };
            cout << "--With Weight-------- unsortedx21:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (unsortedx21, nonNormalizedWeights_21, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(unsortedx21, nonNormalizedWeights_21, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> unsortedx22             = {-10.00  , -21.00    , -3.00    , -4.00  , -7.00 };
            vector<double> nonNormalizedWeights_22 = { 20     ,   0.35    , 30.51    , 12     , 80    };
            cout << "--With Weight-------- unsortedx22:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (unsortedx22, nonNormalizedWeights_22, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(unsortedx22, nonNormalizedWeights_22, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }


        {
            vector<double> unsortedx23             = { 1.0 , 2.0 , 3.0 , 4.0 , 5.0 };
            vector<double> nonNormalizedWeights_23 = { 0.1 , 0.1 , 0.1 , 0.1 , 0.6 };
            cout << "--With Weight-------- unsortedx23:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (unsortedx23, nonNormalizedWeights_23, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(unsortedx23, nonNormalizedWeights_23, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

        {
            vector<double> unsortedx24             = { 1.0 , 2.0 , 3.0 , 4.0 , 5.0 };
            vector<double> nonNormalizedWeights_24 = { 0.6 , 0.1 , 0.1 , 0.1 , 0.1 };
            cout << "--With Weight-------- unsortedx24:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (unsortedx24, nonNormalizedWeights_24, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(unsortedx24, nonNormalizedWeights_24, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }
        
        {
            vector<double> unsortedx25_oneValue             = { 5.0 };
            vector<double> nonNormalizedWeights_25_oneValue = { 0.7 };
            cout << "--With Weight-------- unsortedx25:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (unsortedx25_oneValue, nonNormalizedWeights_25_oneValue, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(unsortedx25_oneValue, nonNormalizedWeights_25_oneValue, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

    }



    { //additional test

        cout << "--- additional test" << endl;

        vector<double> unsortedx26_oneValue             = { -1.0, -1.0, -1.0, -1.0, -1.0, -0.8, -0.8, -0.8, -0.4, -0.4, -0.2 };
        vector<double> nonNormalizedWeights_26_oneValue = {  255,  255,  255,  255,  255,  204,  204,  204,  102,  102,   51 };

        double powerWeight = 1.0;

        {
            S_MeanResult meanResult;
            S_MedianResult medianResult;

            cout << "--No Weight-------- unsortedx26:" << endl;
            meanResult.clear();
            medianResult.clear();

            noWeight_mean_and_absdevAroundMean    (unsortedx26_oneValue, meanResult._mean, meanResult._absdevAroundMean, meanResult._bComputedValuesValid);
            meanResult.show();
            noWeight_median_and_absDevAroundMedian(unsortedx26_oneValue, medianResult._median, medianResult._absDevAroundMedian, medianResult._bComputedValuesValid);
            medianResult.show();
        }

        {

            S_WMeanResult WMeanResult;
            S_WMedianResult WMedianResult;

            cout << "--With Weight-------- unsortedx26:" << endl;
            WMeanResult.clear();
            WMedianResult.clear();
            withWeight_wMean_wAbsdevAroundWMean       (unsortedx26_oneValue, nonNormalizedWeights_26_oneValue, powerWeight, WMeanResult._wMean, WMeanResult._wAbsdevAroundMean, WMeanResult._bComputedValuesValid);
            WMeanResult.show();
            withWeight_wMedian_and_absDevAroundWMedian(unsortedx26_oneValue, nonNormalizedWeights_26_oneValue, powerWeight, WMedianResult._wMedian, WMedianResult._absDevAroundWMedian, WMedianResult._bComputedValuesValid);
            WMedianResult.show();
        }

    }

    return(0);
}
