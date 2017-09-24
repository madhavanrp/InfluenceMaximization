//
//  Phase2.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/31/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef Phase2_hpp
#define Phase2_hpp

#include <stdio.h>
#include "IMTree.hpp"
#include "Graph.hpp"
#include <queue>
#include <cmath>
#include <set>
#include "Diffusion.hpp"

#define EPSLON_TARGETS 2

class Phase2 {
protected:
    IMTree tree;
    Graph *graph;
    vector<vector<int>> rrSets;
public:
    Phase2(Graph *graph);
    virtual struct node* addChild(struct node* parent, int childNodeID, int targets, int nonTargets);
    void doPhase2(int budget, int threshold, vector<int> nonTargetEstimates);
    virtual pair<int,int> findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet);
    void deleteUnexpandedNodes(vector<pair<struct node*, bool>> expandedNodes);
    IMTree* getTree();
};

class Phase2SIM: public Phase2 {
protected:
    
public:
    Phase2SIM(Graph *graph);
    virtual pair<int,int> findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet);
};

class Phase2TIM: public Phase2 {
public:
    Phase2TIM(Graph *graph);
    struct node* addChild(struct node* parent, int childNodeID, int targets, int nonTargets);
    pair<int,int> findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet);
    void addToSeed(int vertex, TIMCoverage *timCoverage);
    pair<int, int> findMaxInfluentialNode(set<int> candidateNodes, TIMCoverage *timCoverage);
};
#endif /* Phase2_hpp */
