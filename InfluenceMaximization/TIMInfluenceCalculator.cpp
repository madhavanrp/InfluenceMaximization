//
//  TIMInfluenceCalculator.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 1/16/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include "TIMInfluenceCalculator.hpp"

TIMInfluenceCalculator::TIMInfluenceCalculator(Graph *graph) {
    //Default Epsilon
    constructCalculator(graph, 2, "IC");
}

TIMInfluenceCalculator::TIMInfluenceCalculator(Graph *graph, double epsilon) {
    constructCalculator(graph, epsilon, "IC");
}

TIMInfluenceCalculator::TIMInfluenceCalculator(Graph *graph, double epsilon, string model) {
    constructCalculator(graph, epsilon, model);
}

void TIMInfluenceCalculator::constructCalculator(Graph *graph, double epsilon, string model) {
    this->graph = graph;
    this->epsilon = epsilon;
    this->model = model;
    int n = graph->getNumberOfVertices();
    
    //Initialize RR Set related data structures
    for(int i=0;i<n;i++) {
        this->targetCounts.push_back(0);
        this->nonTargetCounts.push_back(0);
        this->visitMark.push_back(0);
        this->visited.push_back(false);
    }
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    int i=0;
    while (i<R) {
        rrSetsTargets.push_back(vector<int>());
        rrSetsNonTargets.push_back(vector<int>());
        i++;
    }
    
    // Generate the Random RR Sets
    generateRandomRRSetsNonTargets(R);
    generateRandomRRSetsTargets(R);
    this->lookupTableTargets.reset(new vector<vector<int>>());
    this->lookupTableNonTargets.reset(new vector<vector<int>>());
    
    this->timCoverageTargets.reset(new TIMCoverage(this->lookupTableTargets.get()));
    this->timCoverageNonTargets.reset(new TIMCoverage(this->lookupTableNonTargets.get()));
    
    this->timCoverageTargets->initializeLookupTable(&rrSetsTargets, n);
    this->timCoverageTargets->initializeDataStructures(R, n);
    
    this->timCoverageNonTargets->initializeLookupTable(&rrSetsNonTargets, n);
    this->timCoverageNonTargets->initializeDataStructures(R, n);
    
    
 }

void TIMInfluenceCalculator::generateRandomRRSetsTargets(int R) {
    int n = this->graph->getNumberOfVertices();
    int randomVertex;
    if (this->model.compare("LT")==0) {
        cout << "\n Begin generation of LT model RR Sets";
        cout << "\n Value of R is " << R;
    }
    int totalSize = 0;
    for(int i=0;i<R;i++) {
        randomVertex = rand() % n;
        while(!graph->isTarget(randomVertex)) {
            randomVertex = rand() % n;
        }
        generateRandomRRSet(randomVertex, i, &rrSetsTargets, &targetCounts);
        totalSize += (int)rrSetsTargets[i].size();
    }
    cout << "\n Total Number of elements in RR Sets: " << totalSize;
    cout << "\n Average size of an RR Set is " << (double)totalSize/(double)R;
}

void TIMInfluenceCalculator::generateRandomRRSetsNonTargets(int R) {
    int randomVertex;
    vector<int> *nonTargets = graph->getNonTargets();
    if(graph->getNumberOfNonTargets()>0) {
        for(int i=0;i<R;i++) {
            randomVertex = (*nonTargets)[rand() % graph->getNumberOfNonTargets()];
            assert(!graph->isTarget(randomVertex));
            generateRandomRRSet(randomVertex, i, &rrSetsNonTargets, &nonTargetCounts);
        }
    }
}

void TIMInfluenceCalculator::generateRandomRRSet(int randomVertex, int rrSetID, vector<vector<int>> *rrSets, vector<int> *counts) {
    if (this->model.compare("IC")==0) {
        q.clear();
        
        (*rrSets)[rrSetID].push_back(randomVertex);
        q.push_back(randomVertex);
        int nVisitMark = 0;
        visitMark[nVisitMark++] = randomVertex;
        visited[randomVertex] = true;
        (*counts)[randomVertex]++;
        vector<vector<int>> *graphTranspose = graph->getGraphTranspose();
        while(!q.empty()) {
            int u=q.front();
            q.pop_front();
            for(int j=0; j<(int)(*graphTranspose)[u].size(); j++){
                int v = (*graphTranspose)[u][j];
                if(!graph->flipCoinOnEdge(v, u))
                    continue;
                if(visited[v])
                    continue;
                
                visitMark[nVisitMark++]=v;
                visited[v]=true;
                (*counts)[v]++;
                q.push_back(v);
                (*rrSets)[rrSetID].push_back(v);
            }
        }
        for(int i=0;i<nVisitMark;i++) {
            visited[visitMark[i]] = false;
        }

    }
    else {
        q.clear();
        
        (*rrSets)[rrSetID].push_back(randomVertex);
        q.push_back(randomVertex);
        int nVisitMark = 0;
        visitMark[nVisitMark++] = randomVertex;
        visited[randomVertex] = true;
        (*counts)[randomVertex]++;
        vector<vector<int>> *graphTranspose = graph->getGraphTranspose();
        while (!q.empty()) {
            int u=q.front();
            q.pop_front();
            
            if((*graphTranspose)[u].size()==0)
                continue;
            double randomDouble = (double)rand() / (double)RAND_MAX;
            for(int i=0; i<(int)(*graphTranspose)[u].size(); i++){
                int v = (*graphTranspose)[u][i];
                randomDouble = randomDouble - graph->getWeightForLTModel(v, u);
                if(randomDouble>0)
                    continue;
                
                if(visited[v])
                    break;
                visitMark[nVisitMark++]=v;
                visited[v]=true;
                q.push_back(v);
                
                (*rrSets)[rrSetID].push_back(v);
                break;
            }

        }
        for(int i=0;i<nVisitMark;i++) {
            visited[visitMark[i]] = false;
        }
    }

}

double TIMInfluenceCalculator::getScalingFactorTargets() {
    double scalingFactor = (double)this->graph->getNumberOfTargets()/(int)this->rrSetsTargets.size();
    return scalingFactor;
}

double TIMInfluenceCalculator::getScalingFactorNonTargets() {
    double scalingFactor = (double)this->graph->getNumberOfNonTargets()/(int)this->rrSetsNonTargets.size();
    return scalingFactor;
}

pair<int, int> TIMInfluenceCalculator::findInfluence(set<int> seedSet) {
    return findInfluence(seedSet, NULL);
}

pair<int, int> TIMInfluenceCalculator::findInfluence(set<int> seedSet, set<int> *alreadyActivated) {
//    int targetsInfluenced = this->timCoverageTargets->findInfluence(seedSet, getScalingFactorTargets());
    for (int seed:seedSet) {
        this->timCoverageTargets->addToSeed(seed, &this->rrSetsTargets);
    }
    int count = 0;
    for(bool edge:this->timCoverageTargets->edgeMark) {
        if(edge) count++;
    }
    int targetsInfluenced = round((double) count * getScalingFactorTargets());
    
    for (int seed:seedSet) {
        this->timCoverageNonTargets->addToSeed(seed, &this->rrSetsNonTargets);
    }
    count = 0;
    for(bool edge:this->timCoverageNonTargets->edgeMark) {
        if(edge) count++;
    }
    int nonTargetsInfluenced = round((double) count * getScalingFactorNonTargets());
//    int nonTargetsInfluenced = this->timCoverageNonTargets->findInfluence(seedSet, getScalingFactorNonTargets());
    return make_pair(targetsInfluenced, nonTargetsInfluenced);
}

pair<double, double> TIMInfluenceCalculator::findInfluenceWithoutUpdatingModel(set<int> seedSet) {
    
    double targets = this->timCoverageTargets->findInfluence(seedSet, getScalingFactorTargets());
    double nonTargets = this->timCoverageNonTargets->findInfluence(seedSet, getScalingFactorNonTargets());
    return make_pair(targets, nonTargets);

}

shared_ptr<TIMCoverage> TIMInfluenceCalculator::getTimCoverageTargets() {
    return this->timCoverageTargets;
}

shared_ptr<TIMCoverage> TIMInfluenceCalculator::getTimCoverageNonTargets() {
    return this->timCoverageNonTargets;
}

vector<vector<int>>* TIMInfluenceCalculator::getRRsetsTargets() {
    return &this->rrSetsTargets;
}

vector<vector<int>>* TIMInfluenceCalculator::getRRsetsNonTargets() {
    return &this->rrSetsNonTargets;
}


