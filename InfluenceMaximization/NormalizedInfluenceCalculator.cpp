//
//  NormalizedInfluenceCalculator.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 4/24/19.
//  Copyright © 2019 Madhavan R.P. All rights reserved.
//

#include "NormalizedInfluenceCalculator.hpp"

NormalizedInfluenceCalculator::NormalizedInfluenceCalculator(Graph *graph) {
    sfmt_init_gen_rand(&sfmt, rand());
    constructCalculator(graph, 2, "IC");
    
}
void NormalizedInfluenceCalculator::constructCalculator(Graph *graph, double epsilon, string model) {
    this->graph = graph;
    this->epsilon = epsilon;
    this->model = model;
    int n = graph->getNumberOfVertices();
    
    //Initialize RR Set related data structures
    this->targetCounts = vector<int>(n);
    this->nonTargetCounts = vector<int>(n);
    this->visitMark = vector<int>(n);
    this->visited = vector<bool>(n);
    this->nonTargetInfluence = vector<double>(n);
    for(int i=0;i<n;i++) {
        this->targetCounts[i] = 0;
        this->nonTargetCounts[i] = 0;
        this->visitMark[i] = 0;
        this->visited[i] = false;
    }
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    this->R = R;
    // Generate the Random RR Sets
    generateRandomRRSetsNonTargets(R);
    generateRandomRRSetsTargets(R);
    this->lookupTableTargets.reset(new vector<vector<int>>());
    
    this->normalizedCoverage.reset(new TIMNormalizedCoverage(this->lookupTableTargets.get(), this->getScalingFactorTargets()));
    
    this->normalizedCoverage.get()->initializeLookupTable(&rrSetsTargets, n, nonTargetInfluence);
    this->normalizedCoverage->initializeDataStructures(R, n);
    
    
    
}

void NormalizedInfluenceCalculator::generateRandomRRSetsNonTargets(int R) {
    int randomVertex;
    vector<int> *nonTargets = graph->getNonTargets();
    if(graph->getNumberOfNonTargets()>0) {
        for(int i=0;i<R;i++) {
            randomVertex = (*nonTargets)[sfmt_genrand_uint32(&sfmt) % graph->getNumberOfNonTargets()];
            assert(!graph->isTarget(randomVertex));
            generateRandomRRSetNonTargets(randomVertex, &nonTargetCounts);
        }
    }
    double scalingFactorNonTargets = getScalingFactorNonTargets();
    for (int i=0; i<this->nonTargetCounts.size(); i++) {
        this->nonTargetInfluence[i] = this->nonTargetCounts[i] * scalingFactorNonTargets;
    }
    
}

void NormalizedInfluenceCalculator::generateRandomRRSetsTargets(int R) {
    int n = this->graph->getNumberOfVertices();
    int randomVertex;
    for(int i=0;i<R;i++) {
        randomVertex = sfmt_genrand_uint32(&sfmt) % n;
        while(!graph->isTarget(randomVertex)) {
            randomVertex = sfmt_genrand_uint32(&sfmt) % n;
        }
        generateRandomRRSetTargets(randomVertex, &rrSetsTargets, &targetCounts);
    }
}

void NormalizedInfluenceCalculator::generateRandomRRSetNonTargets(int randomVertex, vector<int> *counts) {
    if (this->model.compare("IC")==0) {
        q.clear();
        
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
            }
        }
        for(int i=0;i<nVisitMark;i++) {
            visited[visitMark[i]] = false;
        }
        
    }
    else {
        q.clear();
        
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
            double randomDouble = sfmt_genrand_res53(&sfmt);
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
                
                break;
            }
            
        }
        for(int i=0;i<nVisitMark;i++) {
            visited[visitMark[i]] = false;
        }
    }
    
}

void NormalizedInfluenceCalculator::generateRandomRRSetTargets(int randomVertex, vector<vector<int>> *rrSets, vector<int> *counts) {
    if (this->model.compare("IC")==0) {
        q.clear();
        
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
            }
        }
        rrSets->push_back(vector<int>(visitMark.begin(), visitMark.begin()+nVisitMark));
        for(int i=0;i<nVisitMark;i++) {
            visited[visitMark[i]] = false;
        }
        
    }
    else {
        q.clear();
        
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
            double randomDouble = sfmt_genrand_res53(&sfmt);
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
                
                break;
            }
            
        }
        rrSets->push_back(vector<int>(visitMark.begin(), visitMark.begin()+nVisitMark));
        for(int i=0;i<nVisitMark;i++) {
            visited[visitMark[i]] = false;
        }
    }
    
}

double NormalizedInfluenceCalculator::getScalingFactorTargets() {
    double scalingFactor = (double)this->graph->getNumberOfTargets()/(int)this->rrSetsTargets.size();
    return scalingFactor;
}

double NormalizedInfluenceCalculator::getScalingFactorNonTargets() {
    double scalingFactor = (double)this->graph->getNumberOfNonTargets()/(int)this->R;
    return scalingFactor;
}

set<int> NormalizedInfluenceCalculator::findTopNodes(int costConstraint) {
    return this->normalizedCoverage.get()->findTopNodes(costConstraint, &rrSetsTargets);
}
