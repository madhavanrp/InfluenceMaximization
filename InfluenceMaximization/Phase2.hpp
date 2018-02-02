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
#include "TIMInfluenceCalculator.hpp"

#define EPSLON_TARGETS 2

class Phase2 {
protected:
    IMTree tree;
    Graph *graph;
    vector<vector<int>>* rrSets;
    TIMInfluenceCalculator *timInfluenceCalculator;
    vector<double> *nonTargetEstimates;
public:
    Phase2(Graph *graph);
    virtual struct node* addChild(struct node* parent, int childNodeID, double targets, double nonTargets);
    
    double getScalingFactorTargets();
    void doPhase2(int budget, int threshold, vector<double> nonTargetEstimates);
    virtual pair<int,int> findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet, double totalNonTargets, int nonTargetThreshold);
    void deleteUnexpandedNodes(vector<pair<struct node*, bool>> expandedNodes);
    IMTree* getTree();
};

class Phase2SIM: public Phase2 {
protected:
    
public:
    Phase2SIM(Graph *graph);
    virtual pair<int,int> findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet, double totalNonTargets, int nonTargetThreshold);
};

class Phase2TIM: public Phase2 {
public:
    Phase2TIM(Graph *graph);
    struct node* addChild(struct node* parent, int childNodeID, double targets, double nonTargets);
    pair<int,int> findMaxInfluentialNode(set<int> candidateNodes, vector<struct node*> seedSet, double totalNonTargets, int nonTargetThreshold);
    int addToSeed(int vertex, TIMCoverage *timCoverage);
    pair<int, int> findMaxInfluentialNode(set<int> candidateNodes, TIMCoverage *timCoverage, double totalNonTargets, int nonTargetThreshold);
};
#endif /* Phase2_hpp */
