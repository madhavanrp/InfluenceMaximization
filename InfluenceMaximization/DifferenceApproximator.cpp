//
//  DifferenceApproximator.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "DifferenceApproximator.hpp"


ModularApproximation::ModularApproximation(vector<int> permutation, ApproximationSetting approximationSetting) {
    this->permutation = new vector<int>(permutation);
    this->n = (int)permutation.size();
    this->reverseMap = new vector<int>(this->n);
    this->approximations = new int[this->n];
    this->setting = approximationSetting;
    constructReverseMap();
}

void ModularApproximation::createTIMEvaluator(Graph *graph) {
    this->timEvaluator = new TIMEvaluator(graph, this->setting);
}


ModularApproximation::~ModularApproximation() {
    delete this->permutation;
    delete this->reverseMap;
    delete this->approximations;
    if(this->timEvaluator!=NULL) {
        delete this->timEvaluator;
    }
}

vector<int> ModularApproximation::getPerumutation() {
    return *this->permutation;
}
vector<int> ModularApproximation::getReverseMap() {
    return *this->reverseMap;
}

TIMEvaluator* ModularApproximation::getTIMEvaluator() {
    return this->timEvaluator;
}

int* ModularApproximation::getApproximations() {
    return this->approximations;
}

void ModularApproximation::constructReverseMap() {
    
    for(int i=0; i<(int)this->permutation->size(); i++) {
        int p = (*this->permutation)[i];
        (*this->reverseMap)[p] = i;
    }
}

void ModularApproximation::calculateApproximation(int element, set<int> *vertices) {
    set<int> seed;
    seed.insert(element);
    pair<int, int> totalInfluence = this->timEvaluator->findInfluence(&seed);
    int f = totalInfluence.first;
    int g = totalInfluence.second;
    this->approximations[element] = f-g;
}

void ModularApproximation::findAllApproximations() {
    set<int> *vertices = new set<int>();
    for (int i=0; i<this->n; i++) {
        int vertex = (*this->reverseMap)[i];
        calculateApproximation(vertex, vertices);
        vertices->insert(vertex);
        assert(vertices->size()==i+1);
    }
    delete vertices;
}

int ModularApproximation::evaluateFunction(set<int> elements) {
    int valueCalculated = 0;
    for(int element: elements) {
        valueCalculated+=evaluateFunction(element);
    }
    return valueCalculated;
}
int ModularApproximation::evaluateFunction(int element) {
    return this->approximations[element];
}

DifferenceApproximator::DifferenceApproximator(Graph *graph) {
    this->graph = graph;
}

void DifferenceApproximator::setN(int n) {
    this->n = n;
}
vector<int> DifferenceApproximator::generatePermutation() {
    int n = this->n;
    this->permutation = new vector<int>();
    for (int i =0; i<n; i++) {
        this->permutation->push_back(i);
    }
    random_shuffle(this->permutation->begin(), this->permutation->end());
    return *this->permutation;
}

vector<int> DifferenceApproximator::generatePermutation(vector<int> startingElements) {
    int totalSize = (int)this->permutation->size();
    
    // Shuffle the starting elements and build the new permutation
    random_shuffle(startingElements.begin(), startingElements.end());
    vector<int> *newPermutation = new vector<int>(totalSize);
    set<int> startingSet;
    int i = 0;
    for(int vertex: startingElements) {
        startingSet.insert(vertex);
        (*newPermutation)[i] = vertex;
        i++;
    }
    
    // Shuffle the existing permutation to randomize the new one.
    random_shuffle(this->permutation->begin(), this->permutation->end());
    for (int element: *this->permutation) {
        if(startingSet.find(element)!= startingSet.end()) continue;
        (*newPermutation)[i] = element;
        i++;
    }
    
    // The existing permutation should have been allocated on heap and deleted on destructor. Switch here.
    delete this->permutation;
    this->permutation = newPermutation;
    return *this->permutation;
}

set<int> DifferenceApproximator::executeGreedyAlgorithm(Graph *graph, ModularApproximation *modularApproximation, int k) {
    
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> orderedNodes;
    for(int i=0; i<graph->n; i++) {
        int evaluation = modularApproximation->evaluateFunction(i);
        orderedNodes.push(make_pair(i, evaluation));
    }
    set<int> seedSet;
    for (int i =0; i<k; i++) {
        seedSet.insert(orderedNodes.top().first);
        orderedNodes.pop();
    }
    return seedSet;
    
}

set<int> DifferenceApproximator::executeGreedyAlgorithmAdjustingPermutation(ApproximationSetting setting, int k) {
    set<int> seedSet;
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> orderedNodes;
    vector<int> permutation = generatePermutation();
    vector<int> startingVector;
    for (int i =0; i<k; i++) {
        orderedNodes = priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>();
        permutation = generatePermutation(startingVector);
        for(int j = 0; j<i; j++) {
            assert(seedSet.find(permutation[j])!=seedSet.end());
        }
        ModularApproximation *approximation = new ModularApproximation(permutation, setting);
        approximation->createTIMEvaluator(graph);
        approximation->findAllApproximations();
        for(int i=0; i<graph->n; i++) {
            int evaluation = approximation->evaluateFunction(i);
            orderedNodes.push(make_pair(i, evaluation));
        }
        delete approximation;

        int top = orderedNodes.top().first;
        while(seedSet.find(top)!=seedSet.end()) {
            orderedNodes.pop();
            top = orderedNodes.top().first;
        }
        seedSet.insert(top);
        startingVector.push_back(top);
    }
    
    return seedSet;
}

DifferenceApproximator::~DifferenceApproximator() {
    if(this->permutation!=NULL) {
        delete this->permutation;
    }
}
