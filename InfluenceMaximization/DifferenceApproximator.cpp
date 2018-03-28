//
//  DifferenceApproximator.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "DifferenceApproximator.hpp"


ModularApproximation::ModularApproximation(vector<int> permutation, ApproximationSetting approximationSetting) {
    this->timEvaluator = NULL;
    this->permutation = new vector<int>(permutation);
    this->n = (int)permutation.size();
    this->reverseMap = new vector<int>(this->n);
    this->approximations = new double[this->n];
    this->setting = approximationSetting;
    constructReverseMap();
}

void ModularApproximation::createTIMEvaluator(Graph *graph) {
    this->timEvaluator = new TIMEvaluator(graph, this->setting);
}

ModularApproximation::ModularApproximation( const ModularApproximation &obj) {
    this->permutation = new vector<int>(*obj.permutation);
    this->reverseMap = new vector<int>(*obj.reverseMap);
    this->n = obj.n;
    this->approximations = new double(*obj.approximations);
    this->setting = obj.setting;
}

ModularApproximation& ModularApproximation::operator=( const ModularApproximation &obj) {
    if(this==&obj) {
        return *this;
    }
    delete this->permutation;
    delete this->reverseMap;
    delete[] this->approximations;
    delete this->timEvaluator;
    
    this->permutation = new vector<int>(*obj.permutation);
    this->reverseMap = new vector<int>(*obj.reverseMap);
    this->n = obj.n;
    this->approximations = new double(*obj.approximations);
    this->setting = obj.setting;
    return *this;
}

ModularApproximation::~ModularApproximation() {
    delete this->permutation;
    delete this->reverseMap;
    delete[] this->approximations;
    delete this->timEvaluator;
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

double* ModularApproximation::getApproximations() {
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
    pair<double, double> totalInfluence = this->timEvaluator->findInfluence(&seed);
    double f = totalInfluence.first;
    double g = totalInfluence.second;
    this->approximations[element] = f-g;
}



void ModularApproximation::findAllApproximations() {
    
//    if(this->setting==setting6) {
//        this->timEvaluator->findInfluence(&relativeSet)
//    }
    set<int> *vertices = new set<int>();
    
    for (int i=0; i<this->n; i++) {
        int vertex = (*this->permutation)[i];
        if(this->setting==setting6) {
            // Do setting 6 special
        } else {
            calculateApproximation(vertex, vertices);
        }
        vertices->insert(vertex);
        assert(vertices->size()==i+1);
    }
    delete vertices;
}

double ModularApproximation::evaluateFunction(set<int> elements) {
    double valueCalculated = 0;
    for(int element: elements) {
        valueCalculated+=evaluateFunction(element);
    }
    return valueCalculated;
}

double ModularApproximation::evaluateFunction(int element) {
    return this->approximations[element];
}

DifferenceApproximator::DifferenceApproximator(Graph *graph) {
    this->graph = graph;
    this->permutation = NULL;
}

double DifferenceApproximator::getDifferenceValue() {
    return differenceValue;
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
    int totalSize = n;
    
    if(this->permutation==NULL) {
        generatePermutation();
    }
    
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

set<int> DifferenceApproximator::executeGreedyAlgorithm(ApproximationSetting setting, int k) {
    
    vector<int> permutation = generatePermutation();
    ModularApproximation modularApproximation(permutation, setting);
    modularApproximation.createTIMEvaluator(this->graph);
    modularApproximation.findAllApproximations();
    
    priority_queue<pair<int, double>, vector<pair<int, double>>, QueueComparator> orderedNodes;
    for(int i=0; i<graph->getNumberOfVertices(); i++) {
        double evaluation = modularApproximation.evaluateFunction(i);
        orderedNodes.push(make_pair(i, evaluation));
    }
    set<int> seedSet;
    double differenceValue = 0;
    for (int i =0; i<k; i++) {
        seedSet.insert(orderedNodes.top().first);
        differenceValue+=orderedNodes.top().second;
        orderedNodes.pop();
    }
    this->differenceValue = differenceValue;
    return seedSet;
    
}

set<int> DifferenceApproximator::executeGreedyAlgorithmAdjustingPermutation(ApproximationSetting setting, int k) {
    set<int> seedSet;
    priority_queue<pair<int, double>, vector<pair<int, double>>, QueueComparator> *orderedNodes;
    vector<int> permutation = generatePermutation();
    vector<int> startingVector;
    double differenceValue = 0;
    for (int i =0; i<k; i++) {
        orderedNodes = new priority_queue<pair<int, double>, vector<pair<int, double>>, QueueComparator>();
        permutation = generatePermutation(startingVector);
        for(int j = 0; j<i; j++) {
            assert(seedSet.find(permutation[j])!=seedSet.end());
        }
        assert(permutation.size()==this->graph->getNumberOfVertices());
        ModularApproximation *approximation = new ModularApproximation(permutation, setting);
        approximation->createTIMEvaluator(graph);
        approximation->findAllApproximations();
        for(int i=0; i<graph->getNumberOfVertices(); i++) {
            double evaluation = approximation->evaluateFunction(i);
            orderedNodes->push(make_pair(i, evaluation));
        }
        delete approximation;
        
        int top = orderedNodes->top().first;
        double diffValue = orderedNodes->top().second;
        while(seedSet.find(top)!=seedSet.end()) {
            orderedNodes->pop();
            top = orderedNodes->top().first;
            diffValue = orderedNodes->top().second;
        }
        delete orderedNodes;
        seedSet.insert(top);
        differenceValue+=diffValue;
        startingVector.push_back(top);
    }
    this->differenceValue = differenceValue;
    
    return seedSet;
}

// This will treat g as a modular function and approximate f-g_approx
set<int> DifferenceApproximator::executeAlgorithmModularG(int k) {
    set<int> seedSet;
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> orderedNodes;
    TIMEvaluator *timEvaluator = new TIMEvaluator(this->graph, setting5);
    TIMCoverage *timCoverageTargets = timEvaluator->getTIMCoverage();
    double reverseScale = (double)1/timEvaluator->getScalingFactorTargets();
    
    for (int i=0; i<graph->getNumberOfVertices(); i++) {
        double nonTargetsEstimate = timEvaluator->findSingleNodeNonTargetsInfluence(i);
        timCoverageTargets->offsetCoverage(i, nonTargetsEstimate * reverseScale * -1);
    }
    seedSet = timCoverageTargets->findTopKNodes(k, timEvaluator->getRRSetsTargets());
    delete timEvaluator;
    
    return seedSet;
}

// Call this function with setting 3. This will approximate only g while optimizing: f - g_aprox
set<int> DifferenceApproximator::executeAlgorithmApproximatingOneFunction(ApproximationSetting setting, int k) {
    set<int> seedSet;
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> orderedNodes;
    vector<int> permutation = generatePermutation();
    ModularApproximation *approximation = new ModularApproximation(permutation, setting);
    approximation->createTIMEvaluator(graph);
    approximation->findAllApproximations();
    TIMCoverage *timCoverageTargets = approximation->getTIMEvaluator()->getTIMCoverage();
    
    for (int i=0; i<graph->getNumberOfVertices(); i++) {
        /* 
         Get g(i) and update coverage so that its value is now f(i) - g(i).
         If you add i to the seed, and update the coverage of each node, its value will be TIM estimate of f(i|seed set) - g(i)
         */
        // This value is negative
        double nonTargetsEstimateNegated = approximation->evaluateFunction(i);
        // Now scale it back
        TIMEvaluator *timEvaluator = approximation->getTIMEvaluator();
        double reverseScale = (double)1/timEvaluator->getScalingFactorTargets();
        timCoverageTargets->offsetCoverage(i, nonTargetsEstimateNegated * reverseScale);
    }
    seedSet = timCoverageTargets->findTopKNodes(k, approximation->getTIMEvaluator()->getRRSetsTargets());
//    double fValue = timCoverageTargets->getNumberOfRRSetsCovered() * approximation->getTIMEvaluator()->getScalingFactorTargets();
//    double gValue = approximation->evaluateFunction(seedSet);
//    this->differenceValue = fValue + gValue;
    
    delete approximation;
    return seedSet;
}

// Call this function with setting 3. This will approximate only g while optimizing: f - g_aprox and extend permutation
set<int> DifferenceApproximator::executeAlgorithmApproximatingOneFunctionExtendPermutation(ApproximationSetting setting, int k) {
    set<int> seedSet;
    vector<int> permutation;
    vector<int> startingVector;
    
    for (int i=0; i<k; i++) {
        permutation = generatePermutation(startingVector);
        ModularApproximation *approximation = new ModularApproximation(permutation, setting);
        approximation->createTIMEvaluator(graph);
        approximation->findAllApproximations();
        TIMCoverage *timCoverageTargets = approximation->getTIMEvaluator()->getTIMCoverage();
        
        for(int seed: seedSet) {
            timCoverageTargets->addToSeed(seed, approximation->getTIMEvaluator()->getRRSetsTargets());
        }
        
        
        TIMEvaluator *timEvaluator = approximation->getTIMEvaluator();
        double reverseScale = (double)1/timEvaluator->getScalingFactorTargets();
        
        vector<int> scaledApproximations;
        for (int j=0; j<this->n; j++) {
            double gValue = approximation->evaluateFunction(j);
            scaledApproximations.push_back(gValue * reverseScale * -1);
        }
        
        pair<int, int> maxNodeAndInfluence = timCoverageTargets->findMaxInfluentialNodeWithApproximations(&seedSet, &scaledApproximations);
        assert(maxNodeAndInfluence.first!=-1);
        assert(seedSet.find(maxNodeAndInfluence.first)==seedSet.end());
        
        seedSet.insert(maxNodeAndInfluence.first);
        startingVector.push_back(maxNodeAndInfluence.first);
        
        delete approximation;
    }
    
    return seedSet;
}

DifferenceApproximator::~DifferenceApproximator() {
    delete this->permutation;
}

DifferenceApproximator& DifferenceApproximator::operator=( const DifferenceApproximator &obj) {
    if (&obj==this) {
        return *this;
    }
    delete this->permutation;
    if(obj.permutation!=NULL) {
        this->permutation = new vector<int>(*obj.permutation);
    }
    return *this;
}

DifferenceApproximator::DifferenceApproximator( const DifferenceApproximator &obj) {
    if(obj.permutation!=NULL) {
        this->permutation = new vector<int>(*obj.permutation);
    }
}

vector<double> DifferenceApproximator::calculateUpperBound(TIMCoverage *timCoverageNonTargets, double scalingFactorNonTargets, set<int> relativeSet) {
    
    double gOfX = timCoverageNonTargets->findInfluence(relativeSet, scalingFactorNonTargets);
    
    set<int> XCopy = relativeSet;
    double marginalGainSummations = 0;
    for (int v:relativeSet) {
        XCopy.erase(v);
        double gain = gOfX - timCoverageNonTargets->findInfluence(XCopy, scalingFactorNonTargets);
        XCopy.insert(v);
        marginalGainSummations+=gain;
        
    }
    set<int> seedSet;
    vector<double> approximations = vector<double>(this->n);
    for (int i=0; i<this->n; i++) {
        if(relativeSet.find(i)==relativeSet.end()) {
            seedSet.insert(i);
            double gOfI = timCoverageNonTargets->findInfluence(seedSet, scalingFactorNonTargets);
            approximations[i] = gOfX - marginalGainSummations + gOfI;
            seedSet.erase(i);
        } else {
            XCopy.erase(i);
            double gain = gOfX - timCoverageNonTargets->findInfluence(XCopy, scalingFactorNonTargets);
            XCopy.insert(i);
            approximations[i] = gOfX - marginalGainSummations + gain;
        }
        
    }
    return approximations;
}

set<int> DifferenceApproximator::executeSupSubProcedure(int k) {
    set<int> seedSet, previousSeedSet;
    TIMEvaluator *timEvaluator = new TIMEvaluator(this->graph, setting5);
    double scalingFactorNonTargets = timEvaluator->getScalingFactorNonTargets();
    TIMCoverage *timCoverageDifference = timEvaluator->getTIMCoverage();
    TIMCoverage *timCoverageNonTargets = timEvaluator->getTIMCoverageNonTargets();
    int n = graph->getNumberOfVertices();
    vector<double> approximations;
    for (int v=0; v<n; v++) {
        approximations.push_back(timEvaluator->findSingleNodeNonTargetsInfluence(v));
    }
    int iteration = 0;
    
    bool converge = false;
    do {
        vector<int> seedVector;
        previousSeedSet = seedSet;
        
        for (int u: previousSeedSet) {
            seedVector.push_back(u);
        }
        vector<double> marginalGains = timCoverageNonTargets->singleNodeMarginalGainWRTSet(seedVector, scalingFactorNonTargets);
        assert(marginalGains.size()==seedVector.size());
        
        //Adjust approximations
        for (int i=0; i<seedVector.size(); i++) {
            approximations[seedVector[i]]= approximations[seedVector[i]] + marginalGains[i] - timEvaluator->findSingleNodeNonTargetsInfluence(seedVector[i]);
        }
        
        for(int i=0; i<n; i++) {
            timCoverageDifference->offsetCoverage(i, -1 * (1/timEvaluator->getScalingFactorTargets()) * approximations[i]);
        }
        timCoverageDifference->updatePriorityQueueWithCurrentValues();
        
        TIMCoverage *coveragePointer = timEvaluator->getTIMCoverage();
        vector<vector<int>> *rrSetsPointer = timEvaluator->getRRSetsTargets();
        seedSet = randGreedyCSO(*coveragePointer, rrSetsPointer, k);
        
        for(int i=0; i<n; i++) {
            timCoverageDifference->offsetCoverage(i, 1 * (1/timEvaluator->getScalingFactorTargets()) * approximations[i]);
        }
        timCoverageDifference->updatePriorityQueueWithCurrentValues();
        
        // Adjust approximations back
        for (int i=0; i<seedVector.size(); i++) {
            approximations[seedVector[i]]= approximations[seedVector[i]] - marginalGains[i] + timEvaluator->findSingleNodeNonTargetsInfluence(seedVector[i]);
        }
        iteration++;
        
        
        set<int> s3;
        set_intersection(previousSeedSet.begin(), previousSeedSet.end(), seedSet.begin(), seedSet.end(), std::inserter(s3,s3.begin()));
        converge = (s3.size()>= (0.7 * k));
        if(iteration%300==0) {
            cout << "\n Completed iteration: " << iteration;
            cout <<"\n Set intersection size is " << s3.size() << flush;
        }
        if(iteration>5000) {
            converge = true;
            seedSet.clear();
        }
    } while(!converge);
    
    delete timEvaluator;
    return seedSet;
}

set<int> DifferenceApproximator::randGreedyCSO(TIMCoverage timCoverageDifference, vector<vector<int>> *rrSets, int budget) {
    set<int> seedSet;
    int k = budget;
    vector<int> *coverage = &timCoverageDifference.coverage;
    while (seedSet.size()<k) {
        priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> queue(timCoverageDifference.queue);
        vector<pair<int, int>> topKPairs;
        int i=0;
        while(i<k) {
            pair<int, int> nodeWithCoverage = make_pair(-1, -1);
            bool nodeFound = false;
            while (!queue.empty() && !nodeFound) {
                nodeWithCoverage = queue.top();
                queue.pop();
                if(nodeWithCoverage.second>(*coverage)[nodeWithCoverage.first]) {
                    queue.push(make_pair(nodeWithCoverage.first, (*coverage)[nodeWithCoverage.first]));
                    continue;
                }
                if(seedSet.find(nodeWithCoverage.first)==seedSet.end()) nodeFound = true;
            }
            if(!nodeFound) {
                break;
            }
            topKPairs.push_back(nodeWithCoverage);
            i++;
        }
        assert(topKPairs.size()==k);
        int randomUIndex = rand() % k;
        int randomU = topKPairs[randomUIndex].first;
        seedSet.insert(randomU);
        timCoverageDifference.addToSeed(randomU, rrSets);
        
    }
    
    return seedSet;
}
