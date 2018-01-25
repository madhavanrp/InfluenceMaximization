//
//  FunctionEvaluators.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef FunctionEvaluators_hpp
#define FunctionEvaluators_hpp

#include <stdio.h>
#include <iostream>
#include <set>
#include "Graph.hpp"
#include "EstimateNonTargets.hpp"
#include "ApproximationSetting.hpp"

#define TIM_EPSILON_TARGETS 2
#define TIM_EPSILON_NON_TARGETS 2


class TIMEvaluator {
private:
    Graph *graph;
    EstimateNonTargets estimateNonTargets;
    vector<double> nonTargets;
    vector<vector<int>> rrSetsTargets;
    vector<vector<int>> rrSetsNonTargets;
    TIMCoverage *timCoverage;
    TIMCoverage *timCoverageNonTargets;
    int count;
    ApproximationSetting setting;
    
public:
    TIMEvaluator(Graph *graph, ApproximationSetting approximationSetting);
    ~TIMEvaluator();
    pair<int, int> findInfluence(set<int> *seedSet);
    int findInfluenceOnTargets(set<int> *seedSet);
    int findInfluenceOnNonTargets(set<int> *seedSet);
    
    int findSingleNodeTargetsInfluence(int vertex);
    int findSingleNodeNonTargetsInfluence(int vertex);
    int findGenericInfluence(set<int> *seedSet, TIMCoverage *timCoverage, vector<vector<int>> *randomRRSets, double scalingFactor);
    
    void calculateNonTargets();
    void calculateTargets();
    
    //Getters
    TIMCoverage *getTIMCoverage();
    TIMCoverage *getTIMCoverageNonTargets();
    vector<vector<int>> *getRRSetsTargets();
    vector<vector<int>> *getRRSetsNonTargets();
    double getScalingFactorTargets();
    double getScalingFactorNonTargets();
    int getCount();
};
#endif /* FunctionEvaluators_hpp */
