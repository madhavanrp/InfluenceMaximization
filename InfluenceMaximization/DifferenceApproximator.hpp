//
//  DifferenceApproximator.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef DifferenceApproximator_hpp
#define DifferenceApproximator_hpp

#include <stdio.h>
#include <vector>
#include <set>
#include "TIMEvaluator.hpp"
#include "Graph.hpp"
#include <algorithm>
#include "ApproximationSetting.hpp"
#include "TIMInfluenceCalculator.hpp"

using namespace std;


class ModularApproximation {
    vector<int> *permutation;
    vector<int> *reverseMap;
    int n;
    double *approximations;
    TIMEvaluator *timEvaluator;
    ApproximationSetting setting;
public:
    ModularApproximation(vector<int> permutation, ApproximationSetting approximationSetting);
    ~ModularApproximation();
    ModularApproximation& operator=( const ModularApproximation &obj);
    ModularApproximation( const ModularApproximation &obj);
    vector<int> getPerumutation();
    vector<int> getReverseMap();
    TIMEvaluator *getTIMEvaluator();
    double* getApproximations();
    
    void createTIMEvaluator(Graph *graph);
    void setApproximationSetting(ApproximationSetting approximationSetting);
    void constructReverseMap();
    void calculateApproximation(int element, set<int> *vertices);
    void findAllApproximations();
    double evaluateFunction(set<int> elements);
    double evaluateFunction(int element);
    
};

class DifferenceApproximator {
    vector<int> *permutation;
    int n;
    Graph *graph;
    double differenceValue;
public:
    double getDifferenceValue();
    DifferenceApproximator(Graph *graph);
    ~DifferenceApproximator();
    DifferenceApproximator& operator=( const DifferenceApproximator &obj);
    DifferenceApproximator( const DifferenceApproximator &obj);
    void setN(int n);
    vector<int> generatePermutation();
    vector<int> generatePermutation(vector<int> startingElements);
    set<int> executeGreedyAlgorithm(ApproximationSetting setting, int k);
    set<int> executeGreedyAlgorithmAdjustingPermutation(ApproximationSetting setting, int k);
    set<int> executeAlgorithmApproximatingOneFunction(ApproximationSetting setting, int k);
    set<int> executeAlgorithmApproximatingOneFunctionExtendPermutation(ApproximationSetting setting, int k);
    
    set<int> executeAlgorithmModularG(int k);
    set<int> executeSupSubProcedure(int k);
    vector<double> calculateUpperBound(TIMCoverage *timCoverageNonTargets, double scalingFactorNonTargets, set<int> relativeSet);
    set<int> randGreedyCSO(TIMCoverage *timCoverageDifference, vector<vector<int>> *rrSets);
};

#endif /* DifferenceApproximator_hpp */
