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
using namespace std;


class TIMCoverage {
public:
    vector<bool> nodeMark;
    vector<bool> edgeMark;
    vector<int> coverage;
    vector<vector<int>> *lookupTable;
    TIMCoverage(vector<vector<int>> *lookupTable) {
        this->lookupTable = lookupTable;
    }
    TIMCoverage() {
        
    }
    
    void decrementCountForVertex(int u, int setID) {
        (*lookupTable)[u].erase(remove((*lookupTable)[u].begin(), (*lookupTable)[u].end(), setID), (*lookupTable)[u].end());
        
    }
    
    void incrementCountForVertex(int u, int randomRRSetID) {
        (*lookupTable)[u].push_back(randomRRSetID);
    }
    
    TIMCoverage createCopy() {
        vector<bool> nodeMarkCopy;
        vector<bool> edgeMarkCopy;
        vector<int> coverageCopy;
        TIMCoverage timCoverageCopy = TIMCoverage(lookupTable);
        
        for(bool x:nodeMark) {
            nodeMarkCopy.push_back(x);
        }
        for(bool x:edgeMark) {
            edgeMarkCopy.push_back(x);
        }
        for(int x:coverage) {
            coverageCopy.push_back(x);
        }
        
        timCoverageCopy.nodeMark = nodeMarkCopy;
        timCoverageCopy.edgeMark = edgeMarkCopy;
        timCoverageCopy.coverage = coverageCopy;
        return timCoverageCopy;
    }
};
#endif /* IMTree_hpp */
