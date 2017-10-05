//
//  GraphTests.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/5/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"
#include "../InfluenceMaximization/Graph.hpp"

TEST_CASE("Graph is read correctly " , "Graph") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    int n = graph->n;
    
    int randomV = rand() % n;
    int inDegree = graph->inDegree[randomV];
    int randomU = graph->graphTranspose[randomV][rand() % graph->graphTranspose[randomV].size()];
    int randomNumber = graph->generateRandomNumber(randomU, randomV);
    REQUIRE(randomNumber>=0);
    REQUIRE(randomNumber<inDegree);
    
    graph->flipCoinOnEdge(randomU, randomV);
    
    graph->setPropogationProbability(0.01);
    int propogationProbabilityNumber = graph->getPropogationProbabilityNumber();
    REQUIRE(propogationProbabilityNumber==100);
    randomNumber = graph->generateRandomNumber(randomU, randomV);
    REQUIRE(randomNumber>=0);
    REQUIRE(randomNumber<100);
}
