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
using namespace std;

class TIMCoverage {
public:
    vector<bool> nodeMark;
    vector<bool> edgeMark;
    vector<int> coverage;
    vector<vector<int>> *lookupTable;
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
    
    TIMCoverage *createCopy() {
        vector<bool> nodeMarkCopy;
        vector<bool> edgeMarkCopy;
        vector<int> coverageCopy;
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
