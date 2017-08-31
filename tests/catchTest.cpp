#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../InfluenceMaximization/IMTree.hpp"

TEST_CASE( "Nodes are added", "IMTree Nodes" ) {
    IMTree imTree;
    imTree.addChild(0, 2, 4, 1);
    imTree.addChild(0, 4, 5, 10);
    imTree.addChild(0, 6, 5, 10);
    for(struct node *aNode: imTree.tree) {
        cout<< "Node ID: " << aNode -> nodeID;
        if ((*aNode).parent==NULL) {
            REQUIRE(aNode-> nodeID == -1);
        } else {
            REQUIRE((*aNode).parent -> nodeID == -1);
        }
    }
}
