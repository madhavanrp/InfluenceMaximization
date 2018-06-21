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
#include "../Graph.hpp"
#include <string.h>
#include <assert.h>
#include <math.h>
#include <unordered_set>
#include <memory>
#include <limits.h>
#include "../TIMInfluenceCalculator.hpp"

class HeirarchicalDecomposition {
    vector<int> binaryTree;
    
    // Number of nodes in tree. Each node has a list of seed sets. 
    vector<vector<unordered_set<int>>> seeds;
    vector<vector<int>> bucketWithSeeds;
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
    
    set<int> divideAndMaximize(int nBuckets);
    int findIndexOfMaximizingSeed(int vertexL, int vertexR, int k);
    
    set<int> findBestSeedFromCandidates(int k, vector<int> candidateNodes);
    set<int> findBestSeedFromTree(int currentNodeIndex, int currentHeight);
};

#endif /* HeirarchicalDecomposition_hpp */
