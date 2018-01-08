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
#include "FunctionEvaluators.hpp"
#include "Graph.hpp"
#include <algorithm>
#include "ApproximationSetting.hpp"

using namespace std;


class ModularApproximation {
    vector<int> *permutation;
    vector<int> *reverseMap;
    int n;
    int *approximations;
    TIMEvaluator *timEvaluator;
    ApproximationSetting setting;
public:
    ModularApproximation(vector<int> permutation, ApproximationSetting approximationSetting);
    ~ModularApproximation();
    vector<int> getPerumutation();
    vector<int> getReverseMap();
    TIMEvaluator *getTIMEvaluator();
    int* getApproximations();
    
    void createTIMEvaluator(Graph *graph);
    void setApproximationSetting(ApproximationSetting approximationSetting);
    void constructReverseMap();
    void calculateApproximation(int element, set<int> *vertices);
    void findAllApproximations();
    int evaluateFunction(set<int> elements);
    int evaluateFunction(int element);
};

class DifferenceApproximator {
    vector<int> *permutation;
    int n;
    Graph *graph;
public:
    DifferenceApproximator(Graph *graph);
    ~DifferenceApproximator();
    void setN(int n);
    vector<int> generatePermutation();
    vector<int> generatePermutation(vector<int> startingElements);
    set<int> executeGreedyAlgorithm(Graph *graph, ModularApproximation *modularApproximation, int k);
    set<int> executeGreedyAlgorithmAdjustingPermutation(ApproximationSetting setting, int k);
    set<int> executeAlgorithmApproximatingOneFunction(ApproximationSetting setting, int k);
    set<int> executeAlgorithmApproximatingOneFunctionExtendPermutation(ApproximationSetting setting, int k);
};

#endif /* DifferenceApproximator_hpp */
