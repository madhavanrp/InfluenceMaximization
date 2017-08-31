//
//  main.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/4/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include <iostream>
#include "InfluenceMaximization/Graph.hpp"
#include "InfluenceMaximization/IMTree.hpp"
#include "InfluenceMaximization/EstimateNonTargets.hpp"
#include "InfluenceMaximization/TIMUtility.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    Graph graph;
    graph.readGraph("world");
//    graph.generateRandomRRSets(25000000, true);
    EstimateNonTargets estimateNonTargets = EstimateNonTargets(graph);
    estimateNonTargets.getNonTargetsUsingTIM();
//    IMTree imTree;
//    imTree.addChild(0, 2, 4, 1);
//    imTree.addChild(0, 4, 5, 10);
//    imTree.addChild(0, 6, 5, 10);
//    int i=0;
//    for(struct node *aNode: imTree.tree) {
//        cout <<"\n Iteration " << i << "\n";
//        cout<< "Node ID: " << aNode -> nodeID;
//        if((*aNode).parent!=NULL) {
//            cout << "\t Parent Node ID: " << (*aNode).parent -> nodeID;
//        }
//        i++;
//    }
    cout <<"\n";
    return 0;
}
