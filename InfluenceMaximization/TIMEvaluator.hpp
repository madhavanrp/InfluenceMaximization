//
//  TIMEvaluator.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef TIMEvaluator_hpp
#define TIMEvaluator_hpp

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
    vector<vector<int>> *reverseMapTargets;
    vector<vector<int>> *reverseMapNonTargets;
    TIMCoverage *timCoverage;
    TIMCoverage *timCoverageNonTargets;
    int count;
    ApproximationSetting setting;
    
public:
    TIMEvaluator(Graph *graph, ApproximationSetting approximationSetting);
    ~TIMEvaluator();
    pair<double, double> findInfluence(set<int> *seedSet);
    double findInfluenceOnTargets(set<int> *seedSet);
    double findInfluenceOnNonTargets(set<int> *seedSet);
    
    double findSingleNodeTargetsInfluence(int vertex);
    double findSingleNodeNonTargetsInfluence(int vertex);
    double findGenericInfluence(set<int> *seedSet, TIMCoverage *timCoverage, vector<vector<int>> *randomRRSets, double scalingFactor);
    
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
