//
//  IMTree.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/25/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef IMTree_hpp
#define IMTree_hpp

#include <stdio.h>
#include <iostream>
#include<deque>
#include <vector>
#include <stack>
#include <deque>
#include<algorithm>
#include <set>
#include "TIM-Models/TIMCoverage.hpp"
#include <limits.h>

using namespace std;
struct node {
    int nodeID;
    double targets;
    double nonTargets;
    struct node *parent;
    vector<struct node*> children;
    int depth;
    TIMCoverage *coverage;
};

class IMSeedSet {
    vector<int> seedVector;
    
public:
    IMSeedSet();
    set<int> seedSet;
    int targets;
    int nonTargets;
    
    set<int> getSeedSet();
    vector<int> getSeedVector();
    int getTargets();
    int getNonTargets();
    
    void setSeedSet(set<int> seedSet);
    void setSeedVector(vector<int> seedVector);
    
    void setTargets(int targets);
    void setNonTargets(int nonTargets);
    
    void addSeed(int nodeID);
};

class IMTree {
private:
    vector<struct node*> tree;
    struct node* root;
    void recursivelyDeleteLeaves(struct node* leaf);
public:
    IMTree(void);
    IMTree( const IMTree &obj);
    ~IMTree();
    vector<struct node*> getLeafNodes(int depth);
    vector<struct node*> findSeedSetInPath(struct node *aNode);
    struct node* addChild(struct node* parent, int childNode, double targets, double nonTargets);
    struct node* makeNode(struct node *parent, struct node *child, int nodeId, double targets, double nonTargets);
    pair<double,double> influenceAlongPath(struct node* leaf);
    void removeLeaf(struct node* leaf);
    void removeBranch(struct node* leaf);
    int getTotalNodes();
    vector<IMSeedSet> getAllSeeds(int depth);
    IMSeedSet getBestSeedSet(int depth);
    
    vector<struct node*> getTree();
    struct node *getRoot();
};

#endif /* IMTree_hpp */
