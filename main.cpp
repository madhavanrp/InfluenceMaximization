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
#include <chrono>

using json = nlohmann::json;


int main(int argc, const char * argv[]) {
    srand(time(0));
    int budget;
    int nonTargetThreshold;
    string graphFileName;
    int percentagetNonTargets;
    bool fromFile = false;
    string nonTargetsFileName;
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
        if(argc>5) {
            nonTargetsFileName = argv[5];
            fromFile = true;
        }
    }
    IMResults::getInstance().setFromFile(fromFile);
    // insert code here...
    float percentageNonTargetsFloat = (float)percentagetNonTargets/(float)100;
    Graph *graph = new Graph;
    graph->readGraph(graphFileName, percentageNonTargetsFloat);
    
    vector<int> nodeCounts;
    clock_t phase1StartTime = clock();
    EstimateNonTargets *estimateNonTargets = NULL;
    if(!fromFile) {
        estimateNonTargets = new EstimateNonTargets(*graph);
        nodeCounts = estimateNonTargets->getNonTargetsUsingTIM();
    } else {
        estimateNonTargets = new EstimateNonTargets();
        estimateNonTargets->readFromFile(nonTargetsFileName);
        nodeCounts = estimateNonTargets->nodeCounts;
        delete estimateNonTargets;
    }
    
    clock_t phase1EndTime = clock();

    double phase1TimeTaken = double(phase1EndTime - phase1StartTime) / CLOCKS_PER_SEC;
    IMResults::getInstance().setPhase1Time(phase1TimeTaken);
    if(!fromFile) {
        nonTargetsFileName = graphFileName;
        nonTargetsFileName+="_" + to_string(budget);
        nonTargetsFileName+="_" + to_string(nonTargetThreshold);
        nonTargetsFileName+="_" + to_string(80);
        nonTargetsFileName+="_" + to_string(rand() % 1000000);
        nonTargetsFileName+="_1";
        nonTargetsFileName+=".txt";
        estimateNonTargets->writeToFile(nonTargetsFileName);
        cout << "\nWriting Non Targets to file " << nonTargetsFileName;
        cout << "\n";
        IMResults::getInstance().setNonTargetFileName(nonTargetsFileName);
        delete estimateNonTargets;
    }

    
    //Start phase 2
    clock_t phase2StartTime = clock();
    Phase2TIM phase2(graph);
    phase2.doPhase2(budget, nonTargetThreshold, nodeCounts);
    clock_t phase2EndTime = clock();

    double phase2TimeTaken = double(phase2EndTime - phase2StartTime) / CLOCKS_PER_SEC;

    IMResults::getInstance().setPhase2Time(phase2TimeTaken);

    
    
    vector<IMSeedSet> allSeedSets = phase2.getTree()->getAllSeeds(budget);
    IMResults::getInstance().addSeedSets(allSeedSets);
    IMResults::getInstance().addBestSeedSet(phase2.getTree()->getBestSeedSet(budget));
    
    
    //Construct results file name
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(nonTargetThreshold);
    resultFileName+="_" + to_string(80);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+="_1";
    resultFileName+=".json";
    IMResults::getInstance().writeToFile(resultFileName);
    
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
