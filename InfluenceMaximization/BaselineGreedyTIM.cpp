//
//  BaselineGreedyTIM.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 1/26/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include "BaselineGreedyTIM.hpp"

set<int> BaselineGreedyTIM::findSeedSet(Graph *graph, int budget, int nonTargetThreshold) {
    TIMInfluenceCalculator timInfluenceCalculator(graph, 2);
    this->rrSetsNonTargetsThreshold = ((double)1/timInfluenceCalculator.getScalingFactorNonTargets()) * nonTargetThreshold;
    this->budget = budget;
    this->threshold = nonTargetThreshold;
    
    set<int> seedSet;
    shared_ptr<TIMCoverage> timCoverageTargets = timInfluenceCalculator.getTimCoverageTargets();
    shared_ptr<TIMCoverage> timCoverageNonTargets = timInfluenceCalculator.getTimCoverageNonTargets();
    
    vector<vector<int>> *rrSetTargets = timInfluenceCalculator.getRRsetsTargets();
    vector<vector<int>> *rrSetNonTargets = timInfluenceCalculator.getRRsetsNonTargets();
    this->rrSetTargets = rrSetTargets;
    this->rrSetNonTargets = rrSetNonTargets;
    this->timCoverageTargets = timCoverageTargets;
    this->timCoverageNonTargets = timCoverageNonTargets;
    
    for (int i=0; i<budget; i++) {
        int maxNode = timCoverageTargets.get()->findMaxInfluentialNodeAndUpdateModel(rrSetTargets, this).first;
        timCoverageNonTargets.get()->addToSeed(maxNode, rrSetNonTargets);
        seedSet.insert(maxNode);
        this->orderedSeed.push_back(maxNode);
    }
    FILE_LOG(logDEBUG) << "\n Baseline Greedy with TIM completed";
    FILE_LOG(logDEBUG) << "\n Scaling factor targets: " << timInfluenceCalculator.getScalingFactorTargets();
    FILE_LOG(logDEBUG) << "\n Scaling factor Non targets: " << timInfluenceCalculator.getScalingFactorNonTargets();
    FILE_LOG(logDEBUG) << "\n Targets: Number of RR sets covered: " << timCoverageTargets.get()->getNumberOfRRSetsCovered();
    FILE_LOG(logDEBUG) << "\n Targets hit: " << timCoverageTargets.get()->getNumberOfRRSetsCovered() * timInfluenceCalculator.getScalingFactorTargets();
    
    FILE_LOG(logDEBUG) << "\n Non Targets: Number of RR sets covered: " << timCoverageNonTargets.get()->getNumberOfRRSetsCovered();
    FILE_LOG(logDEBUG) << "\n Non Targets hit: " << timCoverageNonTargets.get()->getNumberOfRRSetsCovered() * timInfluenceCalculator.getScalingFactorNonTargets();
    return seedSet;
}

bool BaselineGreedyTIM::isNodeValid(int nodeID) {
    int numberOfNonTargetRRSetsCovered = this->timCoverageNonTargets.get()->numberOfNewRRSetsCoveredByVertex(nodeID);
//    cout << "\n Calling here to check validity";
//    cout << "\n " << numberOfNonTargetRRSetsCovered + this->timCoverageNonTargets.get()->getNumberOfRRSetsCovered() << " and  " << this->rrSetsNonTargetsThreshold;
    return (numberOfNonTargetRRSetsCovered + this->timCoverageNonTargets.get()->getNumberOfRRSetsCovered() <=this->rrSetsNonTargetsThreshold);
}

vector<int> BaselineGreedyTIM::getOrderedSeed() {
    return this->orderedSeed;
}
