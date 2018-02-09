//
//  BaselineGreedyTIM.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 1/26/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#ifndef BaselineGreedyTIM_hpp
#define BaselineGreedyTIM_hpp

#include <stdio.h>
#include "Graph.hpp"
#include <set>
#include "TIMInfluenceCalculator.hpp"
#include "TIM-Models/TIMCoverage.hpp"
#include "log.h"

class BaselineGreedyTIM: public virtual NodeChecker {
    vector<int> orderedSeed; // For greedy algorithm.
    int budget;
    int threshold;
    bool exceedThreshold;
    int rrSetsNonTargetsThreshold;
    Graph *graph;
    vector<vector<int>> *rrSetTargets;
    vector<vector<int>> *rrSetNonTargets;
    shared_ptr<TIMCoverage> timCoverageTargets;
    shared_ptr<TIMCoverage> timCoverageNonTargets;
    vector<double> bestThetaGainSummations;
    vector<double> bestThetaGains;
    double bestThetaGain;
    double bestThetaGainSummation;
public:
    vector<int> getOrderedSeed();
    set<int> findSeedSet(Graph *graph, int budget, int nonTargetThreshold);
    double getBestThetaGain();
    vector<double> getBestThetaGains();
    
    double getBestThetaGainSummation();
    vector<double> getBestThetaGainSummations();
    
    virtual bool isNodeValid(int nodeID);
    virtual double getScalingFactor();
    
};

#endif /* BaselineGreedyTIM_hpp */
