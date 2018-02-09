//
//  TIMCoverage.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 1/16/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include "TIMCoverage.hpp"
TIMCoverage::TIMCoverage(vector<vector<int>> *lookupTable) {
    this->lookupTable = lookupTable;
    this->numberOfRRSetsCovered = 0;
}

void TIMCoverage::decrementCountForVertex(int u, int setID) {
    (*lookupTable)[u].erase(std::remove((*lookupTable)[u].begin(), (*lookupTable)[u].end(), setID), (*lookupTable)[u].end());
}

void TIMCoverage::incrementCountForVertex(int u, int randomRRSetID) {
    (*lookupTable)[u].push_back(randomRRSetID);
}

int TIMCoverage::countForVertex(int u) {
    assert(u<this->lookupTable->size());
    return (int)(*lookupTable)[u].size();
}

vector<int> TIMCoverage::getRRSetsCoveredByVertex(int vertex) {
    return (*lookupTable)[vertex];
}

int TIMCoverage::numberOfNewRRSetsCoveredByVertex(int vertex) {
    return this->coverage[vertex];
}

void TIMCoverage::offsetCoverage(int vertex, int offset) {
    this->coverage[vertex] = this->coverage[vertex] + offset;
}

void TIMCoverage::initializeLookupTable(vector<vector<int>>* randomRRSets, int n) {
    
    for(int i=0;i<n; i++) {
        (*lookupTable).push_back(vector<int>());
    }
    
    for(int rrSetID=0; rrSetID<randomRRSets->size();rrSetID++) {
        
        for(int vertex: (*randomRRSets)[rrSetID]) {
            incrementCountForVertex(vertex, rrSetID);
        }
    }
}

void TIMCoverage::initializeDataStructures(int R, int n) {
    this->numberOfNodesToCover = n;
    int numberCovered;
    for (int i = 0; i < n; i++) {
        nodeMark.push_back(false);
        coverage.push_back(0);
    }
    for (int i = 0; i < R; i++) {
        edgeMark.push_back(false);
    }
    for (int i = 0; i < n; i++) {
        
        numberCovered = this->countForVertex(i);
        queue.push(make_pair(i, numberCovered));
        coverage[i] = numberCovered;
        nodeMark[i] = true;
    }
    this->R = R;
    
}

pair<int, double> TIMCoverage::findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets, NodeChecker *nodeChecker) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queue = &this->queue;
    
    vector<int> *coverage = &this->coverage;
    vector<bool> *nodeMark = &this->nodeMark;
    int maximumGainNode = -1;
    int influence = 0;
    while(!queue->empty()) {
        pair<int,int> element = queue->top();
        if(element.second > (*coverage)[element.first]) {
            queue->pop();
            element.second = (*coverage)[element.first];
            queue->push(element);
            continue;
        }
        
        queue->pop();
        if (nodeChecker!=NULL && !nodeChecker->isNodeValid(element.first)) {
            continue;
        }
        if(!(*nodeMark)[element.first]) {
            continue;
        }
        
        maximumGainNode = element.first;
        influence = (*coverage)[element.first];
        break;
    }
    
    double scalingFactor = (double)this->numberOfNodesToCover/rrSets->size();
    double scaledInfluence = (double) influence * scalingFactor;
    
    vector<bool> *edgeMark = &this->edgeMark;
    (*nodeMark)[maximumGainNode] = false;
    int numberCovered = this->countForVertex(maximumGainNode);
    vector<int> edgeInfluence = (*this->lookupTable)[maximumGainNode];
    
    for (int i = 0; i < numberCovered; i++) {
        if ((*edgeMark)[edgeInfluence[i]]) continue;
        
        vector<int> nList = (*rrSets)[edgeInfluence[i]];
        for (int l :
             nList) {
            if ((*nodeMark)[l]) {
                (*coverage)[l]--;
            }
        }
        (*edgeMark)[edgeInfluence[i]] = true;
        this->numberOfRRSetsCovered++;
    }
    
    return make_pair(maximumGainNode, scaledInfluence);
}


pair<int, double> TIMCoverage::findMaxInfluentialNodeWithNoUpdates(vector<vector<int>> *rrSets, NodeChecker *nodeChecker) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queueCopy = new priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>(this->queue);
    
    vector<int> *coverage = &this->coverage;
    vector<bool> *nodeMark = &this->nodeMark;
    int maximumGainNode = -1;
    int influence = 0;
    while(!queueCopy->empty()) {
        pair<int,int> element = queueCopy->top();
        if(element.second > (*coverage)[element.first]) {
            queueCopy->pop();
            element.second = (*coverage)[element.first];
            queueCopy->push(element);
            continue;
        }
        
        queueCopy->pop();
        if (nodeChecker!=NULL && !nodeChecker->isNodeValid(element.first)) {
            continue;
        }
        if(!(*nodeMark)[element.first]) {
            continue;
        }
        
        maximumGainNode = element.first;
        influence = (*coverage)[element.first];
        break;
    }
    delete queueCopy;
    double scalingFactor = (double)this->numberOfNodesToCover/rrSets->size();
    double scaledInfluence = (double) influence * scalingFactor;
    return make_pair(maximumGainNode, scaledInfluence);
}

pair<int, double> TIMCoverage::findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets) {
    return findMaxInfluentialNodeAndUpdateModel(rrSets, NULL);
}

pair<int, int> TIMCoverage::findMaxInfluentialNodeWithApproximations(set<int> *seedSet, vector<int> *approximationsScaled) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queueCopy = new priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>(this->queue);
    int maxValue = -1;
    int maximumGainNode = -1;
    while(!queueCopy->empty()) {
        pair<int, int> topElement = queueCopy->top();
        queueCopy->pop();
        int scaledApproximation = (*approximationsScaled)[topElement.first];
        if(this->coverage[topElement.first] - scaledApproximation > maxValue) {
            if(seedSet->find(topElement.first)==seedSet->end()) {
                maxValue = this->coverage[topElement.first] - scaledApproximation;
                maximumGainNode = topElement.first;
            }
        }
    }
    
    int R = this->R;
    double scaledInfluence = (double) maxValue * this->nodeMark.size()/R;
    delete queueCopy;
    
    return make_pair(maximumGainNode, scaledInfluence);
    
}

set<pair<int, int>> TIMCoverage::findTopKNodesWithInfluence(int k, vector<vector<int>> *rrSets) {
    set<pair<int, int>> topKNodesWithInfluence;
    for(int i=0; i< k; i++) {
        topKNodesWithInfluence.insert(findMaxInfluentialNodeAndUpdateModel(rrSets));
    }
    return topKNodesWithInfluence;
}

set<int> TIMCoverage::findTopKNodes(int k, vector<vector<int>> *rrSets) {
    set<int> topKNodes;
    for(int i=0; i< k; i++) {
        topKNodes.insert(findMaxInfluentialNodeAndUpdateModel(rrSets).first);
    }
    return topKNodes;
}

void TIMCoverage::addToSeed(int vertex, vector<vector<int>> *rrSets) {
    
    vector<int> *coverage = &this->coverage;
    vector<bool> *nodeMark = &this->nodeMark;
    vector<bool> *edgeMark = &this->edgeMark;
    (*nodeMark)[vertex] = false;
    int numberCovered = this->countForVertex(vertex);
    vector<int> edgeInfluence = (*this->lookupTable)[vertex];
    
    for (int i = 0; i < numberCovered; i++) {
        if ((*edgeMark)[edgeInfluence[i]]) continue;
        
        vector<int> nList = (*rrSets)[edgeInfluence[i]];
        for (int l :
             nList) {
            if ((*nodeMark)[l]) {
                (*coverage)[l]--;
            }
        }
        (*edgeMark)[edgeInfluence[i]] = true;
        this->numberOfRRSetsCovered++;
    }
}

int TIMCoverage::findInfluence(set<int> seedSet, double scalingFactor) {
    // Should not update anything
    // For each vertex in seed set:
    //  Go to each rr set covered. Mark edgeMark[rrSet Covered] as true
    // Go through edgeMark and find number of trues. Scale it as per scaling factor and return influence.
    vector<int> edgeMarkTemporary(this->edgeMark.size(), false);
    int rrSetsCovered = 0;
    for(int seed:seedSet) {
        int numberCovered = this->countForVertex(seed);
        vector<int> edgeInfluence = (*this->lookupTable)[seed];
        for (int rrSetID: edgeInfluence) {
            if(edgeMarkTemporary[rrSetID]) continue;
            edgeMarkTemporary[rrSetID] = true;
            rrSetsCovered++;
        }
    }
    return round((double)rrSetsCovered * scalingFactor);
}

int TIMCoverage::getNumberOfRRSetsCovered() {
//    int count=0;
//    for(bool edge: edgeMark) {
//        if(edge) count++;
//    }
//    assert(count==this->numberOfRRSetsCovered);
    return this->numberOfRRSetsCovered;
}
    
void TIMCoverage::setNumberOfNodesToCover(int number) {
    this->numberOfNodesToCover = number;
}

TIMCoverage::TIMCoverage( const TIMCoverage &obj) {
    queue = obj.queue;
    lookupTable = obj.lookupTable;
    R = obj.R;
    numberOfRRSetsCovered = obj.numberOfRRSetsCovered;
    for(bool x: obj.nodeMark) {
        nodeMark.push_back(x);
    }
    
    for(bool x: obj.edgeMark) {
        edgeMark.push_back(x);
    }
    for(int x: obj.coverage) {
        coverage.push_back(x);
    }
}

TIMCoverage::~TIMCoverage() {
    this->lookupTable = NULL;
}
