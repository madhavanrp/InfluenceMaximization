#ifndef IMTreeTests_cpp
#define IMTreeTests_cpp

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../InfluenceMaximization/IMTree.hpp"



IMTree createTestIMTree() {
    IMTree imTree;
    struct node* root = imTree.root;
    imTree.addChild(root, 2, 4, 1);
    imTree.addChild(root, 4, 5, 10);
    imTree.addChild(root, 6, 5, 10);
    return imTree;
}

IMTree createTestIMTree(int depth) {
    IMTree imTree;
    struct node* root = imTree.root;
    int nodeID = 0;
    int numberOfNodesPerLevel = 5;
    for (int i = 0; i<depth; i++) {
        vector<struct node*> leafNodes = imTree.getLeafNodes(i);
        if(i==0) REQUIRE(leafNodes.size()==1);
        else {
            REQUIRE(leafNodes.size()==numberOfNodesPerLevel);
        }
        for(int j =0;j<numberOfNodesPerLevel;j++) {
            if(leafNodes.size()==1) {
                imTree.addChild(leafNodes[0], nodeID++, rand()%10, rand()%10);
            }
            else {
                imTree.addChild(leafNodes[j], nodeID++, rand()%10, rand()%10);
            }
            
        }
    }
    REQUIRE(nodeID==(depth*numberOfNodesPerLevel));
    return imTree;
}

IMTree createIMTreeWithVaryingBranchingFactor(int depth) {
    IMTree imTree;
    struct node* root = imTree.root;
    int nodeID = 0;
    int nodesInLevel;
    int maxBranchingFactor = 10;
    for (int i = 0; i<depth; i++) {
        //vary the branching factor
        nodesInLevel = rand() % maxBranchingFactor;
        vector<struct node*> leafNodes = imTree.getLeafNodes(i);
        if(i==0) REQUIRE(leafNodes.size()==1);
        for(int j =0;j<nodesInLevel;j++) {
            if(leafNodes.size()==1) {
                imTree.addChild(leafNodes[0], nodeID++, rand()%10, rand()%10);
            }
            else {
                imTree.addChild(leafNodes[j%leafNodes.size()], nodeID++, rand()%10, rand()%10);
            }
            
        }
    }
    REQUIRE(nodeID<=(depth*maxBranchingFactor));
    return imTree;
}

TEST_CASE("Creating Test IMTree With Depth", "IMTree") {
    int depth = 5;
    IMTree imTree = createTestIMTree(depth);
    vector<struct node*> leafNodes = imTree.getLeafNodes(depth);
    for(struct node* leaf:leafNodes) {
        vector<struct node*> seed = imTree.findSeedSetInPath(leaf);
        REQUIRE(seed.size()==depth);
    }
    
//    imTree = createIMTreeWithVaryingBranchingFactor(depth);
//    
//    leafNodes = imTree.getLeafNodes(depth);
//    for(struct node* leaf:leafNodes) {
//        vector<struct node*> seed = imTree.findSeedSetInPath(leaf);
//        REQUIRE(seed.size()==depth);
//    }
    
}

TEST_CASE( "Nodes are added", "IMTree Nodes" ) {
    IMTree imTree = createTestIMTree();
    for(struct node *aNode: imTree.tree) {
        if ((*aNode).parent==NULL) {
            REQUIRE(aNode-> nodeID == -1);
        } else {
            REQUIRE((*aNode).parent -> nodeID == -1);
        }
    }
}

TEST_CASE("Nodes are created with correct ID", "IMTree Node") {
    IMTree imTree;
    struct node* parent = imTree.root;
    int i =0;
    int depth = 1;
    while(i<10) {
        struct node* child = imTree.addChild(parent, i, 10, 10);
        
        int coin = rand() %2;
        if(coin==1) {
            assert(imTree.addChild(parent, 500, 10, 10)->depth==depth);
        }
        REQUIRE(child->depth==depth);
        parent = child;
        REQUIRE(parent->depth==depth);
        
        
        i++;
        depth++;
    }
}

TEST_CASE("Influence along a path", "IMTree Path" ) {
    IMTree imTree = createTestIMTree(10);
    struct node* parent = imTree.root;
    for (int i =0; i<10; i++) {
        parent = imTree.addChild(parent, i, 10, 5);
    }
    pair<int,int> influence = imTree.influenceAlongPath(parent);
    REQUIRE(influence.first==100);
    REQUIRE(influence.second==50);
}

TEST_CASE("Seed sets for a node", "IMTree Seed Sets") {
    IMTree imTree = createTestIMTree();
    struct node *leafNode = imTree.tree[imTree.tree.size()-1];
    vector<struct node*> seed = imTree.findSeedSetInPath(leafNode);
    REQUIRE(seed.size()==1);
    REQUIRE(seed[0]==leafNode);
}

TEST_CASE("Seed set along root node", "IMTree Seed Sets") {
    IMTree imTree = createTestIMTree();
    struct node *root = imTree.root;
    vector<struct node*> seed = imTree.findSeedSetInPath(root);
    REQUIRE(seed.size()==0);
}

TEST_CASE("Get proper leaf nodes", "IMTree leaf nodes") {
    IMTree imTree = createTestIMTree();
    vector<struct node*> leafNodes = imTree.getLeafNodes(1);
    REQUIRE(leafNodes.size()==3);
    imTree = *new IMTree();
    REQUIRE(imTree.getLeafNodes(0).size()==1);
}

void deleteHere(IMTree tree, struct node *leaf) {
    tree.removeLeaf(leaf);
}

TEST_CASE("Removing leaves from IM Tree", "IMTree leaf nodes") {
    int depth = 5;
    IMTree imTree = createTestIMTree(depth);
    vector<struct node*> leaves = imTree.getLeafNodes(depth);
    struct node* leaf = leaves[0];
    struct node* parent = leaf->parent;
    int leafID = leaf->nodeID;
    int oldChildrenCount = (int)parent->children.size();

    struct node* anotherPointer = leaf;
//    imTree.removeLeaf(anotherPointer);
    deleteHere(imTree, anotherPointer);
    int newChildrenCount = (int)parent->children.size();
    REQUIRE(oldChildrenCount==(newChildrenCount+1));
    
    bool leafDeleted = true;
    for(struct node* leaf: parent->children) {
        if(leaf->nodeID==leafID) {
            leafDeleted = false;
        }
    }
    REQUIRE(leafDeleted);
    
}


#endif
