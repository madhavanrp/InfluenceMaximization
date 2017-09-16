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
#include "InfluenceMaximization/Phase2.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    Graph *graph = new Graph;
    graph->readGraph("world");
//    graph.generateRandomRRSets(25000000, true);
    EstimateNonTargets estimateNonTargets = EstimateNonTargets(*graph);
    vector<int> nodeCounts = estimateNonTargets.getNonTargetsUsingTIM();
    Phase2TIM phase2(*graph);
    phase2.doSomething(nodeCounts);
    
    cout <<"\n";
    return 0;
}
