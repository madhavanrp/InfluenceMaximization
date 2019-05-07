//
//  NormalizedInfluenceCalculator.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 4/24/19.
//  Copyright © 2019 Madhavan R.P. All rights reserved.
//

#ifndef NormalizedInfluenceCalculator_hpp
#define NormalizedInfluenceCalculator_hpp

#include <stdio.h>
#include "Graph.hpp"
#include "TIM-Models/TIMNormalizedCoverage.hpp"
#include "TIM-Models/TIMCoverage.hpp"
class NormalizedInfluenceCalculator {
protected:
    Graph *graph;
    double epsilon;
    string model;
    
    vector<vector<int>> rrSetsTargets;
    sfmt_t sfmt;
    int R;
    
    vector<int> nonTargetCounts, targetCounts, visitMark;
    vector<double> nonTargetInfluence;
    vector<bool> visited;
    deque<int> q;
    
    shared_ptr<TIMNormalizedCoverage> normalizedCoverage;
    
    shared_ptr<vector<vector<int>>> lookupTableTargets;
    
public:
    NormalizedInfluenceCalculator(Graph *graph);
    void constructCalculator(Graph *graph, double epsilon, string model);
    void generateRandomRRSetsNonTargets(int R);
    void generateRandomRRSetNonTargets(int randomVertex, vector<int> *counts);
    void generateRandomRRSetsTargets(int R);
    void generateRandomRRSetTargets(int randomVertex, vector<vector<int>> *rrSets, vector<int> *counts);
    double getScalingFactorTargets();
    double getScalingFactorNonTargets();
    set<int> findTopNodes(int costConstraint);
};
#endif /* NormalizedInfluenceCalculator_hpp */
