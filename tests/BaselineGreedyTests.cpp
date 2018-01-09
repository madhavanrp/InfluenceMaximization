//
//  BaselineGreedyTests.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 1/8/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"
#include "../InfluenceMaximization/BaselineGreedy.hpp"

TEST_CASE("Comparator", "BaselineGreedy") {
    BaselineGreedy baselineGreedy;
    priority_queue<CELFNode, vector<CELFNode>, CelfComparator> *queue = baselineGreedy.getQueue();
    int n = 100;
    int max = 0;
    int maxNode = -1;
    for (int i=0; i<10; i++) {
        CELFNode node;
        node.nodeID = i;
        node.totalTargets = rand() % n;
        node.totalNonTargets = rand() % n;
        node.marginalGainTargets = node.totalTargets;
        node.marginalGainNonTargets = node.totalNonTargets;
        node.flag = 0;
        if (node.totalTargets>max) {
            max = node.totalTargets;
            maxNode = node.nodeID;
        }
        queue->push(node);
    }
    REQUIRE(queue->size()==10);

    REQUIRE(queue->top().nodeID==maxNode);
    CELFNode topNode = queue->top();
    queue->pop();
    CELFNode secondTop = queue->top();
    queue->pop();
    secondTop.flag = 1;
    secondTop.marginalGainTargets = topNode.marginalGainTargets;
    queue->push(topNode);
    REQUIRE(queue->top().nodeID==topNode.nodeID);
    queue->push(secondTop);
    REQUIRE(queue->top().nodeID==secondTop.nodeID);
}
