//
//  FunctionEvaluators.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "FunctionEvaluators.hpp"
#include <assert.h>

TIMEvaluator::TIMEvaluator(Graph *graph, ApproximationSetting approximationSetting) {
    this->graph = graph;
    this->setting = approximationSetting;
    calculateNonTargets();
    calculateTargets();
    this->count = 0;
}

TIMEvaluator::~TIMEvaluator() {
    if(this->timCoverage!=NULL) {
        delete this->timCoverage;
    }
    if(this->timCoverageNonTargets!=NULL) {
        delete this->timCoverageNonTargets;
    }
}

TIMCoverage* TIMEvaluator::getTIMCoverage() {
    return this->timCoverage;
}

TIMCoverage* TIMEvaluator::getTIMCoverageNonTargets() {
    return this->timCoverageNonTargets;
}

vector<vector<int>>* TIMEvaluator::getRRSetsTargets() {
    return &this->rrSetsTargets;
}
vector<vector<int>>* TIMEvaluator::getRRSetsNonTargets() {
    return &this->rrSetsNonTargets;
}

int TIMEvaluator::getCount() {
    return count;
}

double TIMEvaluator::getScalingFactorTargets() {
    double scalingFactor = (double)this->graph->n/(int)this->rrSetsTargets.size();
    return scalingFactor;
}

double TIMEvaluator::getScalingFactorNonTargets() {
    double scalingFactor = (double)this->graph->getNumberOfNonTargets()/(int)this->rrSetsNonTargets.size();
    return scalingFactor;
}

void TIMEvaluator::calculateNonTargets() {
    EstimateNonTargets estimateNonTargets(this->graph);
    this->nonTargets = estimateNonTargets.getNonTargetsUsingTIM();
    this->rrSetsNonTargets = *estimateNonTargets.getRandomRRSets();
    
    int n = this->graph->n;
    int R = (int)this->rrSetsNonTargets.size();
    vector<vector<int>> *lookupTable = new vector<vector<int>>();
    this->timCoverageNonTargets = new TIMCoverage(lookupTable);
    this->timCoverageNonTargets->initializeLookupTable(&this->rrSetsNonTargets, n);
    this->timCoverageNonTargets->initializeDataStructures(R, n);
    
}

void TIMEvaluator::calculateTargets() {
    
    int n = this->graph->n;
    double epsilon = TIM_EPSILON_TARGETS;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    graph->generateRandomRRSets(R, true);
    rrSetsTargets = (*graph->getRandomRRSets());
    vector<vector<int>> *lookupTable = new vector<vector<int>>();
    this->timCoverage = new TIMCoverage(lookupTable);
    this->timCoverage->initializeLookupTable(&rrSetsTargets, n);
    this->timCoverage->initializeDataStructures(R, n);
}

int TIMEvaluator::findInfluenceOnTargets(set<int> *seedSet) {
    //Don't create copy for now. TODO: Complete this correctly
    TIMCoverage *timCoverage = this->timCoverage;
    double scalingFactor = getScalingFactorTargets();
    int scaledInfluence =
    findGenericInfluence(seedSet, timCoverage, &this->rrSetsTargets, scalingFactor);
    return scaledInfluence;
}

int TIMEvaluator::findInfluenceOnNonTargets(set<int> *seedSet) {
    TIMCoverage *timCoverage = this->timCoverageNonTargets;
    double scalingFactor = getScalingFactorNonTargets();
    int scaledInfluence =
    findGenericInfluence(seedSet, timCoverage, &this->rrSetsNonTargets, scalingFactor);
    return scaledInfluence;
}

// We want this to operate on random RR sets for targets and non targets
int TIMEvaluator::findGenericInfluence(set<int> *seedSet, TIMCoverage *timCoverage, vector<vector<int>> *randomRRSets, double scalingFactor) {
    
    int totalInfluence = 0;
    
    vector<int> *coverage = &timCoverage->coverage;
    vector<bool> *nodeMark = &timCoverage->nodeMark;
    vector<bool> *edgeMark = &timCoverage->edgeMark;
    for(int vertex: *seedSet) {
        (*nodeMark)[vertex] = false;
        //Add the marginal gain
        totalInfluence+= (*coverage)[vertex];
        int numberCovered = timCoverage->countForVertex(vertex);
        
        vector<int> edgeInfluence = (*timCoverage->lookupTable)[vertex];
        
        for (int i = 0; i < numberCovered; i++) {
            if ((*edgeMark)[edgeInfluence[i]]) continue;
            
            vector<int> nList = (*randomRRSets)[edgeInfluence[i]];
            for (int l :
                 nList) {
                if ((*nodeMark)[l]) {
                    (*coverage)[l]--;
                }
            }
            (*edgeMark)[edgeInfluence[i]] = true;
        }
    }
    int scaledInfluence = round((double)totalInfluence * scalingFactor);
    return scaledInfluence;
}

pair<int, int> TIMEvaluator::findInfluence(set<int> *seedSet) {
    // For now not calculating the non targets modularly. TODO: separate this.
    int targetsInfluenced=0, nonTargetsInfluenced=0;
    switch (this->setting) {
        case setting1: {
            targetsInfluenced = findInfluenceOnTargets(seedSet);
            for(int seed:*seedSet) {
                nonTargetsInfluenced+= findSingleNodeNonTargetsInfluence(seed);
            }
            break;
        }
        case setting2: {
            
            targetsInfluenced = findInfluenceOnTargets(seedSet);
            nonTargetsInfluenced = findInfluenceOnNonTargets(seedSet);
            break;
        }
        case setting3: {
            nonTargetsInfluenced = findInfluenceOnNonTargets(seedSet);
            targetsInfluenced = 0;
            break;
        }
        case setting4: {
            for(int seed:*seedSet) {
                targetsInfluenced+= findSingleNodeTargetsInfluence(seed);
            }
            nonTargetsInfluenced = findInfluenceOnNonTargets(seedSet);
            break;
        }
    }
    
    return make_pair(targetsInfluenced, nonTargetsInfluenced);
}

int TIMEvaluator::findSingleNodeTargetsInfluence(int vertex) {
    int rrSetsCovered = this->timCoverage->countForVertex(vertex);
    return round(getScalingFactorTargets() * rrSetsCovered);
}

int TIMEvaluator::findSingleNodeNonTargetsInfluence(int vertex) {
    return this->nonTargets[vertex];
}
