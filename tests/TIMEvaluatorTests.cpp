//
//  TIMEvaluatorTests.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/24/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"
#include "../InfluenceMaximization/TIMEvaluator.hpp"

TEST_CASE("Value of Targets correctly calculated " , "Function Evaluation") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    TIMEvaluator timEvaluator(graph, setting1);
    set<int> seedSet;
    for (int i=0; i<20; i++) {
        int vertex = rand()%graph->getNumberOfVertices();
        if(seedSet.find(vertex)!=seedSet.end()) continue;
        seedSet.insert(vertex);
        pair<int, int> influence = timEvaluator.findInfluence(&seedSet);
        TIMCoverage *coverage = timEvaluator.getTIMCoverage();
        vector<bool> nodeMark = coverage->nodeMark;
        vector<bool> edgeMark = coverage->edgeMark;
        REQUIRE(nodeMark[vertex]==false);
        vector<int> rrSetsCovered = coverage->getRRSetsCoveredByVertex(vertex);
        for (int rrSetID:rrSetsCovered) {
            REQUIRE(edgeMark[rrSetID]);
        }

    }
    
    TIMCoverage *coverage = timEvaluator.getTIMCoverage();
    vector<bool> nodeMark = coverage->nodeMark;
    vector<bool> edgeMark = coverage->edgeMark;
    for(int i=0; i<graph->getNumberOfVertices();i++) {
        bool inSeed = true;
        if(seedSet.find(i)==seedSet.end()) {
            inSeed = false;
        }
        if(inSeed) {
            REQUIRE(nodeMark[i]==false);
            vector<int> rrSetsCovered = coverage->getRRSetsCoveredByVertex(i);
            for (int rrSetID:rrSetsCovered) {
                REQUIRE(edgeMark[rrSetID]);
            }
        } else {
            REQUIRE(nodeMark[i]==true);
        }
    }
    delete graph;
    
}

TEST_CASE("Setting 3 approximation", "Function Evaluation") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    TIMEvaluator timEvaluator(graph, setting3);
    TIMCoverage *coverageTargets = timEvaluator.getTIMCoverage();
    TIMCoverage *coverageNonTargets = timEvaluator.getTIMCoverageNonTargets();
    vector<bool> *nodeMark = coverageNonTargets->getNodeMark();
    vector<bool> *edgeMark = coverageNonTargets->getEdgeMark();
    REQUIRE(coverageNonTargets!=NULL);
    REQUIRE(coverageNonTargets->getNumberOfRRSetsCovered()==0);
    set<int> seedSet;
    for (int i=0; i<20; i++) {
        int vertex = rand()%graph->getNumberOfVertices();
        if(seedSet.find(vertex)!=seedSet.end()) continue;
        seedSet.insert(vertex);
        pair<double, double> influence = timEvaluator.findInfluence(&seedSet);
        REQUIRE(influence.first==0);
        vector<int> rrSetsCovered = coverageNonTargets->getRRSetsCoveredByVertex(vertex);
        for (int rrSetID:rrSetsCovered) {
            REQUIRE((*edgeMark)[rrSetID]);
        }
    }
    
    int totalRRSetsCovered = 0;
    for(bool edge: *edgeMark) {
        if (edge) {
            totalRRSetsCovered++;
        }
    }
    REQUIRE(totalRRSetsCovered==coverageNonTargets->getNumberOfRRSetsCovered());
    delete graph;
}
