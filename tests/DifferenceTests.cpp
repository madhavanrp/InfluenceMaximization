//
//  DifferenceTests.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 2/22/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"
#include "../InfluenceMaximization/DifferenceApproximator.hpp"

TEST_CASE("Setting 3 is correct", "Difference Tests") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    
}
