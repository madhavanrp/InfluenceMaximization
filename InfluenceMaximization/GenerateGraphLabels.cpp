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
    generate();
}

void GenerateGraphLabels::generate() {
    //Assume setting 1
    int n = this->graph->n;
    int numberOfTargets = round((float)n * this->percentage);
    int numberOfNonTargets = n - numberOfTargets;
    vector<bool> labels(n);
    float probability = 0.75f;
    
    for(int i=0; i<n; i++) {
        labels[i] = true;
    }
    int count = 0;
    vector<vector<int>> adjList = graph->graph;
    cout << "\n Value of n is " << n  << flush;
    cout << "\n Number of non targets aimed is " << numberOfNonTargets;
    cout << "\n Number of  targets aimed is " << numberOfTargets;
    
    while(true) {
        int randomVertex = rand() % n;
        if(!labels[randomVertex]) continue;
        labels[randomVertex] = false;
        count++;
        if(count >= numberOfNonTargets) break;
        for(int neighbor: adjList[randomVertex]) {
            if(!labels[neighbor]) continue;
            float randomFloat = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/probability));
            if(randomFloat<=probability) {
                labels[neighbor] = false;
                count++;
                if(count >= numberOfNonTargets) break;
            }
        
        }
        if(count >= numberOfNonTargets) break;
    }
    int sanityCount = 0;
    for(bool aBool: labels) {
        if(!aBool) sanityCount++;
    }
    cout << "\n Total number of non targets: " << count;
    cout << "\n Sanity count: " << sanityCount;
    cout << "\n Value of n: " << n;
    this->graph->setLabels(labels, this->percentage);
    this->graph->writeLabels();
}
