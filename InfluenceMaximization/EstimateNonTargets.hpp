//
//  EstimateNonTargets.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/29/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef EstimateNonTargets_hpp
#define EstimateNonTargets_hpp

#include <stdio.h>
#include "Graph.hpp"
#include <cmath>
#include <assert.h>
#include "json.hpp"
#include "Diffusion.hpp"
#include "SFMT/SFMT.h"

#define EPSILON 2

class EstimateNonTargets {
    int n;
    vector<double> nodeCounts;
    vector<vector<int>> rrSets;
    Graph *graph;
    string model;
    sfmt_t sfmt;
    
public:
    
    
    EstimateNonTargets(Graph *graph);
    EstimateNonTargets();
    void setDiffusionModel(string model);
    int getNonTargets(int vertex);
    void writeToFile(string fileName);
    vector<double> getNonTargetsUsingTIM();
    vector<double> getNonTargetsUsingSIM();
    vector<double> *getAllNonTargetsCount();
    void generateRandomRRSet(int randomVertex);
    vector<vector<int>>* generateRandomRRSets(int R, bool label);
    vector<vector<int>> *getRandomRRSets();
    deque<int> q;
    vector<bool> visited;
    vector<int> visitMark;
    void readFromFile(string fileName);
    
};
#endif /* EstimateNonTargets_hpp */


