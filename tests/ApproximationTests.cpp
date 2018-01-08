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

TEST_CASE("Permutation starting with some set", "Difference Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->n;
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    set<int> startingSet;
    for (int i = 0; i<10; i++) {
        int randomNumber = rand()%n;
        while(startingSet.find(randomNumber)!=startingSet.end()) {
            randomNumber = rand() % n;
        }
        startingSet.insert(randomNumber);
    }
    int startingSetSize = (int)startingSet.size();
    REQUIRE(startingSetSize==10);
    vector<int> startingElements;
    for (int element: startingSet) {
        
        startingElements.push_back(element);
    }
    REQUIRE(startingElements.size()==10);
    vector<int> newPermutation = differenceApproximator.generatePermutation(startingElements);
    REQUIRE(newPermutation.size()==n);
    bool correctlyStarting = true;
    for (int i = 0; i < startingSetSize; i++) {
        if(startingSet.find(newPermutation[i]) == startingSet.end()) {
            correctlyStarting = false;
        }
    }
    REQUIRE(correctlyStarting);
}

TEST_CASE("Reverse map" , "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->n;
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    ModularApproximation modularApprox(permutation, setting1);
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
    ModularApproximation modularApprox(permutation, setting1);
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
    ModularApproximation modularApprox(permutation, setting1);
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

TEST_CASE("Non Targets evaluation", "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->n;
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    ModularApproximation modularApprox(permutation, setting2);
    modularApprox.createTIMEvaluator(graph);
    
    TIMCoverage *timTargetsCoverage = modularApprox.getTIMEvaluator()->getTIMCoverage();
    TIMCoverage *timNonTargetsCoverage = modularApprox.getTIMEvaluator()->getTIMCoverageNonTargets();
    vector<vector<int>> rrSetsTargets = *modularApprox.getTIMEvaluator()->getRRSetsTargets();
    vector<vector<int>> rrSetsNonTargets = *modularApprox.getTIMEvaluator()->getRRSetsNonTargets();
    bool correctAdditionOfTargets = true;
    bool correctAdditionOfNonTargets = true;
    for(int vertex: permutation) {
        modularApprox.calculateApproximation(vertex, NULL);
        
        for(int rrSetID: (*timTargetsCoverage->lookupTable)[vertex]) {
            if(!timTargetsCoverage->edgeMark[rrSetID]) {
                correctAdditionOfTargets = false;
            }
        }
        for(int rrSetID: (*timNonTargetsCoverage->lookupTable)[vertex]) {
            if(!timNonTargetsCoverage->edgeMark[rrSetID]) {
                correctAdditionOfNonTargets = false;
            }
        }
    }
    
    REQUIRE(correctAdditionOfTargets);
    REQUIRE(correctAdditionOfNonTargets);
    
}
