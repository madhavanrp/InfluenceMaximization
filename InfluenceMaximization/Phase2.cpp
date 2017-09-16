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
    
    int threshold = 10;
    int budget = 1;
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
                pair<int,int> nodePair = findMaxInfluentialNode(nonTargetMap[i], seedSet);
                int nextNode = nodePair.first;
                if(nextNode==-1) continue;
                int targets = nodePair.second;
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
    
    vector<struct node*> leafNodes = tree.getLeafNodes(depth);
    for(struct node* leaf: leafNodes) {
        pair<int, int> influence = tree.influenceAlongPath(leaf);
        cout << "\n Targets hit = " << influence.first << " Non Targets hit = " << influence.second;
        cout << "\n Seed set: ";
        vector<struct node*> seedSet = tree.findSeedSetInPath(leaf);
        for(struct node* seed:seedSet) {
            cout<< " " << seed->nodeID;
        }
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


pair<int,int> Phase2::findMaxInfluentialNode(vector<int> candidateNodes, vector<struct node*> seedSet) {
    int numberOfCandidateNodes = (int)candidateNodes.size();
    if(numberOfCandidateNodes==0) return make_pair(-1,0);
    
    int nextNode = candidateNodes[rand()%numberOfCandidateNodes];
    return make_pair(nextNode, rand()%20);
}

Phase2::Phase2(Graph graph) {
    this->graph = graph;
}

Phase2TIM::Phase2TIM(Graph graph): Phase2(graph) {
    int n = graph.n;
    double epsilon = EPSLON_TARGETS;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
//    R = 23648871;
    graph.generateRandomRRSets(R, true);
    rrSets = graph.getRandomRRSets();
    vector<vector<int>> *lookupTable = new vector<vector<int>>();
    TIMCoverage *coverage = new TIMCoverage(lookupTable);
    coverage->initializeLookupTable(rrSets, n);
    coverage->initializeDataStructures(R, n);
    int count = 0;
    for(vector<int> oneSet:rrSets) {
        for(int vertex:oneSet) {
            if(vertex==15228) count++;
        }
    }
    assert(count==coverage->countForVertex(15228));
    tree.root->coverage = coverage;
    
}
void Phase2TIM::addChild(struct node* parent, int childNodeID, int targets, int nonTargets) {
    struct node *newChild=NULL;
    if(parent->children.size()>0) {
        TIMCoverage *coverage = parent->coverage;
        assert(coverage!=NULL);
        TIMCoverage *coverageCopy = (*coverage).createCopy();
        assert(coverageCopy->retainCount==1);
        newChild = tree.addChild(parent, childNodeID, targets, nonTargets);
        newChild->coverage = coverageCopy;
        assert(newChild->coverage->retainCount==1);
        
    } else {
        assert(parent->coverage!=NULL);
        newChild = tree.addChild(parent, childNodeID, targets, nonTargets);
        newChild->coverage = parent->coverage;
        newChild->coverage->retain();
    }
    addToSeed(childNodeID, newChild->coverage);
}

bool vectorContains(vector<int> aVector, int a) {
    bool contains = false;
    for(int element:aVector) {
        if(a==element) {
            contains = true;
            break;
        }
    }
    return contains;
}

pair<int,int> Phase2TIM::findMaxInfluentialNode(vector<int> candidateNodes, vector<struct node*> seedSet) {
    
    int numberOfCandidateNodes = (int)candidateNodes.size();
    
    if(numberOfCandidateNodes==0) {
        return make_pair(-1,0);
    }
    
    // The first node in the vector should be the leaf. If it's empty, then take the root
    struct node* leaf;
    if(seedSet.size()>0) {
        leaf = seedSet[0];
    }
    else {
        leaf = tree.root;
    }
    TIMCoverage *timCoverage = leaf->coverage;
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queueCopy = new priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>(timCoverage->queue);
    
//    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *queue = &timCoverage->queue;
    
    vector<int> *coverage = &timCoverage->coverage;
    vector<bool> *nodeMark = &timCoverage->nodeMark;
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
        //Make sure the correct node is removed
        
        queueCopy->pop();
        if(!(*nodeMark)[element.first]) {
            continue;
        }
        if(vectorContains(candidateNodes, element.first)) {
            maximumGainNode = element.first;
            influence = (*coverage)[element.first];
            break;
        }
        
    }
    
    delete queueCopy;
    // TODO: Scale this.
//    double scaledInfluence = (double) influence * nodeMark->size()/(int)this->rrSets.size();
    return make_pair(maximumGainNode, influence);
}

void Phase2TIM::addToSeed(int vertex, TIMCoverage *timCoverage) {
    
    vector<int> *coverage = &timCoverage->coverage;
    vector<bool> *nodeMark = &timCoverage->nodeMark;
    vector<bool> *edgeMark = &timCoverage->edgeMark;
    (*nodeMark)[vertex] = false;
    int numberCovered = timCoverage->countForVertex(vertex);
    vector<int> edgeInfluence = (*timCoverage->lookupTable)[vertex];
    
    for (int i = 0; i < numberCovered; i++) {
        if ((*edgeMark)[edgeInfluence[i]]) continue;
        
        vector<int> nList = rrSets[edgeInfluence[i]];
        for (int l :
             nList) {
            if ((*nodeMark)[l]) {
                (*coverage)[l]--;
            }
        }
        (*edgeMark)[edgeInfluence[i]] = true;
    }
}
