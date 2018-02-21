//
//  GenerateGraphLabels.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 12/15/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "GenerateGraphLabels.hpp"

GenerateGraphLabels::GenerateGraphLabels(Graph *graph, float percentage, LabelSetting setting) {
    this->graph = graph;
    this->setting = setting;
    this->percentage = percentage;
    generate();
}

GenerateGraphLabels::GenerateGraphLabels(Graph *graph, float percentage) {
    this->graph = graph;
    this->setting = LabelSettingTIMNonTargets;
    this->percentage = percentage;
    this->totalNumberOfNonTargets = 0;
    int n = graph->getNumberOfVertices();
    this->numberOfTargetsToLabel = round((float)n * percentage);
    this->numberOfNonTargetsToLabel = n - this->numberOfTargetsToLabel;
    this->labels = vector<bool>(n);
    for (int i=0; i<n; i++) {
        this->labels[i] = true;
    }
    generate();
}

void GenerateGraphLabels::doDFSWithLabel(int currentNode, int currentDepth, int depthLimit) {
    if(currentDepth>depthLimit) return;
    if(!this->labels[currentNode]) return;
    if(this->totalNumberOfNonTargets>=this->numberOfNonTargetsToLabel) return;
    this->labels[currentNode] = false;
    this->totalNumberOfNonTargets++;
    float probability = 0.75f;
    vector<vector<int>> *adjacencyList = this->graph->getGraph();
    for(int neighbour: (*adjacencyList)[currentNode]) {
        float randomFloat = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/probability));
        if(randomFloat<=probability && !this->labels[currentNode]) {
            doDFSWithLabel(neighbour, currentDepth+1, depthLimit);
        }
    }
}

void GenerateGraphLabels::generate() {
    int n = this->graph->getNumberOfVertices();
    int numberOfTargets = round((float)n * this->percentage);
    int numberOfNonTargets = n - numberOfTargets;
    switch (this->setting) {
        case LabelSetting1:
            generateWithSetting1(numberOfTargets, numberOfNonTargets);
            break;
        case LabelSettingTIMNonTargets:
            generateWithTIMNonTargets(numberOfTargets, numberOfNonTargets);
            
        default:
            generateWithSetting1(numberOfTargets, numberOfNonTargets);
            break;
    }
    this->graph->setLabels(this->labels, this->percentage);
    this->graph->writeLabels();
}

void GenerateGraphLabels::generateWithSetting1(int numberOfTargets, int numberOfNonTargets) {
    int n = graph->getNumberOfVertices();
    vector<vector<int>> adjList = *graph->getGraph();
    cout << "\n Value of n is " << n  << flush;
    cout << "\n Number of non targets aimed is " << numberOfNonTargets;
    cout << "\n Number of  targets aimed is " << numberOfTargets;
    int level = 2;
    while(this->totalNumberOfNonTargets< this->numberOfNonTargetsToLabel) {
        int randomVertex = rand() % n;
        if(!this->labels[randomVertex]) continue;
        doDFSWithLabel(randomVertex, 0, level);
    }
    int sanityCount = 0;
    for(bool aBool: this->labels) {
        if(!aBool) sanityCount++;
    }
    cout << "\n Total number of non targets: " << this->totalNumberOfNonTargets;
    cout << "\n Sanity count: " << sanityCount;
    cout << "\n Value of n: " << n;
}

void GenerateGraphLabels::generateWithTIMNonTargets(int numberOfTargets, int numberOfNonTargets) {
    double epsilon = (double)2;
    int n = this->graph->getNumberOfVertices();
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    //    R = 23648871;
    Graph *graph = this->graph;
    graph->generateRandomRRSets(R, true);
    vector<vector<int>>* rrSets = graph->getRandomRRSets();
    
    vector<vector<int>> lookupTable;
    TIMCoverage *timCoverage = new TIMCoverage(&lookupTable);
    timCoverage->initializeLookupTable(rrSets, n);
    timCoverage->initializeDataStructures(R, n);
    set<int> seedSet;
    double scalingFactor = (double)n/(double)R;
    double summation = 0;
    while(timCoverage->getNumberOfRRSetsCovered() * scalingFactor< numberOfNonTargets ) {
        pair<int, double> nodeWithInfluence = timCoverage->findMaxInfluentialNodeAndUpdateModel(rrSets);
        seedSet.insert(nodeWithInfluence.first);
        summation+=nodeWithInfluence.second;
        double shouldBe = timCoverage->getNumberOfRRSetsCovered() * scalingFactor;
//        assert(summation==(timCoverage->getNumberOfRRSetsCovered() * scalingFactor));
    }
    delete timCoverage;
    set<int> activatedSet = findActivatedSetAndInfluenceUsingDiffusion(graph, seedSet, NULL).second;
    for (int activeNode:activatedSet) {
        this->labels[activeNode] = false;
        this->totalNumberOfNonTargets++;
    }
}
