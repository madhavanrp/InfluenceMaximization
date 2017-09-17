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
#include "InfluenceMaximization/Diffusion.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    Graph *graph = new Graph;
    graph->readGraph("world");
    
    vector<int> seedSet;
    seedSet.push_back(100);
    seedSet.push_back(474);
    seedSet.push_back(639);
    seedSet.push_back(239);
    seedSet.push_back(124);
    seedSet.push_back(606);
    seedSet.push_back(196);
    seedSet.push_back(287);
    seedSet.push_back(1162);
    seedSet.push_back(66);
//    seedSet.push_back(99);
//    seedSet.push_back(128);
//    seedSet.push_back(14);
//    seedSet.push_back(192);
//    seedSet.push_back(563);
//    seedSet.push_back(4824);
//    seedSet.push_back(634);
//    seedSet.push_back(221);
//    seedSet.push_back(525);
//    seedSet.push_back(37);
    vector<int> activatedSet = performDiffusion(*graph, seedSet);
    
    cout << "\n Activated set is ";
    for(int activated: activatedSet) {
        cout << activated << " ";
    }
    
//    EstimateNonTargets estimateNonTargets = EstimateNonTargets(*graph);
//    vector<int> nodeCounts = estimateNonTargets.getNonTargetsUsingTIM();
//    
//    Phase2TIM phase2(*graph);
//    phase2.doSomething(nodeCounts);
    
    cout <<"\n";
    return 0;
}
