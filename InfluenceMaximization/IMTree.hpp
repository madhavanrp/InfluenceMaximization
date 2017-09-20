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
    int targets;
    int nonTargets;
    struct node *parent;
    vector<struct node*> children;
    int depth;
    TIMCoverage *coverage;
    ~node() {
        if(coverage!=NULL) {
            coverage->release();
            coverage=NULL;
        }
    }
};

class IMSeedSet {
    
public:
    IMSeedSet();
    set<int> seedSet;
    int targets;
    int nonTargets;
    
    set<int> getSeedSet();
    int getTargets();
    int getNonTargets();
    
};

class IMTree {
public:
    IMTree(void);
    struct node* root;
    vector<struct node*> tree;
    vector<struct node*> getLeafNodes(int depth);
    void printTree();
    vector<struct node*> findSeedSetInPath(struct node *aNode);
    struct node* addChild(struct node* parent, int childNode, int targets, int nonTargets);
    struct node* makeNode(struct node *parent, struct node *child, int nodeId, int targets, int nonTargets);
    pair<int,int> influenceAlongPath(struct node* leaf);
    void removeLeaf(struct node* leaf);
    void removeBranch(struct node* leaf);
    int getTotalNodes();
    vector<IMSeedSet> getAllSeeds(int depth);
    IMSeedSet getBestSeedSet(int depth);
};

#endif /* IMTree_hpp */
