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
    timCoverageTargets.get()->setNumberOfNodesToCover(graph->getNumberOfTargets());
    shared_ptr<TIMCoverage> timCoverageNonTargets = timInfluenceCalculator.getTimCoverageNonTargets();
    timCoverageNonTargets.get()->setNumberOfNodesToCover(graph->getNumberOfNonTargets());
    
    vector<vector<int>> *rrSetTargets = timInfluenceCalculator.getRRsetsTargets();
    vector<vector<int>> *rrSetNonTargets = timInfluenceCalculator.getRRsetsNonTargets();
    this->rrSetTargets = rrSetTargets;
    this->rrSetNonTargets = rrSetNonTargets;
    this->timCoverageTargets = timCoverageTargets;
    this->timCoverageNonTargets = timCoverageNonTargets;
    
    vector<double> maxTargetsExceedingThreshold;
    double targetsAccordingToCalculation = 0;
    double bestThetaGain = 0;
    double bestThetaSummation = 0;
    for (int i=0; i<budget; i++) {
        //First get two theta
        this->exceedThreshold = true;
        pair<int, double> nodeWithTargets = timCoverageTargets.get()->findMaxInfluentialNodeWithNoUpdates(rrSetTargets, this);
        double numberOfTargetsTwiceThreshold = nodeWithTargets.second;
        maxTargetsExceedingThreshold.push_back(numberOfTargetsTwiceThreshold);
        bestThetaSummation+=numberOfTargetsTwiceThreshold;
        if (numberOfTargetsTwiceThreshold>bestThetaGain) {
            bestThetaGain = numberOfTargetsTwiceThreshold;
        }
        this->bestThetaGainSummations.push_back(bestThetaSummation);
        this->bestThetaGains.push_back(bestThetaGain);
        
        
        this->exceedThreshold = false;
        pair<int, double> testPair = timCoverageTargets.get()->findMaxInfluentialNodeWithNoUpdates(rrSetTargets, this);
        int newRRSetsCovered = timCoverageTargets.get()->numberOfNewRRSetsCoveredByVertex(testPair.first);
        pair<int, double> maxNodeAndTargets = timCoverageTargets.get()->findMaxInfluentialNodeAndUpdateModel(rrSetTargets, this);
        double influenceGainCalculated = (double)newRRSetsCovered * timInfluenceCalculator.getScalingFactorTargets();
        assert(testPair.first==maxNodeAndTargets.first);
        assert(testPair.second==maxNodeAndTargets.second);
        assert(influenceGainCalculated==maxNodeAndTargets.second);
        int maxNode = maxNodeAndTargets.first;
        targetsAccordingToCalculation+=maxNodeAndTargets.second;
        timCoverageNonTargets.get()->addToSeed(maxNode, rrSetNonTargets);
        
        
        seedSet.insert(maxNode);
        this->orderedSeed.push_back(maxNode);
    }
    this->bestThetaGain = bestThetaGain;
    this->bestThetaGainSummation = bestThetaSummation;
    FILE_LOG(logDEBUG) << "\n Baseline Greedy with TIM completed";
    FILE_LOG(logDEBUG) << "\n Scaling factor targets: " << timInfluenceCalculator.getScalingFactorTargets();
    FILE_LOG(logDEBUG) << "\n Scaling factor Non targets: " << timInfluenceCalculator.getScalingFactorNonTargets();
    FILE_LOG(logDEBUG) << "\n Targets: Number of RR sets covered: " << timCoverageTargets.get()->getNumberOfRRSetsCovered();
    FILE_LOG(logDEBUG) << "\n Targets hit: " << timCoverageTargets.get()->getNumberOfRRSetsCovered() * timInfluenceCalculator.getScalingFactorTargets();
    FILE_LOG(logDEBUG) << "\n Targets according to calculation: " << targetsAccordingToCalculation;
    
    FILE_LOG(logDEBUG) << "\n Non Targets: Number of RR sets covered: " << timCoverageNonTargets.get()->getNumberOfRRSetsCovered();
    FILE_LOG(logDEBUG) << "\n Non Targets hit: " << timCoverageNonTargets.get()->getNumberOfRRSetsCovered() * timInfluenceCalculator.getScalingFactorNonTargets();
    return seedSet;
}

bool BaselineGreedyTIM::isNodeValid(int nodeID) {
    int numberOfNonTargetRRSetsCovered = this->timCoverageNonTargets.get()->numberOfNewRRSetsCoveredByVertex(nodeID);
    int rrSetsForNonTargets = this->exceedThreshold? 2 * this->rrSetsNonTargetsThreshold:this->rrSetsNonTargetsThreshold;
    return (numberOfNonTargetRRSetsCovered + this->timCoverageNonTargets.get()->getNumberOfRRSetsCovered() <=rrSetsForNonTargets);
}

double BaselineGreedyTIM::getScalingFactor() {
    return (double)graph->getNumberOfNonTargets()/this->rrSetNonTargets->size();
}

double BaselineGreedyTIM::getBestThetaGain() {
    return this->bestThetaGain;
}


vector<double> BaselineGreedyTIM::getBestThetaGains() {
    return this->bestThetaGains;
}

vector<int> BaselineGreedyTIM::getOrderedSeed() {
    return this->orderedSeed;
}

double BaselineGreedyTIM::getBestThetaGainSummation() {
    return this->bestThetaGainSummation;
}

vector<double> BaselineGreedyTIM::getBestThetaGainSummations() {
    return this->bestThetaGainSummations;
}

