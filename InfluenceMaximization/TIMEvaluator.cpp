//
//  FunctionEvaluators.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "TIMEvaluator.hpp"
#include <assert.h>

TIMEvaluator::TIMEvaluator(Graph *graph, ApproximationSetting approximationSetting) {
    this->timCoverage = NULL;
    this->timCoverageNonTargets = NULL;
    this->reverseMapTargets = NULL;
    this->reverseMapNonTargets = NULL;
    this->graph = graph;
    this->setting = approximationSetting;
    calculateNonTargets();
    calculateTargets();
    this->count = 0;
}

TIMEvaluator::~TIMEvaluator() {
    delete this->reverseMapTargets;
    delete this->reverseMapNonTargets;
    delete this->timCoverage;
    delete this->timCoverageNonTargets;
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
    double scalingFactor = (double)this->graph->getNumberOfTargets()/(int)this->rrSetsTargets.size();
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
    
    int n = this->graph->getNumberOfVertices();
    int R = (int)this->rrSetsNonTargets.size();
    this->reverseMapNonTargets = new vector<vector<int>>();
    this->timCoverageNonTargets = new TIMCoverage(this->reverseMapNonTargets);
    this->timCoverageNonTargets->initializeLookupTable(&this->rrSetsNonTargets, n);
    this->timCoverageNonTargets->initializeDataStructures(R, n);
    
}

void TIMEvaluator::calculateTargets() {
    
    int n = this->graph->getNumberOfVertices();
    double epsilon = TIM_EPSILON_TARGETS;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    graph->generateRandomRRSets(R, true);
    rrSetsTargets = (*graph->getRandomRRSets());
    this->reverseMapTargets = new vector<vector<int>>();
    this->timCoverage = new TIMCoverage(this->reverseMapTargets);
    this->timCoverage->initializeLookupTable(&rrSetsTargets, n);
    this->timCoverage->initializeDataStructures(R, n);
}

double TIMEvaluator::findInfluenceOnTargets(set<int> *seedSet) {
    //Don't create copy for now. TODO: Complete this without updating model
    TIMCoverage *timCoverage = this->timCoverage;
    double scalingFactor = getScalingFactorTargets();
    double scaledInfluence = findGenericInfluence(seedSet, timCoverage, &this->rrSetsTargets, scalingFactor);
    return scaledInfluence;
}

double TIMEvaluator::findInfluenceOnNonTargets(set<int> *seedSet) {
    TIMCoverage *timCoverage = this->timCoverageNonTargets;
    double scalingFactor = getScalingFactorNonTargets();
    double scaledInfluence = findGenericInfluence(seedSet, timCoverage, &this->rrSetsNonTargets, scalingFactor);
    return scaledInfluence;
}

// We want this to operate on random RR sets for targets and non targets
double TIMEvaluator::findGenericInfluence(set<int> *seedSet, TIMCoverage *timCoverage, vector<vector<int>> *randomRRSets, double scalingFactor) {
    
    double totalInfluence = 0;
    
    vector<int> *coverage = &timCoverage->coverage;
    vector<bool> *nodeMark = &timCoverage->nodeMark;
    vector<bool> *edgeMark = &timCoverage->edgeMark;
    for(int vertex: *seedSet) {
        if(!(*nodeMark)[vertex]) continue;
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
    timCoverage->incrementRRSetsCovered(totalInfluence);
    double scaledInfluence = totalInfluence * scalingFactor;
    return scaledInfluence;
}

pair<double, double> TIMEvaluator::findInfluence(set<int> *seedSet) {
    // For now not calculating the non targets modularly. TODO: separate this.
    double targetsInfluenced=0, nonTargetsInfluenced=0;
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
        case setting5: {
            targetsInfluenced = 0;
            for (int seed:*seedSet) {
                nonTargetsInfluenced+= findSingleNodeNonTargetsInfluence(seed);
            }
            
        }
        break;
    }
    
    return make_pair(targetsInfluenced, nonTargetsInfluenced);
}

double TIMEvaluator::findSingleNodeTargetsInfluence(int vertex) {
    int rrSetsCovered = this->timCoverage->countForVertex(vertex);
    return getScalingFactorTargets() * (double)rrSetsCovered;
}

double TIMEvaluator::findSingleNodeNonTargetsInfluence(int vertex) {
    return this->nonTargets[vertex];
}
