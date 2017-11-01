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

DifferenceApproximator::~DifferenceApproximator() {
    if(this->permutation!=NULL) {
        delete this->permutation;
    }
}
