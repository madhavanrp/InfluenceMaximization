//
//  BaselineGreedy.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/10/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef BaselineGreedy_hpp
#define BaselineGreedy_hpp

#include <stdio.h>
#include "Graph.hpp"
#include <set>
#include "Diffusion.hpp"

typedef struct CELFNode{
    int nodeID;
    int totalTargets;
    int totalNonTargets;
    int marginalGainTargets;
    int marginalGainNonTargets;
    int flag;
}CELFNode;

struct CelfComparator {
    bool operator()(CELFNode a, CELFNode b)
    {
        if(a.marginalGainTargets<b.marginalGainTargets) {
            return true;
        } else if(a.marginalGainTargets>b.marginalGainTargets) {
            return false;
        }
        else {
            return a.flag<b.flag;
        }
    }
};


class BaselineGreedy {
    vector<int> orderedSeed; // For greedy algorithm. 
    priority_queue<CELFNode, vector<CELFNode>, CelfComparator> queue;
    void performInitialDiffusion(Graph *graph, int nonTargetThreshold);
public:
    priority_queue<CELFNode, vector<CELFNode>, CelfComparator> *getQueue();
    vector<int> getOrderedSeed();
    set<int> findSeedSet(Graph *graph, int budget, int nonTargetThreshold);
};

#endif /* BaselineGreedy_hpp */
