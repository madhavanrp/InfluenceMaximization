//
//  GreedyModularKnapsack.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 4/24/19.
//  Copyright © 2019 Madhavan R.P. All rights reserved.
//

#include "GreedyModularKnapsack.hpp"

GreedyModularKnapsack::GreedyModularKnapsack(Graph *graph) {
    this->normalizedInfluenceCalculator = shared_ptr<NormalizedInfluenceCalculator>(new NormalizedInfluenceCalculator(graph));
}
//GreedyModularKnapsack::GreedyModularKnapsack(Graph *graph, string diffusionModel) {
//    this->normalizedInfluenceCalculator = shared_ptr<NormalizedInfluenceCalculator>(new NormalizedInfluenceCalculator(graph, diffusionModel));
//    
//}
set<int> GreedyModularKnapsack::executeNormalizedGreedy(int cost) {

    //Begin the algorithm
    set<int> s1, s2; // Return maximum of these
    
    // Find set of size one that maximizes
    
    s2 =  this->normalizedInfluenceCalculator.get()->findTopNodes(cost);
    cout << "\n Found set of size: " << s2.size();
    return s2;
    
    
}
