//
//  HeirarchicalDecomposition.hpp
//  IMPrototypes
//
//  Created by Madhavan R.P on 6/8/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#ifndef HeirarchicalDecomposition_hpp
#define HeirarchicalDecomposition_hpp

#include <stdio.h>
#include "Graph.hpp"
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unordered_set>
#include <memory>
#include "../TIMInfluenceCalculator.hpp"

class HeirarchicalDecomposition {
    vector<int> binaryTree;
    
    // Number of nodes in tree. Each node has a list of seed sets. 
    vector<vector<unordered_set<int>>> seeds;
    vector<int> leaves;
    int height;
    int k;
    
    // Find Influence
    shared_ptr<TIMInfluenceCalculator> timInfluenceCalculator;
protected:
    vector<int> readVertexBuckets(string decompositionFile);
    vector<int> getRandomBuckets(int n);
    
public:
    HeirarchicalDecomposition(Graph *graph, string decompositionFile, int k);
//    HeirarchicalDecomposition(Graph *graph, vector<int> leafOrdering);
    set<int> maximizeUsingDP();
    int findIndexOfMaximizingSeed(int vertexL, int vertexR, int k);
};

#endif /* HeirarchicalDecomposition_hpp */
