//
//  Phase2.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/31/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "Phase2.hpp"



void Phase2::doSomething(vector<int> nodeCounts) {
    int threshold = 20;
    int budget = 20;
    IMTree tree;
    vector<vector<int>> nonTargetMap;
    for(int i=0;i<=threshold;i++) {
        nonTargetMap.push_back(vector<int>());
    }
    int totalNodesLessThanThreshold = 0;
    int numberOfNodes = (int)nodeCounts.size();
    
    for (int i=0; i<numberOfNodes; i++) {
        if(nodeCounts[i]<=threshold) {
            nonTargetMap[nodeCounts[i]].push_back(i);
            totalNodesLessThanThreshold++;
        }
    }
    
    int depth = 0;
    int totalTargets = -1;
    int totalNonTargets = -1;
    while(depth<budget) {
        vector<struct node*> leafNodes = tree.getLeafNodes(depth);
        pair<pair<int,int>, struct node*> nodesByNonTargetCount[threshold+1];
        
        for(struct node* leaf: leafNodes) {
            totalNonTargets = 0;
            totalTargets = 0;
            vector<struct node *> seedSet = tree.findSeedSetInPath(leaf);
            pair<int, int> influence = tree.influenceAlongPath(leaf);
            totalTargets = influence.first;
            totalNonTargets = influence.second;
            
            //Branch here
            for(int i=0;i<=(threshold-totalNonTargets);i++) {
                int nextNode = findMaxInfluentialNode(nonTargetMap[i], seedSet);
                if(nextNode==-1) continue;
                int targets = rand()%20;
                //The pruning happens here
                if(nodesByNonTargetCount[totalNonTargets+i].second==NULL) {
                    nodesByNonTargetCount[totalNonTargets+i] = make_pair(make_pair(nextNode, targets), leaf);
                } else {
                    if(nodesByNonTargetCount[totalNonTargets+i].first.second<targets) {
                        nodesByNonTargetCount[totalNonTargets+i] = make_pair(make_pair(nextNode, targets), leaf);
                        
                    }
                }
            }
        }
        
        
        //Go through nodes by non target and expand.
        for (int i = 0; i<(threshold+1); i++) {
            
            if(nodesByNonTargetCount[i].second==NULL) continue;
            
            int nodeID = nodesByNonTargetCount[i].first.first;
            int targets = nodesByNonTargetCount[i].first.second;
            tree.addChild(nodesByNonTargetCount[i].second, nodeID, targets, i);
        }
        depth++;
    }
    
    
}



int Phase2::findMaxInfluentialNode(vector<int> candidateNodes, vector<struct node*> seedSet) {
    int numberOfCandidateNodes = (int)candidateNodes.size();
    if(numberOfCandidateNodes==0) return -1;
    
    int nextNode = candidateNodes[rand()%numberOfCandidateNodes];
    return nextNode;
    
}
