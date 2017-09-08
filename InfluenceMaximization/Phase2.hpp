//
//  Phase2.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/31/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef Phase2_hpp
#define Phase2_hpp

#include <stdio.h>
#include "IMTree.hpp"
class Phase2 {
    
public:
    void doSomething(vector<int> nodeCounts);
    int findMaxInfluentialNode(vector<int> candidateNodes, vector<struct node*> seedSet);
};

#endif /* Phase2_hpp */
