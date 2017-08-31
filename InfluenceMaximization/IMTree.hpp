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

#endif /* IMTree_hpp */
using namespace std;
struct node {
    int nodeID;
    int targets;
    int nonTargets;
    struct node *parent;
    vector<struct node*> children;
};
class IMTree {
public:
    IMTree(void);
    vector<struct node*> tree;
    void addChild(int level, int node, int targets, int nonTargets);
    struct node* makeNode(struct node *parent, struct node *child, int nodeId, int targets, int nonTargets);
};
