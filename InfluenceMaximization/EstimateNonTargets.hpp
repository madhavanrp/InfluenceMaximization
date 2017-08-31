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


#define EPSILON 2

class EstimateNonTargets {
    int n;
    
public:
    Graph graph;
    vector<int> nodeCounts;
    
    
    EstimateNonTargets(Graph graph);
    int getNonTargets(int vertex);
    void writeToFile();
    vector<int> getNonTargetsUsingTIM();
    vector<int> generateRandomRRSet(int randomVertex, int rrSetID);
    void generateRandomRRSets(int R, bool label);
    vector<vector<int>> rrSets;
    deque<int> q;
    vector<bool> visited;
    vector<int> visitMark;
    
};
#endif /* EstimateNonTargets_hpp */


