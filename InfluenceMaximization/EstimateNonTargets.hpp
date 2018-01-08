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


#define EPSILON 2

class EstimateNonTargets {
    int n;
    
public:
    Graph graph;
    vector<int> nodeCounts;
    
    
    EstimateNonTargets(Graph graph);
    EstimateNonTargets();
    int getNonTargets(int vertex);
    void writeToFile(string fileName);
    vector<int> getNonTargetsUsingTIM();
    vector<int> getNonTargetsUsingSIM();
    vector<int> generateRandomRRSet(int randomVertex, int rrSetID);
    vector<vector<int>>* generateRandomRRSets(int R, bool label);
    vector<vector<int>> rrSets;
    vector<vector<int>> *getRandomRRSets();
    deque<int> q;
    vector<bool> visited;
    vector<int> visitMark;
    void readFromFile(string fileName);
    
};
#endif /* EstimateNonTargets_hpp */


