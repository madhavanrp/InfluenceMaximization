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

void testIFContainsALLVertices(vector<int> *permutation) {
    set<int> aSet;
    for(int v: *permutation) {
        aSet.insert(v);
    }
    REQUIRE(aSet.size()==permutation->size());
}

TEST_CASE("Correct size permutation" , "Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->getNumberOfVertices();
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    REQUIRE(permutation.size()==n);
    set<int> verticesCovered;
    for(int v: permutation) {
        verticesCovered.insert(v);
    }
    REQUIRE(verticesCovered.size()==n);
    delete graph;
}

TEST_CASE("Permutation starting with some set", "Difference Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->getNumberOfVertices();
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
    delete graph;
}

TEST_CASE("Reverse map" , "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->getNumberOfVertices();
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
    delete graph;
    
}

TEST_CASE("Function Eval - Data Structures" , "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->getNumberOfVertices();
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
    delete graph;
}

TEST_CASE("Function Eval" , "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->getNumberOfVertices();
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
    delete graph;
}

TEST_CASE("Non Targets evaluation", "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->getNumberOfVertices();
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
    delete graph;
    
}

TEST_CASE("Setting 3 evaluation", "Modular Approx") {
    Graph *graph = new Graph;
    graph->readGraph("ca-GrQc-processed.txt");
    DifferenceApproximator differenceApproximator(graph);
    int n = graph->getNumberOfVertices();
    differenceApproximator.setN(n);
    vector<int> permutation = differenceApproximator.generatePermutation();
    ModularApproximation modularApprox(permutation, setting3);
    modularApprox.createTIMEvaluator(graph);
    
    TIMCoverage *timTargetsCoverage = modularApprox.getTIMEvaluator()->getTIMCoverage();
    TIMCoverage *timNonTargetsCoverage = modularApprox.getTIMEvaluator()->getTIMCoverageNonTargets();
    vector<vector<int>> rrSetsTargets = *modularApprox.getTIMEvaluator()->getRRSetsTargets();
    vector<vector<int>> rrSetsNonTargets = *modularApprox.getTIMEvaluator()->getRRSetsNonTargets();
    
    bool correctAdditionOfNonTargets = true;
    double *calculatedApproximations = new double[n];
    for(int vertex: permutation) {
        int marginalGainRRSets = timNonTargetsCoverage->numberOfNewRRSetsCoveredByVertex(vertex);
        modularApprox.calculateApproximation(vertex, NULL);
        double functionValue = (-1 * marginalGainRRSets * modularApprox.getTIMEvaluator()->getScalingFactorNonTargets());
        calculatedApproximations[vertex] = functionValue;
        for(int rrSetID: (*timNonTargetsCoverage->lookupTable)[vertex]) {
            if(!timNonTargetsCoverage->edgeMark[rrSetID]) {
                correctAdditionOfNonTargets = false;
            }
        }
    }
    bool correctFunctionCalculation = true;
    int number = 0;
    for(int i=0; i<n; i++) {
        number++;
        if(modularApprox.evaluateFunction(i)!=calculatedApproximations[i]) {
            correctFunctionCalculation = false;
        }
    }
    delete[] calculatedApproximations;
    REQUIRE(correctAdditionOfNonTargets);
    REQUIRE(correctFunctionCalculation);
    delete graph;
    
}

TEST_CASE("Max Influential node with approximations", "TIMCoverage and Approximation") {
    srand(time(0));
    vector<vector<int>> reverseMap = vector<vector<int>>();
    int n = 100;
    int R = n * 10;
    
    int maxNode = -1;
    int maxRRSetsCovered = -1;
    int secondMaxNode = -1;
    int secondMaxRRSetsCovered = -1;
    vector<vector<int>> rrSetsTargets;
    for (int i=0; i<R; i++) {
        rrSetsTargets.push_back(vector<int>());
    }
    for (int i=0; i<n; i++) {
        int numberOfRRSetsToCover = rand() % (R/2);
        vector<int> rrSetsCovered;
        set<int> rrSetsCoveredSet;
        while(rrSetsCoveredSet.size()<numberOfRRSetsToCover) {
            rrSetsCoveredSet.insert(rand()% R);
        }
        for(int rrSetID: rrSetsCoveredSet) {
            rrSetsCovered.push_back(rrSetID);
            rrSetsTargets[rrSetID].push_back(i);
        }
        if(numberOfRRSetsToCover>maxRRSetsCovered) {
            secondMaxRRSetsCovered = maxRRSetsCovered;
            secondMaxNode = maxNode;
            maxRRSetsCovered = numberOfRRSetsToCover;
            maxNode = i;
        }else if(numberOfRRSetsToCover<=maxRRSetsCovered && numberOfRRSetsToCover>secondMaxRRSetsCovered) {
            secondMaxRRSetsCovered = numberOfRRSetsToCover;
            secondMaxNode = i;
        }
    }
    
    
    //Create approximations:
    vector<int> approximations = vector<int>(n);
    for (int i=0; i<n; i++) {
        approximations[i] = 0;
        if (i==maxNode) {
            approximations[i] = maxRRSetsCovered;
        }
    }
    
    TIMCoverage *coverage = new TIMCoverage(&reverseMap);
    coverage->initializeLookupTable(&rrSetsTargets, n);
    coverage->initializeDataStructures(R, n);
    
    TIMCoverage *copy = new TIMCoverage(*coverage);
    set<int> seedSet;
    pair<int,double> nodeWithInfluence = copy->findMaxInfluentialNodeWithApproximations(&seedSet, &approximations);
    delete copy;
    seedSet.clear();
    REQUIRE(nodeWithInfluence.first!=maxNode);
    
    copy = new TIMCoverage(*coverage);
    seedSet.insert(nodeWithInfluence.first);
    pair<int, double> newPair = copy->findMaxInfluentialNodeWithApproximations(&seedSet, &approximations);
    REQUIRE(newPair.first!=nodeWithInfluence.first);
}
