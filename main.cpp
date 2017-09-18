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
#include "InfluenceMaximization/IMResults/IMResults.h"
#include "InfluenceMaximization/memoryusage.h"
#include <sstream>

using json = nlohmann::json;

int main(int argc, const char * argv[]) {
    int budget;
    int nonTargetThreshold;
    if(argc==0) {
        budget=20;
        nonTargetThreshold = 10;
    } else {
        budget = atoi(argv[1]);
        nonTargetThreshold = atoi(argv[2]);
    }
    // insert code here...
    Graph *graph = new Graph;
    graph->readGraph("world");
    
    
    
    clock_t phase1StartTime = clock();
    EstimateNonTargets *estimateNonTargets = new EstimateNonTargets(*graph);
    vector<int> nodeCounts = estimateNonTargets->getNonTargetsUsingTIM();
//    EstimateNonTargets *estimateNonTargets = new EstimateNonTargets();
//    estimateNonTargets->readFromFile();
//    vector<int> nodeCounts = estimateNonTargets->nodeCounts;
    clock_t phase1EndTime = clock();
    delete estimateNonTargets;
    
    double phase1TimeTaken = double(phase1EndTime - phase1StartTime) / CLOCKS_PER_SEC;
    IMResults::getInstance().setPhase1Time(phase1TimeTaken);
    
    clock_t phase2StartTime = clock();
    Phase2TIM phase2(graph);
    phase2.doPhase2(budget, nonTargetThreshold, nodeCounts);
    clock_t phase2EndTime = clock();
    
    double phase2TimeTaken = double(phase2EndTime - phase2StartTime) / CLOCKS_PER_SEC;
    
    IMResults::getInstance().setPhase2Time(phase2TimeTaken);

    IMResults::getInstance().writeToFile("results/aResult.json");
    
    
    IMTree *tree = phase2.getTree();
    for(struct node* leaf:tree->getLeafNodes(budget)) {
        vector<int> seedSet;
        for(struct node* seed:tree->findSeedSetInPath(leaf)) {
            seedSet.push_back(seed->nodeID);
        }
        vector<int> activatedSet = performDiffusion(graph, seedSet);
        int targets = 0;
        int nonTargets = 0;
        for(int activeNode:activatedSet) {
            if(graph->labels[activeNode]) {
                targets++;
            } else {
                nonTargets++;
            }
        }
        cout << "\n For Threshold " << tree->influenceAlongPath(leaf).second << " Targets activated is  " << targets;
        cout << " Non targets activated is " << nonTargets;
    }
    delete graph;
    cout << "\n";
    disp_mem_usage("");
    return 0;
}
