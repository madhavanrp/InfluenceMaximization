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
#include <string>

using json = nlohmann::json;

int main(int argc, const char * argv[]) {
    int budget;
    int nonTargetThreshold;
    string graphFileName;
    if(argc<=1) {
        budget=20;
        nonTargetThreshold = 10;
        graphFileName = "graph_ic.inf";
        
    } else {
        budget = atoi(argv[2]);
        nonTargetThreshold = atoi(argv[3]);
        graphFileName = argv[1];
    }
    // insert code here...
    Graph *graph = new Graph;
    graph->readGraph(graphFileName);
    
    
    
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

    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(nonTargetThreshold);
    resultFileName+="_" + to_string(80);
    resultFileName+="_" + to_string(rand() % 1000000);
    IMResults::getInstance().writeToFile(resultFileName);
    
    
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
