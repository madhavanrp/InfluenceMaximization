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

#define TIM_EPSILON_TARGETS 2
#define TIM_EPSILON_NON_TARGETS 2


class TIMEvaluator {
private:
    Graph *graph;
    EstimateNonTargets estimateNonTargets;
    vector<int> nonTargets;
    vector<vector<int>> rrSetsTargets;
    TIMCoverage *timCoverage;
    int count;
    
public:
    TIMEvaluator(Graph *graph);
    ~TIMEvaluator();
    pair<int, int> findInfluence(set<int> *seedSet);
    int findInfluenceOnTargets(set<int> *seedSet);
    int findSingleNodeNonTargetsInfluence(int vertex);
    
    void calculateNonTargets();
    void calculateTargets();
    
    //Getters
    TIMCoverage *getTIMCoverage();
    int getCount();
};
#endif /* FunctionEvaluators_hpp */
