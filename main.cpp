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

#define PHASE1TIM_PHASE2TIM 1;
#define PHASE1SIM_PHASE2SIM 2;

int main(int argc, const char * argv[]) {
    srand(time(0));
    int budget;
    int nonTargetThreshold;
    string graphFileName;
    int percentagetNonTargets;
    bool fromFile = false;
    string nonTargetsFileName;
    int method;
    if(argc<=1) {
        budget=20;
        nonTargetThreshold = 10;
        graphFileName = "graph_ic.inf";
        percentagetNonTargets = 80;
        method = PHASE1TIM_PHASE2TIM;
        
    } else {
        budget = atoi(argv[2]);
        nonTargetThreshold = atoi(argv[3]);
        graphFileName = argv[1];
        percentagetNonTargets = atoi(argv[4]);
        method = atoi(argv[5]);
        if(argc>6) {
            nonTargetsFileName = argv[6];
            fromFile = true;
        }
    }
    
    
    cout << "\n Conducting experiments for:\n";
    cout <<" Graph: " << graphFileName;
    cout << "\t Budget: " << budget;
    cout << "\t Non Target Threshod: " << nonTargetThreshold;
    cout << "\t Percentage:  " << percentagetNonTargets;
    cout << "\t Method: " <<method;
    if(fromFile) {
        cout << "\n Reading Non targets from file: " << nonTargetsFileName;
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
        if(method==1) {
            nodeCounts = estimateNonTargets->getNonTargetsUsingTIM();
        } else {
            nodeCounts = estimateNonTargets->getNonTargetsUsingSIM();
        }
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
    Phase2 *phase2= NULL;
    if(method==1) {
        phase2 = new Phase2TIM(graph);
    }
    else {
        phase2 = new Phase2SIM(graph);
    }
    phase2->doPhase2(budget, nonTargetThreshold, nodeCounts);
    IMResults::getInstance().addBestSeedSet(phase2->getTree()->getBestSeedSet(budget));
    clock_t phase2EndTime = clock();
    double phase2TimeTaken = double(phase2EndTime - phase2StartTime) / CLOCKS_PER_SEC;
    
    IMResults::getInstance().setPhase2Time(phase2TimeTaken);

    
    
    vector<IMSeedSet> allSeedSets = phase2->getTree()->getAllSeeds(budget);
    IMResults::getInstance().addSeedSets(allSeedSets);
    
    //Construct results file name
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(nonTargetThreshold);
    resultFileName+="_" + to_string(80);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+="_1";
    resultFileName+=".json";
    
    IMSeedSet bestSeedSet = phase2->getTree()->getBestSeedSet(budget);
    delete phase2;
    
    pair<int, int> influenceOfBestSeedSet = findInfluenceUsingDiffusion(graph, bestSeedSet.getSeedSet());
    int targetsActivated = influenceOfBestSeedSet.first;
    int nonTargetsActivated = influenceOfBestSeedSet.second;
    
    cout << "\n Targets activated = " << targetsActivated;
    cout << "\n Non targets activated = " << nonTargetsActivated;
    
    IMResults::getInstance().setExpectedTargets(make_pair(targetsActivated, nonTargetsActivated));
    IMResults::getInstance().writeToFile(resultFileName);
    
    disp_mem_usage("");
    return 0;
}
