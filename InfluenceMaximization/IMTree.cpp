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
    struct node *root = new struct node();
    root -> nodeID = -1;
    root -> parent = NULL;
    root -> depth = 0;
    tree.push_back(root);
    this-> root = root;
}

struct node* IMTree::addChild(struct node* parent, int childNode, int targets, int nonTargets) {
    struct node *child = new struct node();
    struct node* newChild = makeNode(parent, child, childNode, targets, nonTargets);
    parent->children.push_back(newChild);
    tree.push_back(newChild);
    return newChild;
    
}

int IMTree::getTotalNodes() {
    vector<struct node*> leafNodes;
    deque<struct node*> someQueue;
    
    
    someQueue.push_back(root);
    while(!someQueue.empty()) {
        struct node *current = someQueue.front();
        someQueue.pop_front();
        for(struct node* child:current->children) {
            if(child->children.size()==0) {
                leafNodes.push_back(child);
            } else {
                leafNodes.push_back(child);
                someQueue.push_back(child);
            }
        }
    }
    return (int)leafNodes.size();
}
vector<struct node*> IMTree::getLeafNodes(int depth) {
    vector<struct node*> leafNodes;
    deque<struct node*> someQueue;
    
    //If the root is the leaf
    if(root->children.size()==0) {
        leafNodes.push_back(root);
        return leafNodes;
    }
    
    someQueue.push_back(root);
    while(!someQueue.empty()) {
        struct node *current = someQueue.front();
        someQueue.pop_front();
        for(struct node* child:current->children) {
            if(child->children.size()==0) {
                if(child->depth==depth) {
                    leafNodes.push_back(child);
                }
            } else {
                someQueue.push_back(child);
            }
        }
    }
    return leafNodes;
}

vector<struct node*> IMTree::findSeedSetInPath(struct node *aNode) {
    vector<struct node*> seedSets;
    if(aNode==root) return seedSets;
    seedSets.push_back(aNode);
    struct node* current = aNode;
    while(current->parent!=tree[0]) {
        seedSets.push_back(current->parent);
        current = current->parent;
    };
    return seedSets;
}

struct node* IMTree:: makeNode(struct node *parent, struct node *child, int nodeId, int targets, int nonTargets) {
    (*child).nodeID = nodeId;
    (*child).parent = parent;
    (*child).targets = targets;
    (*child).nonTargets = nonTargets;
    (*child).depth = (parent->depth) + 1;
    return child;
    
}

pair<int,int> IMTree::influenceAlongPath(struct node* leaf) {
    vector<struct node *> seedSet = findSeedSetInPath(leaf);
    int totalNonTargets = 0;
    int totalTargets = 0;
    // Find non targets and targets along this path.
    for (int i=0; i<seedSet.size(); i++) {
        struct node* vertex = seedSet[i];
        totalTargets+= vertex->targets;
        totalNonTargets+= vertex->nonTargets;
    }
    return make_pair(totalTargets, totalNonTargets);
}

void IMTree::removeLeaf(struct node* leaf) {
    vector<struct node*> *leaves = &(leaf->parent->children);
    (*leaves).erase(std::remove((*leaves).begin(), (*leaves).end(), leaf), (*leaves).end());
    delete leaf;
}

void IMTree::removeBranch(struct node* leaf) {
    struct node* parent = leaf->parent;
    vector<struct node*> *leaves = &(leaf->parent->children);
    (*leaves).erase(std::remove((*leaves).begin(), (*leaves).end(), leaf), (*leaves).end());
    delete leaf;
    
    //Recursively remove leaf node
    if(parent!=NULL && parent->children.size()==0) {
        removeBranch(parent);
    }
}
void IMTree::printTree() {
//    cout <<"\nBegin printing tree\n";
//    for(struct node *leaf: getLeafNodes()) {
//        cout<<"\n";
//        vector<struct node*> path = findSeedSetInPath(leaf);
//        for(struct node* vertex:path) {
//            cout << "\t:" << vertex->nodeID;
//        }
//        cout<<"\n";
//    }
}
