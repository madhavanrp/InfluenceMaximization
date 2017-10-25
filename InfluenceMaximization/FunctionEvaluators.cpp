//
//  FunctionEvaluators.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "FunctionEvaluators.hpp"
#include <assert.h>

TIMEvaluator::TIMEvaluator(Graph *graph) {
    this->graph = graph;
    calculateNonTargets();
    calculateTargets();
    this->count = 0;
}

TIMEvaluator::~TIMEvaluator() {
    if(this->timCoverage!=NULL) {
        delete this->timCoverage;
    }
}

TIMCoverage* TIMEvaluator::getTIMCoverage() {
    return this->timCoverage;
}

int TIMEvaluator::getCount() {
    return count;
}

void TIMEvaluator::calculateNonTargets() {
    EstimateNonTargets estimateNonTargets(*this->graph);
    this->nonTargets = estimateNonTargets.getNonTargetsUsingTIM();
}

void TIMEvaluator::calculateTargets() {
    
    int n = this->graph->n;
    double epsilon = TIM_EPSILON_TARGETS;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    graph->generateRandomRRSets(R, true);
    rrSetsTargets = graph->getRandomRRSets();
    vector<vector<int>> *lookupTable = new vector<vector<int>>();
    this->timCoverage = new TIMCoverage(lookupTable);
    this->timCoverage->initializeLookupTable(rrSetsTargets, n);
    this->timCoverage->initializeDataStructures(R, n);
}

int TIMEvaluator::findInfluenceOnTargets(set<int> *seedSet) {
    //Don't create copy for now. TODO: Complete this correctly
    TIMCoverage *coverageCopy = this->timCoverage;
    int totalInfluence = 0;
    
    vector<int> *coverage = &coverageCopy->coverage;
    vector<bool> *nodeMark = &coverageCopy->nodeMark;
    vector<bool> *edgeMark = &coverageCopy->edgeMark;
    for(int vertex: *seedSet) {
        (*nodeMark)[vertex] = false;
        //Add the marginal gain
        totalInfluence+= (*coverage)[vertex];
        int numberCovered = coverageCopy->countForVertex(vertex);
        
        vector<int> edgeInfluence = (*coverageCopy->lookupTable)[vertex];
        
        for (int i = 0; i < numberCovered; i++) {
            if ((*edgeMark)[edgeInfluence[i]]) continue;
            
            vector<int> nList = rrSetsTargets[edgeInfluence[i]];
            for (int l :
                 nList) {
                if ((*nodeMark)[l]) {
                    (*coverage)[l]--;
                }
            }
            (*edgeMark)[edgeInfluence[i]] = true;
        }
    }
    int scaledInfluence = (double)totalInfluence * nodeMark->size()/(int)rrSetsTargets.size();
    return scaledInfluence;
}

pair<int, int> TIMEvaluator::findInfluence(set<int> *seedSet) {
    // For now not calculating the non targets modularly. TODO: separate this.
    int targets = findInfluenceOnTargets(seedSet);
    int nonTargetsInfluenced = 0;
    for(int seed:*seedSet) {
        nonTargetsInfluenced+= findSingleNodeNonTargetsInfluence(seed);
    }
    return make_pair(targets, nonTargetsInfluenced);
}

int TIMEvaluator::findSingleNodeNonTargetsInfluence(int vertex) {
    return this->nonTargets[vertex];
}
