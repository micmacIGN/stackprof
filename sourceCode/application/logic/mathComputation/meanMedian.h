#ifndef meanMedian_H
#define meanMedian_H

#include <vector>
#include <string>

using namespace std;

void showVector(const vector<double>& vect, const string& strVectName);

bool noWeight_mean_and_absdevAroundMean(const vector<double>& vectUnsortedValues, double& mean  , double& absdevAroundMean  , bool& bComputedValuesValid);

bool noWeight_median_and_absDevAroundMedian(vector<double> vectUnsortedValues , double& median, double& absDevAroundMedian, bool& bComputedValuesValid);

bool withWeight_wMean_wAbsdevAroundWMean(const vector<double>& vectValues, const vector<double>& vectNonNormalizedWeight, double powerOfWeight,
                                        double& wMean, double& wAbsdevAroundMean, bool& bComputedValuesValid);

bool withWeight_wMedian_and_absDevAroundWMedian(vector<double> vectValues, const vector<double>& vectNonNormalizedWeight, double powerOfWeight,
                                        double& wMedian, double& absDevAroundWMedian, bool& bComputedValuesValid);

#endif
