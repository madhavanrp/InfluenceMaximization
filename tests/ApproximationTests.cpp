//
//  ApproximationTests.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include <stdio.h>
#include "catch.hpp"
#include "../InfluenceMaximization/DifferenceApproximator.hpp"

TEST_CASE("Correct size permutation" , "Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->n;
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    REQUIRE(permutation.size()==n);
    set<int> verticesCovered;
    for(int v: permutation) {
        verticesCovered.insert(v);
    }
    REQUIRE(verticesCovered.size()==n);
}

TEST_CASE("Reverse map" , "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->n;
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    ModularApproximation modularApprox(permutation);
    permutation = modularApprox.getPerumutation();
    REQUIRE(permutation.size()==n);
    vector<int> reverseMap = modularApprox.getReverseMap();
    bool correct = true;
    for(int i=0; i<(int)permutation.size(); i++) {
        int element = permutation[i];
        if(reverseMap[element]!=i) {
            correct = false;
        }
        
    }
    REQUIRE(correct);
    
    
}

TEST_CASE("Function Eval - Data Structures" , "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->n;
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    ModularApproximation modularApprox(permutation);
    modularApprox.createTIMEvaluator(graph);
    permutation = modularApprox.getPerumutation();
    modularApprox.findAllApproximations();
    
    TIMCoverage *timCoverage = modularApprox.getTIMEvaluator()->getTIMCoverage();
    vector<bool> nodeMark = timCoverage->nodeMark;
    vector<bool> edgeMark = timCoverage->edgeMark;
    bool allEdgesMarked = true;
    bool allNodesVisited = true;
    
    REQUIRE(nodeMark.size()==n);
    for(bool mark: nodeMark) {
        if(mark==true) {
            allNodesVisited = false;
        }
    }
    REQUIRE(allNodesVisited);
    
    for(bool mark: edgeMark) {
        if(mark==false) {
            allEdgesMarked = false;
        }
    }
    REQUIRE(allEdgesMarked);
}

TEST_CASE("Function Eval" , "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->n;
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    ModularApproximation modularApprox(permutation);
    modularApprox.createTIMEvaluator(graph);
    permutation = modularApprox.getPerumutation();
    vector<int> approximations;
    for(int vertex: permutation) {
        modularApprox.calculateApproximation(vertex, NULL);
        int approx = modularApprox.getApproximations()[vertex];
        approximations.push_back(approx);
    }
    bool allFunctionEvaluationsCorrect = true;
    for (int i=0; i<permutation.size(); i++) {
        int vertex = permutation[i];
        int approximation = approximations[i];
        int evaluation = modularApprox.evaluateFunction(vertex);
        if(evaluation!=approximation) allFunctionEvaluationsCorrect = false;
    }
    REQUIRE(allFunctionEvaluationsCorrect);
}
