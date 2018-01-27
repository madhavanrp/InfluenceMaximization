//
//  TIMInfluenceCalculator.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 1/16/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#ifndef TIMInfluenceCalculator_hpp
#define TIMInfluenceCalculator_hpp

#include <stdio.h>
#include "Graph.hpp"
#include <set>
#include <cmath>
#include <memory>
#include "TIM-Models/TIMCoverage.hpp"

class TIMInfluenceCalculator {
    Graph *graph;
    double epsilon;
    void constructCalculator(Graph *graph, double epsilon);
    vector<vector<int>> rrSetsNonTargets;
    vector<vector<int>> rrSetsTargets;
    
    shared_ptr<TIMCoverage> timCoverageTargets, timCoverageNonTargets;
    shared_ptr<vector<vector<int>>> lookupTableTargets, lookupTableNonTargets;
    deque<int> q;
    
    
    vector<bool> visited;
    vector<int> visitMark;
    
    
    //Maintain counts
    vector<int> nonTargetCounts;
    vector<int> targetCounts;
    
public:
    TIMInfluenceCalculator(Graph *graph);
    TIMInfluenceCalculator(Graph *graph, double epsilon);
    
    //Generation of Random RR Sets
    void generateRandomRRSet(int randomVertex, int rrSetID, vector<vector<int>> *rrSets, vector<int> *counts);
    void generateRandomRRSetsTargets(int R);
    void generateRandomRRSetsNonTargets(int R);
    
    //Finding influence
    pair<int, int> findInfluence(set<int> seedSet);
    pair<int, int> findInfluence(set<int> seedSet, set<int> *alreadyActivated);
    
    // Coverage Getters
    shared_ptr<TIMCoverage> getTimCoverageTargets();
    shared_ptr<TIMCoverage> getTimCoverageNonTargets();
    
    //RR Sets getters
    vector<vector<int>> *getRRsetsTargets();
    vector<vector<int>> *getRRsetsNonTargets();
    
    
    double getScalingFactorTargets();
    double getScalingFactorNonTargets();
};

#endif /* TIMInfluenceCalculator_hpp */
