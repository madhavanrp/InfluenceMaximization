//
//  Phase2.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/31/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "Phase2.hpp"
#include <assert.h>
#include "log.h"
void Phase2::doPhase2(int budget, int threshold, vector<double> nonTargetEstimates) {
    this->nonTargetEstimates = &nonTargetEstimates;
    vector<set<int>> nonTargetMap;
    for(int i=0;i<=threshold;i++) {
        nonTargetMap.push_back(set<int>());
    }
    int totalNodesLessThanThreshold = 0;
    int numberOfNodes = (int)nonTargetEstimates.size();
    
    for (int i=0; i<numberOfNodes; i++) {
        if(nonTargetEstimates[i]<=threshold) {
            
            nonTargetMap[round(nonTargetEstimates[i])].insert(i);
            totalNodesLessThanThreshold++;
        }
    }
    
    int depth = 0;
    double totalTargets = -1;
    double totalNonTargets = -1;
    while(depth<budget) {
        FILE_LOG(logDEBUG) << "Starting depth " << depth;
        vector<struct node*> leafNodes = tree.getLeafNodes(depth);
        pair<pair<int,pair<int,int>>, struct node*> nodesByNonTargetCount[threshold+1];
        vector<pair<struct node*, bool>> expandedNodes;
        
        for(struct node *leaf: leafNodes) {
            totalNonTargets = 0;
            totalTargets = 0;
            vector<struct node *> seedSet = tree.findSeedSetInPath(leaf);
            pair<double, double> influence = tree.influenceAlongPath(leaf);
            totalTargets = influence.first;
            totalNonTargets = influence.second;
            assert(totalNonTargets<=threshold);
            
            // Maintain expanded nodes if needed
            if(leaf!=tree.getRoot()) {
                expandedNodes.push_back(make_pair(leaf, false));
            }
            
            //Branch here
            for(int i=0;i<=(threshold-totalNonTargets);i++) {
                pair<int, double> nodePair = findMaxInfluentialNode(nonTargetMap[i], seedSet, totalNonTargets, threshold);
                int nextNode = nodePair.first;
                if(nextNode==-1) continue;
                double targets = nodePair.second;
                //The pruning happens here
                
                assert(totalNonTargets+i<=threshold);
//                assert((*this->nonTargetEstimates)[nextNode]<=i);
                pair<int,int> nodeInfluence = make_pair(targets, i);
                if(nodesByNonTargetCount[(int)round(totalNonTargets)+i].second==NULL) {
                    nodesByNonTargetCount[(int)round(totalNonTargets)+i] = make_pair(make_pair(nextNode, nodeInfluence), leaf);
                } else {
                    struct node* currentNodeParent = nodesByNonTargetCount[(int)round(totalNonTargets)+i].second;
                    pair<double,double> currentTotalInfluence = tree.influenceAlongPath(currentNodeParent);
//                    assert(currentTotalInfluence.second + nodesByNonTargetCount[(int)round(totalNonTargets)+i].first.second.second<=totalNonTargets+i);
                    
                    if(nodesByNonTargetCount[(int)round(totalNonTargets)+i].first.second.first + currentTotalInfluence.first < targets + totalTargets) {
                        nodesByNonTargetCount[(int)round(totalNonTargets)+i] = make_pair(make_pair(nextNode, nodeInfluence), leaf);
                        int targetsHitByNode = round(leaf->coverage->getNumberOfRRSetsCovered() * getScalingFactorTargets());

                        
                    }
                }
            }
        }
        
        
        
        //Go through nodes by non target and expand.
        for (int i = 0; i<(threshold+1); i++) {
            
            if(nodesByNonTargetCount[i].second==NULL) continue;
            
            int nodeID = nodesByNonTargetCount[i].first.first;
            int targets = nodesByNonTargetCount[i].first.second.first;
            double nonTargets = (*this->nonTargetEstimates)[nodeID];
            
            FILE_LOG(logDEBUG) << "\n Adding child node with  " << targets << " targets and " << nonTargets << " Non Targets" << " and child ID is " << nodeID << " Non target count: " << i;
            
            struct node* newChild = addChild(nodesByNonTargetCount[i].second, nodeID, targets, nonTargets);
            
            
//            assert((int)round(tree.influenceAlongPath(newChild).second)==i);
            for(int j=0;j<expandedNodes.size();j++) {
                if(expandedNodes[j].first==nodesByNonTargetCount[i].second && !expandedNodes[j].second) {
                    expandedNodes[j].second=true;
                    break;
                }
            }
            pair<double, double> influenceAlongNewChild = tree.influenceAlongPath(newChild);
            FILE_LOG(logDEBUG) << "\n Influence Along path T, NT : " << influenceAlongNewChild.first << " , " << influenceAlongNewChild.second;
            assert(influenceAlongNewChild.second<=threshold);
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
        FILE_LOG(logDEBUG) << "Finishing depth " << depth;
    }
}

IMTree* Phase2::getTree() {
    return &tree;
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

struct node* Phase2::addChild(struct node* parent, int childNodeID, double targets, double nonTargets) {
    struct node* newChild = tree.addChild(parent, childNodeID, targets, nonTargets);
    return newChild;
}

double Phase2::getScalingFactorTargets() {
    return (double)graph->n/(double)rrSets->size();
}

pair<int,int> Phase2::findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet, double totalNonTargets, int nonTargetThreshold) {
    assert(false);
    int numberOfCandidateNodes = (int)candidateNodes.size();
    if(numberOfCandidateNodes==0) return make_pair(-1,0);
    
    int nextNode = rand()% numberOfCandidateNodes;
    return make_pair(nextNode, rand()%20);
}

Phase2::Phase2(Graph *graph) {
    this->graph = graph;
}


Phase2SIM::Phase2SIM(Graph *graph): Phase2(graph) {
    
}

Phase2TIM::Phase2TIM(Graph *graph): Phase2(graph) {
    int n = graph->n;
    double epsilon = EPSLON_TARGETS;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
//    R = 23648871;
    graph->generateRandomRRSets(R, true);
    rrSets = graph->getRandomRRSets();
    vector<vector<int>> *lookupTable = new vector<vector<int>>();
    TIMCoverage *coverage = new TIMCoverage(lookupTable);
    coverage->initializeLookupTable(rrSets, n);
    coverage->initializeDataStructures(R, n);
    
    tree.getRoot()->coverage = coverage;
    
}
struct node* Phase2TIM::addChild(struct node* parent, int childNodeID, double targets, double nonTargets) {
    struct node *newChild=NULL;
    if(parent->children.size()>0) {
        TIMCoverage *coverage = parent->coverage;
        assert(coverage!=NULL);
        TIMCoverage *coverageCopy = new TIMCoverage(*coverage);
        newChild = tree.addChild(parent, childNodeID, targets, nonTargets);
        newChild->coverage = coverageCopy;
        
    } else {
        assert(parent->coverage!=NULL);
        newChild = tree.addChild(parent, childNodeID, targets, nonTargets);
        newChild->coverage = new TIMCoverage(*parent->coverage);
    }
    addToSeed(childNodeID, newChild->coverage);
    
    return newChild;
}

pair<int, int> Phase2TIM::findMaxInfluentialNode(set<int> candidateNodes, TIMCoverage *timCoverage, double totalNonTargets, int nonTargetThreshold) {
    int originalSize = (int)timCoverage->queue.size();
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
        if(candidateNodes.find(element.first)!=candidateNodes.end()) {
            // Also make sure that it does not go over threhold
            if ((*this->nonTargetEstimates)[element.first] + totalNonTargets <= nonTargetThreshold) {
                maximumGainNode = element.first;
                influence = (*coverage)[element.first];
                break;
            }
        }
        
    }
    
    delete queueCopy;
    assert(timCoverage->queue.size()==originalSize);
    if(maximumGainNode!=-1) {
        assert(candidateNodes.find(maximumGainNode)!=candidateNodes.end());
    }
    double scaledInfluence = (double) influence * (double)nodeMark->size()/(double)this->rrSets->size();
    return make_pair(maximumGainNode, round(scaledInfluence));
}
pair<int,int> Phase2TIM::findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet, double totalNonTargets, int nonTargetThreshold) {
    
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
        leaf = tree.getRoot();
    }
    TIMCoverage *timCoverage = leaf->coverage;
    pair<int, int> maxNodeAndMarginalInfluence = findMaxInfluentialNode(candidateNodes, timCoverage, totalNonTargets, nonTargetThreshold);
    return maxNodeAndMarginalInfluence;
}

int Phase2TIM::addToSeed(int vertex, TIMCoverage *timCoverage) {
    
    vector<int> *coverage = &timCoverage->coverage;
    vector<bool> *nodeMark = &timCoverage->nodeMark;
    vector<bool> *edgeMark = &timCoverage->edgeMark;
    (*nodeMark)[vertex] = false;
    int numberCovered = timCoverage->countForVertex(vertex);
    vector<int> edgeInfluence = (*timCoverage->lookupTable)[vertex];
    int numberOfNewRRSetsCovered = 0;
    for (int i = 0; i < numberCovered; i++) {
        if ((*edgeMark)[edgeInfluence[i]]) continue;
        
        vector<int> nList = (*rrSets)[edgeInfluence[i]];
        for (int l :
             nList) {
            if ((*nodeMark)[l]) {
                (*coverage)[l]--;
            }
        }
        (*edgeMark)[edgeInfluence[i]] = true;
        numberOfNewRRSetsCovered++;
    }
    return numberOfNewRRSetsCovered;
}

pair<int,int> Phase2SIM::findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet, double totalNonTargets, int nonTargetThreshold) {
    set<int> seeds;
    for (struct node *seed:seedSet) {
        seeds.insert(seed->nodeID);
    }
    
    pair<pair<int,int>, set<int>> currentActiveSetInfluence = findActivatedSetAndInfluenceUsingDiffusion(graph, seeds, NULL);
    int maxInfluentialNode = -1;
    int maxMarginalGain = INT_MIN;
    for(int candidateNode: candidateNodes) {
        if (seeds.find(candidateNode)==seeds.end()) {
            seeds.insert(candidateNode);
            pair<pair<int, int>, set<int>> newSpreadAndInfluence = findActivatedSetAndInfluenceUsingDiffusion(graph, seeds, &currentActiveSetInfluence.second);
            
            int marginalInfluenceSpread = newSpreadAndInfluence.first.first - currentActiveSetInfluence.first.first;
            if (marginalInfluenceSpread > maxMarginalGain) {
                maxMarginalGain = marginalInfluenceSpread;
                maxInfluentialNode = candidateNode;
            }
            seeds.erase(candidateNode);
        }
    }
    return make_pair(maxInfluentialNode, maxMarginalGain);
}

