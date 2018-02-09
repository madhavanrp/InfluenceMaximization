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
#include <set>
#include <math.h>
using namespace std;

struct QueueComparator {
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return a.second < b.second;
    }
};

class NodeChecker {
public:
    virtual bool isNodeValid(int nodeID)=0;
};

class TIMCoverage {
    int numberOfRRSetsCovered;
    int numberOfNodesToCover;
public:
    vector<bool> nodeMark;
    vector<bool> edgeMark;
    vector<int> coverage;
    vector<vector<int>> *lookupTable;
    int R;
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> queue;
    
    
    TIMCoverage(vector<vector<int>> *lookupTable);
    
    void decrementCountForVertex(int u, int setID);
    
    void incrementCountForVertex(int u, int randomRRSetID);
    
    int countForVertex(int u);
    int numberOfNewRRSetsCoveredByVertex(int vertex);
    
    vector<int> getRRSetsCoveredByVertex(int vertex);
    
    void offsetCoverage(int vertex, int offset) ;
    
    void initializeLookupTable(vector<vector<int>>* randomRRSets, int n) ;
    
    void initializeDataStructures(int R, int n) ;
    
    pair<int, double> findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets) ;
    pair<int, double> findMaxInfluentialNodeAndUpdateModel(vector<vector<int>> *rrSets, NodeChecker *nodeChecker);
    pair<int, double> findMaxInfluentialNodeWithNoUpdates(vector<vector<int>> *rrSets, NodeChecker *nodeChecker);
    
    pair<int, int> findMaxInfluentialNodeWithApproximations(set<int> *seedSet, vector<int> *approximationsScaled);
    
    set<pair<int, int>> findTopKNodesWithInfluence(int k, vector<vector<int>> *rrSets);
    
    set<int> findTopKNodes(int k, vector<vector<int>> *rrSets);
    
    void addToSeed(int vertex, vector<vector<int>> *rrSets);

    int findInfluence(set<int> seedSet, double scalingFactor);
    int getNumberOfRRSetsCovered();
    
    void setNumberOfNodesToCover(int number);
    
    TIMCoverage( const TIMCoverage &obj);
    ~TIMCoverage();
};


#endif /* IMTree_hpp */
