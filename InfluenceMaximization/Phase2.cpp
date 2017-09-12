//
//  Phase2.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/31/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "Phase2.hpp"
#include <assert.h>
int TIMCoverage::totalCount=0;
void Phase2::doSomething(vector<int> nodeCounts) {
    
    int threshold = 20;
    int budget = 20;
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
        vector<pair<struct node*, bool>> expandedNodes;
        
        for(struct node *leaf: leafNodes) {
            totalNonTargets = 0;
            totalTargets = 0;
            vector<struct node *> seedSet = tree.findSeedSetInPath(leaf);
            pair<int, int> influence = tree.influenceAlongPath(leaf);
            totalTargets = influence.first;
            totalNonTargets = influence.second;
            
            // Maintain expanded nodes if needed
            if(leaf!=tree.root) {
                expandedNodes.push_back(make_pair(leaf, false));
            }
            
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
            addChild(nodesByNonTargetCount[i].second, nodeID, targets, i);
            for(int j=0;j<expandedNodes.size();j++) {
                if(expandedNodes[j].first==nodesByNonTargetCount[i].second && !expandedNodes[j].second) {
                    expandedNodes[j].second=true;
                    break;
                }
            }
        }
        int toRemove = 0;
        for(pair<struct node*, bool> expandedNode:expandedNodes) {
            if(expandedNode.second==false && expandedNode.first!=NULL) {
                toRemove++;
                tree.removeBranch(expandedNode.first);
                assert(expandedNode.first->children.size()==0);
            }
        }
        depth++;
        
    }
}

void Phase2::deleteUnexpandedNodes(vector<pair<struct node*, bool>> expandedNodes) {
    cout<<"\n Contents of expanded node:";
    for(pair<struct node*, bool> expandedNode:expandedNodes) {
        cout << "\n";
        cout << expandedNode.first->nodeID << " ";
        cout << expandedNode.second;
        cout <<flush;
    }
    for(pair<struct node*, bool> expandedNode:expandedNodes) {
        if(expandedNode.second==false && expandedNode.first!=NULL) {
            cout<<"\n Removing leaf" << expandedNode.first->nodeID << flush;
            tree.removeLeaf(expandedNode.first);
        }
    }
}

void Phase2::addChild(struct node* parent, int childNodeID, int targets, int nonTargets) {
    tree.addChild(parent, childNodeID, targets, nonTargets);
}


int Phase2::findMaxInfluentialNode(vector<int> candidateNodes, vector<struct node*> seedSet) {
    int numberOfCandidateNodes = (int)candidateNodes.size();
    if(numberOfCandidateNodes==0) return -1;
    
    int nextNode = candidateNodes[rand()%numberOfCandidateNodes];
    return nextNode;
}

Phase2::Phase2(Graph graph) {
    this->graph = graph;
}

Phase2TIM::Phase2TIM(Graph graph): Phase2(graph) {
    vector<vector<int>> randomRRSets = graph.getRandomRRSets();
    TIMCoverage *coverage = new TIMCoverage(&randomRRSets);
    tree.root->coverage = coverage;
    
}
void Phase2TIM::addChild(struct node* parent, int childNodeID, int targets, int nonTargets) {
    if(parent->children.size()>0) {
        TIMCoverage *coverage = parent->coverage;
        assert(coverage!=NULL);
        TIMCoverage *coverageCopy = (*coverage).createCopy();
        assert(coverageCopy->nodeMark.size()==0);
        assert(coverageCopy->retainCount==1);
        struct node *newChild = tree.addChild(parent, childNodeID, targets, nonTargets);
        newChild->coverage = coverageCopy;
        assert(newChild->coverage->retainCount==1);
        
    } else {
        assert(parent->coverage!=NULL);
        struct node *newChild = tree.addChild(parent, childNodeID, targets, nonTargets);
        newChild->coverage = parent->coverage;
        newChild->coverage->retain();
    }
}
