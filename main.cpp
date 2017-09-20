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

void printTopk(int k, vector<int> counts) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> queue;
    for(int i=0;i<counts.size();i++) {
        queue.push(make_pair(i, counts[i]));
    }
    for(int i=0;i<k;i++) {
        pair<int,int> vertexPair = queue.top();
        queue.pop();
        cout << "\nVertex " << vertexPair.first << " Covers: " <<vertexPair.second;
    }
}

int main(int argc, const char * argv[]) {
    srand(time(0));
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
//    printTopk(20, nodeCounts);
    
    Phase2TIM phase2(graph);
    TIMCoverage *coverage = phase2.getTree()->root->coverage;
    set<int> candidateNodes;
    for(int i=0;i<graph->n;i++) {
        candidateNodes.insert(i);
    }
//    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> queueCopy = coverage->queue;
//    
//    cout << "\n Initial max influential node";
//    for (int i=0; i<20; i++) {
//        cout << "\n Max influential node = " << queueCopy.top().first << " Coverage is " << queueCopy.top().second;
//        queueCopy.pop();
//    }
//    cout << "\n Accounting for submodularity:";
//    vector<int> seedSet;
//    for(int i=0; i<20; i++) {
//        pair<int, int> maxPair = phase2.findMaxInfluentialNode(candidateNodes, coverage);
//        phase2.addToSeed(maxPair.first, coverage);
//        cout << "\n Max influential node = " << maxPair.first << " Coverage is " << maxPair.second;
//        seedSet.push_back(maxPair.first);
//    }
//    
//    vector<int> activatedSet = performDiffusion(graph, seedSet);
//    cout << "\n Activated set size = " << activatedSet.size();
    
    
    Phase2TIM newPhase2(graph);

    newPhase2.doPhase2(20, 10, nodeCounts);
    
    
    vector<struct node*> leafNodes = newPhase2.getTree()->getLeafNodes(budget);
    for(struct node* leaf:leafNodes) {
        vector<int> seedSet;
        vector<struct node*> seedAlongPath = newPhase2.getTree()->findSeedSetInPath(leaf);
        for (struct node* seed:seedAlongPath) {
            seedSet.push_back(seed->nodeID);
        }
        
        vector<int> activatedVector = performDiffusion(graph, seedSet);
        set<int> activatedSet;
        int targetsActivated = 0;
        int nonTargetsActivated = 0;
        for(int i:activatedVector) {
            activatedSet.insert(i);
            if(graph->labels[i]) targetsActivated++;
            else nonTargetsActivated++;
        }
        for(int i:seedSet) {
            assert(activatedSet.find(i)!=activatedSet.end());
        }
//        cout << "\n Activated set size = " << activatedVector.size();
        assert(targetsActivated+nonTargetsActivated==activatedVector.size());
        cout << "\n Targets activated = " << targetsActivated;
        cout << "\n Non targets activated = " << nonTargetsActivated;
        
    }
    
    disp_mem_usage("");
    return 0;
}
