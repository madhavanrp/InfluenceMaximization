//
//  IMTree.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/25/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "IMTree.hpp"

using namespace std;

IMTree::IMTree() {
    struct node *root = (struct node*)malloc(sizeof(struct node));
    root -> nodeID = -1;
    root -> parent = NULL;
    tree.push_back(root);
    cout << tree[0] -> nodeID;
}

void IMTree::addChild(int level, int node, int targets, int nonTargets) {
//    tree.push_back(make_pair(node, make_pair(targets, nonTargets)));
    struct node *child = (struct node*)malloc(sizeof(struct node));
    cout<< tree.size();
    cout << tree[0] -> nodeID;
    tree.push_back(makeNode(tree[0], child, node, targets, nonTargets));
    
}

struct node* IMTree:: makeNode(struct node *parent, struct node *child, int nodeId, int targets, int nonTargets) {
    cout<<"Parent node Id " << (*parent).nodeID;
    (*child).nodeID = nodeId;
    (*child).parent = parent;
    (*child).targets = targets;
    (*child).nonTargets = nonTargets;
    return child;
    
}

//vector<struct node*> IMTree:: findNodesAtLevel(int level) {
//    struct node *root = tree[0];
//    stack<struct node*> s;
//    s.push(root);
//    int depth = 0;
//    vector<struct node*> leafNodes;
//    while(!s.empty()) {
//        struct node *current = (struct node*)s.top();
//        s.pop();
//        
//        for(struct node *child:current->children) {
//            s.push(child);
//        }
//    }
//    
//    
//}
