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
        assert(n<R);
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
//            cout << "\n Vertex " << i << " appears in " << numberCovered << " RR Sets";
            queue.push(make_pair(i, numberCovered));
            coverage[i] = numberCovered;
            nodeMark[i] = true;
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
