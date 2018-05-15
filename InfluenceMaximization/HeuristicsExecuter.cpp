//
//  HeuristicsExecuter.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 5/15/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include "HeuristicsExecuter.hpp"
#include "TIMInfluenceCalculator.hpp"

set<int> HeuristicsExecuter::executeNonTargetMinimizer(Graph *graph, int budget, int nontargetThreshold) {
    TIMInfluenceCalculator timInfluenceCalculator(graph, 2);
    double scalingFactorNonTargets = timInfluenceCalculator.getScalingFactorNonTargets();
    
    set<int> seedSet;
    shared_ptr<TIMCoverage> timCoverageTargets = timInfluenceCalculator.getTimCoverageTargets();
    shared_ptr<TIMCoverage> timCoverageNonTargets = timInfluenceCalculator.getTimCoverageNonTargets();

    vector<vector<int>> *rrSetTargets = timInfluenceCalculator.getRRsetsTargets();
    vector<vector<int>> *rrSetNonTargets = timInfluenceCalculator.getRRsetsNonTargets();
    for (int i=0; i<budget; i++) {
        timCoverageNonTargets->constructReverseQueue();
        set<int> candidateNodes = timCoverageNonTargets->findMinInfluentialNodes(rrSetNonTargets);
        int maxNode = -1;
        int maxInfluence = INT_MIN;
        assert(candidateNodes.size()>0);
        for (int c: candidateNodes) {
            assert(c!=-1);
            int newInfluence = timCoverageTargets->numberOfNewRRSetsCoveredByVertex(c);
            if(newInfluence>maxInfluence && seedSet.find(c)==seedSet.end()) {
                maxNode = c;
                maxInfluence = newInfluence;
            }
            
        }
        if(round(scalingFactorNonTargets * (maxInfluence + timCoverageNonTargets->getNumberOfRRSetsCovered()))>=nontargetThreshold) {
            break;
        }
        timCoverageTargets.get()->addToSeed(maxNode, rrSetTargets);
        timCoverageNonTargets.get()->addToSeed(maxNode, rrSetNonTargets);
        seedSet.insert(maxNode);
        assert(seedSet.size()==i+1);
    }
    return seedSet;
}
