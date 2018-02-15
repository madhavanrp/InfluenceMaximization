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
    this->setting = LabelSetting1;
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
    //Assume setting 1
    int n = this->graph->n;
    int numberOfTargets = round((float)n * this->percentage);
    int numberOfNonTargets = n - numberOfTargets;
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
    this->graph->setLabels(this->labels, this->percentage);
    this->graph->writeLabels();
}
