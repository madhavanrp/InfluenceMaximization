//
//  HeuristicsExecuter.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 5/15/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#ifndef HeuristicsExecuter_hpp
#define HeuristicsExecuter_hpp

#include <stdio.h>
#include "Graph.hpp"
#include <set>
#include <limits.h>

class HeuristicsExecuter {
    
public:
    set<int> executeNonTargetMinimizer(Graph *graph, int budget, int nontargetThreshold);
};

#endif /* HeuristicsExecuter_hpp */
