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
    srand(time(0));
    int budget;
    int nonTargetThreshold;
    string graphFileName;
    int percentagetNonTargets;
    if(argc<=1) {
        budget=20;
        nonTargetThreshold = 10;
        graphFileName = "graph_ic.inf";
        percentagetNonTargets = 80;
        
    } else {
        budget = atoi(argv[2]);
        nonTargetThreshold = atoi(argv[3]);
        graphFileName = argv[1];
        percentagetNonTargets = atoi(argv[4]);
    }
    // insert code here...
    float percentageNonTargetsFloat = (float)percentagetNonTargets/(float)100;
    Graph *graph = new Graph;
    graph->readGraph(graphFileName, percentageNonTargetsFloat);
    
    
    
    clock_t phase1StartTime = clock();
    EstimateNonTargets *estimateNonTargets = new EstimateNonTargets(*graph);
    vector<int> nodeCounts = estimateNonTargets->getNonTargetsUsingTIM();
    
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
    resultFileName+=".json";
    IMResults::getInstance().writeToFile(resultFileName);
    
    vector<IMSeedSet> allSeedSets = phase2.getTree()->getAllSeeds(budget);
    for(IMSeedSet imSeedSet:allSeedSets) {
        set<int> seedSet = imSeedSet.getSeedSet();
        vector<int> activatedVector = performDiffusion(graph, seedSet);
        int targetsActivated = 0;
        int nonTargetsActivated = 0;
        for(int i:activatedVector) {
            if(graph->labels[i]) targetsActivated++;
            else nonTargetsActivated++;
        }
        assert(targetsActivated+nonTargetsActivated==activatedVector.size());
        cout << "\n Targets activated = " << targetsActivated;
        cout << "\n Non targets activated = " << nonTargetsActivated;
        
    }
    
    cout << "\n Finding best";
    IMSeedSet bestSeedSet = phase2.getTree()->getBestSeedSet(budget);
    int targetsActivated = 0;
    int nonTargetsActivated = 0;
    for(int i:performDiffusion(graph, bestSeedSet.getSeedSet())) {
        if(graph->labels[i]) targetsActivated++;
        else nonTargetsActivated++;
    }
    
    cout << "\n Targets activated = " << targetsActivated;
    cout << "\n Non targets activated = " << nonTargetsActivated;
    
    disp_mem_usage("");
    return 0;
}
