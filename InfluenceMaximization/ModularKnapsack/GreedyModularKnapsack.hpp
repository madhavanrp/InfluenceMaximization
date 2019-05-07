//
//  GreedyModularKnapsack.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 4/24/19.
//  Copyright © 2019 Madhavan R.P. All rights reserved.
//

#ifndef GreedyModularKnapsack_hpp
#define GreedyModularKnapsack_hpp

#include <stdio.h>
#include "Graph.hpp"
#include "NormalizedInfluenceCalculator.hpp"
class GreedyModularKnapsack {
    shared_ptr<NormalizedInfluenceCalculator> normalizedInfluenceCalculator;
public:
    
    
    GreedyModularKnapsack(Graph *graph);
//    GreedyModularKnapsack(Graph *graph, string diffusionModel);
    set<int> executeNormalizedGreedy(int cost);
    
};
#endif /* GreedyModularKnapsack_hpp */
