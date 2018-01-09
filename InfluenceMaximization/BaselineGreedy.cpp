//
//  BaselineGreedy.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/10/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "BaselineGreedy.hpp"
#include <assert.h>
#include "log.h"

priority_queue<CELFNode, vector<CELFNode>, CelfComparator> *BaselineGreedy::getQueue() {
    return &this->queue;
}

vector<int> BaselineGreedy::getOrderedSeed() {
    return this->orderedSeed;
}

set<int> BaselineGreedy::findSeedSet(Graph *graph,int budget, int nonTargetThreshold) {
    performInitialDiffusion(graph, nonTargetThreshold);
    FILE_LOG(logDEBUG) << "\n Finished initial round of diffusion";
    FILE_LOG(logDEBUG) << "\n Queue size is " << queue.size();
    int k = 0;
    set<int> seedSet;
    pair<int, int> influence;
    int totalTargets = 0;
    int oldMarginalGainTargets = 0;
    while(k<budget) {
        if(queue.empty()) {
            FILE_LOG(logDEBUG) << "\n Number of nodes found: " << seedSet.size();
            break;
        }
        CELFNode topNode = queue.top();
        queue.pop();
        if(topNode.flag!=k) {
            oldMarginalGainTargets = topNode.marginalGainTargets;
            seedSet.insert(topNode.nodeID);
            influence = findInfluenceUsingDiffusion(graph, seedSet, NULL);
            seedSet.erase(topNode.nodeID);
            topNode.marginalGainTargets = influence.first - totalTargets;
            if(topNode.marginalGainTargets<0) topNode.marginalGainTargets = 0;
            topNode.totalNonTargets = influence.second;
            topNode.totalTargets = influence.first>=totalTargets?influence.first:totalTargets;
            topNode.flag = k;
            if(topNode.totalNonTargets<=nonTargetThreshold) {
                queue.push(topNode);
                FILE_LOG(logDEBUG) << "\n Updating node: " << topNode.nodeID << " from " << oldMarginalGainTargets << " to  " << topNode.marginalGainTargets;
                FILE_LOG(logDEBUG) << ". Non targets now: " << topNode.totalNonTargets;
            } else {
                FILE_LOG(logDEBUG) << "\n Discarding Node: " << topNode.nodeID << " as non targets with it is: " << topNode.totalNonTargets;
            }
            continue;
        }
        
        // The node is up to date here
        assert(topNode.totalNonTargets<=nonTargetThreshold);
        seedSet.insert(topNode.nodeID);
        orderedSeed.push_back(topNode.nodeID);
        totalTargets = topNode.totalTargets;
        FILE_LOG(logDEBUG)<< "\n Adding node with ID: " << topNode.nodeID;
        FILE_LOG(logDEBUG) << "\n Targets: " << totalTargets << " Non Targets: " << topNode.totalNonTargets;
        k++;
    }
    return seedSet;
}

void BaselineGreedy::performInitialDiffusion(Graph *graph, int nonTargetThreshold) {
    int n = graph->n;
    set<int> *seedSet = new set<int>;
    pair<int, int> influence;
    int maxInfluence = -1;
    int numberOfNodesEliminated = 0;
    for(int i=0;i<n;i++) {
        seedSet->insert(i);
        influence = findInfluenceUsingDiffusion(graph, *seedSet, NULL);
        seedSet->erase(i);
        assert(seedSet->size()==0);
        if(i%500==0){
            FILE_LOG(logDEBUG) << "\n Finished " << i << " nodes ";
        }
        if(influence.second>nonTargetThreshold) {
            numberOfNodesEliminated++;
            continue;
        }
        CELFNode celfNode;
        celfNode.nodeID = i;
        celfNode.marginalGainTargets = influence.first;
        celfNode.totalNonTargets = influence.second;
        celfNode.totalTargets = influence.first;
        celfNode.flag = 0;
        queue.push(celfNode);
        
    }
    FILE_LOG(logDEBUG) << "\n Number of nodes eliminated after initial diffusion: " << numberOfNodesEliminated;
    delete seedSet;
}
