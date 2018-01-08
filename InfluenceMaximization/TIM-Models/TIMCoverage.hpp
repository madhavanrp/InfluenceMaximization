//
//  TIMCoverage.c
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 9/8/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//


#ifndef TIMCoverage_c
#define TIMCoverage_c

#include <stdio.h>
#include <algorithm>
#include <assert.h>
#include <queue>
using namespace std;

struct QueueComparator {
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return a.second < b.second;
    }
};

class TIMCoverage {
public:
    vector<bool> nodeMark;
    vector<bool> edgeMark;
    vector<int> coverage;
    vector<vector<int>> *lookupTable;
    int R;
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> queue;
    int retainCount = 0;
    
    static int totalCount;
    
    TIMCoverage(vector<vector<int>> *lookupTable) {
        this->lookupTable = lookupTable;
        retainCount++;
        assert(retainCount==1);
        TIMCoverage::totalCount++;
    }
    
    void decrementCountForVertex(int u, int setID) {
        (*lookupTable)[u].erase(std::remove((*lookupTable)[u].begin(), (*lookupTable)[u].end(), setID), (*lookupTable)[u].end());
    }
    
    void incrementCountForVertex(int u, int randomRRSetID) {
        (*lookupTable)[u].push_back(randomRRSetID);
    }
    
    int countForVertex(int u) {
        assert(u<this->lookupTable->size());
        return (int)(*lookupTable)[u].size();
    }
    
    vector<int> getRRSetsCoveredByVertex(int vertex) {
        return (*lookupTable)[vertex];
    }
    
    void offsetCoverage(int vertex, int offset) {
        this->coverage[vertex] = this->coverage[vertex] + offset;
    }
    
    void initializeLookupTable(vector<vector<int>> randomRRSets, int n) {
        
        for(int i=0;i<n; i++) {
            (*lookupTable).push_back(vector<int>());
        }
        
        for(int rrSetID=0; rrSetID<randomRRSets.size();rrSetID++) {
            
            for(int vertex: randomRRSets[rrSetID]) {
                incrementCountForVertex(vertex, rrSetID);
            }
        }
    }
    
    void initializeDataStructures(int R, int n) {
        int numberCovered;
        for (int i = 0; i < n; i++) {
            nodeMark.push_back(false);
            coverage.push_back(0);
        }
        assert(nodeMark.size()==n);
        assert(coverage.size()==n);
        for (int i = 0; i < R; i++) {
            edgeMark.push_back(false);
        }
        assert(edgeMark.size()==R);
        for (int i = 0; i < n; i++) {
            
            numberCovered = this->countForVertex(i);
            queue.push(make_pair(i, numberCovered));
            coverage[i] = numberCovered;
            nodeMark[i] = true;
        }
        this->R = R;
        
    }
    
    pair<int, int> findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets) {
        priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queueCopy = &this->queue;
        
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
            if(!(*nodeMark)[element.first]) {
                continue;
            }
            
            maximumGainNode = element.first;
            influence = (*coverage)[element.first];
            break;
        }
        
        int R = this->R;
        double scaledInfluence = (double) influence * nodeMark->size()/R;
        
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
        }
        
        return make_pair(maximumGainNode, scaledInfluence);
    }
    
    pair<int, int> findMaxInfluentialNodeWithApproximations(set<int> *seedSet, vector<int> *approximationsScaled) {
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
    
    set<pair<int, int>> findTopKNodesWithInfluence(int k, vector<vector<int>> *rrSets) {
        set<pair<int, int>> topKNodesWithInfluence;
        for(int i=0; i< k; i++) {
            topKNodesWithInfluence.insert(findMaxInfluentialNodeAndUpdateModel(rrSets));
        }
        return topKNodesWithInfluence;
    }
    
    set<int> findTopKNodes(int k, vector<vector<int>> *rrSets) {
        set<int> topKNodes;
        for(int i=0; i< k; i++) {
            topKNodes.insert(findMaxInfluentialNodeAndUpdateModel(rrSets).first);
        }
        return topKNodes;
    }
    
    void addToSeed(int vertex, vector<vector<int>> *rrSets) {
        
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
        }
    }

    
    TIMCoverage *createCopy() {
        vector<bool> nodeMarkCopy;
        vector<bool> edgeMarkCopy;
        vector<int> coverageCopy;
        priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> queueCopy = queue;
        TIMCoverage *timCoverageCopy = new TIMCoverage(lookupTable);
        
        for(bool x:nodeMark) {
            nodeMarkCopy.push_back(x);
        }
        
        for(bool x:edgeMark) {
            edgeMarkCopy.push_back(x);
        }
        for(int x:coverage) {
            coverageCopy.push_back(x);
        }
        timCoverageCopy->nodeMark = nodeMarkCopy;
        timCoverageCopy->edgeMark = edgeMarkCopy;
        timCoverageCopy->coverage = coverageCopy;
        timCoverageCopy->queue = queueCopy;
        return timCoverageCopy;
    }
    
    void release() {
        retainCount--;
        if(retainCount==0) {
            TIMCoverage::totalCount--;
            delete this;
        }
    }
    
    void retain() {
        retainCount++;
    }
};


#endif /* IMTree_hpp */
