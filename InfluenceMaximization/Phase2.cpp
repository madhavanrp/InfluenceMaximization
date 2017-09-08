//
//  Phase2.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/31/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "Phase2.hpp"
#include <assert.h>



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
        if(depth==0) {
            assert(leafNodes.size()==1);
        } else {
            assert(leafNodes.size()<=(threshold+1));
        }
        
        pair<pair<int,int>, struct node*> nodesByNonTargetCount[threshold+1];
        
        cout << "\n leaf nodes size " << leafNodes.size() << flush;
        
        bool toggle[threshold+1];
        for(int i=0;i<(threshold+1);i++) {
            toggle[i] = false;
            assert(nodesByNonTargetCount[i].second==NULL);
        }
        
        for(struct node* leaf: leafNodes) {
            totalNonTargets = 0;
            totalTargets = 0;
            cout <<"\n Starting find for leaf " << leaf->nodeID << flush;
            vector<struct node *> seedSet = tree.findSeedSetInPath(leaf);
            cout << "\n Finishing 1" << flush;
            pair<int, int> influence = tree.influenceAlongPath(leaf);
            totalTargets = influence.first;
            totalNonTargets = influence.second;
            
            //Branch here
            for(int i=0;i<=(threshold-totalNonTargets);i++) {
                int nextNode = findMaxInfluentialNode(nonTargetMap[i], seedSet);
                if(nextNode==-1) continue;
                int targets = rand()%20;
                assert(leaf!=NULL);
                //The pruning happens here
                if(nodesByNonTargetCount[totalNonTargets+i].second==NULL) {
                    assert(!toggle[totalNonTargets+i]);
                    nodesByNonTargetCount[totalNonTargets+i] = make_pair(make_pair(nextNode, targets), leaf);
                    toggle[totalNonTargets+i] = true;
                } else {
                    assert(toggle[totalNonTargets+i]);
                    if(nodesByNonTargetCount[totalNonTargets+i].first.second<targets) {
                        nodesByNonTargetCount[totalNonTargets+i] = make_pair(make_pair(nextNode, targets), leaf);
                        
                    }
                }
            }
            cout <<"\n Threshold = " <<threshold << " Non Targets = "<< totalNonTargets << " Difference= " << threshold-totalNonTargets;
            cout <<"\n Finishing 2" << flush;
        }
        
        
        //Go through nodes by non target and expand.
        cout <<" \n At depth " << depth;
        for (int i = 0; i<(threshold+1); i++) {
            
            if(nodesByNonTargetCount[i].second==NULL) continue;
            
            int nodeID = nodesByNonTargetCount[i].first.first;
            int targets = nodesByNonTargetCount[i].first.second;
            cout <<"\n For threshold " << i << " adding node " << nodeID << "  With targets" << targets;
            cout << " with parent  " << nodesByNonTargetCount[i].second->nodeID;
            struct node* newChild = tree.addChild(nodesByNonTargetCount[i].second, nodeID, targets, i);
            pair<int, int> leafInfluence = tree.influenceAlongPath(newChild);
            cout <<"\n Current node's non target should be " << i << " and total = " << leafInfluence.second;
        }
        depth++;
    }
    
    cout << "Leaf nodes size at final depth = " << tree.getLeafNodes(depth).size();
//    tree.printTree();
    
}



int Phase2::findMaxInfluentialNode(vector<int> candidateNodes, vector<struct node*> seedSet) {
    int numberOfCandidateNodes = (int)candidateNodes.size();
    if(numberOfCandidateNodes==0) return -1;
    
    int nextNode = candidateNodes[rand()%numberOfCandidateNodes];
    return nextNode;
    
}
