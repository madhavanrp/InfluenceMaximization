//
//  EstimateNonTargets.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/29/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "EstimateNonTargets.hpp"

EstimateNonTargets::EstimateNonTargets(Graph *graph) {
    this->graph = graph;
    for(int i=0;i<graph->getNumberOfVertices();i++) {
        this->nodeCounts.push_back(0);
        this->visitMark.push_back(0);
        this->visited.push_back(false);
    }
    n = graph->getNumberOfVertices();
    this->model = "IC";
    sfmt_init_gen_rand(&sfmt, rand());
}

EstimateNonTargets::EstimateNonTargets() {
    
}

void EstimateNonTargets::setDiffusionModel(string model) {
    this->model = model;
}

vector<vector<int>>* EstimateNonTargets::getRandomRRSets() {
    return &this->rrSets;
}

int EstimateNonTargets::getNonTargets(int vertex) {
//    return getNonTargetsUsingTIM();
    return 0;
}

vector<double> EstimateNonTargets::getNonTargetsUsingSIM() {
    set<int> seedSet;
    for(int i=0; i<n; i++) {
        seedSet.insert(i);
        int nonTargets = findInfluenceUsingDiffusion(this->graph, seedSet).second;
        seedSet.clear();
        nodeCounts[i] = nonTargets;
    }
    return nodeCounts;
}

vector<double> EstimateNonTargets::getNonTargetsUsingTIM() {
    int n = graph->getNumberOfVertices();
    double epsilon = EPSILON;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    cout << "\n For phase 1: R is " << R;
    cout << "\n Non targets number is " << graph->getNumberOfNonTargets();
    generateRandomRRSets(R, false);
    return nodeCounts;
}

vector<double>* EstimateNonTargets::getAllNonTargetsCount() {
    return &this->nodeCounts;
}

vector<vector<int>>* EstimateNonTargets::generateRandomRRSets(int R, bool label) {
    clock_t begin = clock();
    rrSets = vector<vector<int>>();
    int randomVertex;
    if(graph->getNumberOfNonTargets()>0) {
        vector<int> *nonTargets = graph->getNonTargets();
        for(int i=0;i<R;i++) {
            randomVertex = (*nonTargets)[sfmt_genrand_uint32(&this->sfmt) % graph->getNumberOfNonTargets()];
            assert(graph->isNonTarget(randomVertex));
            generateRandomRRSet(randomVertex);
        }
    }
    int i=0;
    int maxNode = -1;
    int maxInfluence = -1;
    while(i<n) {
//        cout << "\n Phase1 : Vertex  " << i << " covered RR sets is " << nodeCounts[i];
       nodeCounts[i] = (double)nodeCounts[i] * (double)graph->getNumberOfNonTargets()/(double)R;
        
        if(nodeCounts[i]==0) {
            assert(graph->isTarget(i));
        }
        if(nodeCounts[i] > maxInfluence) {
            maxNode = i;
            maxInfluence = nodeCounts[i];
        }
        i++;
        
    }
    
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout <<"\n Generated " << R << " RR sets\n";
    cout << "Elapsed time " << elapsed_secs;
    cout<< " \n Time per RR Set is " << elapsed_secs/R;
    return &rrSets;
}

void EstimateNonTargets::generateRandomRRSet(int randomVertex) {
    q.clear();
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    nodeCounts[randomVertex]++;
    vector<vector<int>> *graphTranspose = graph->getGraphTranspose();
    while(!q.empty()) {
        if (this->model.compare("IC")==0) {
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
                nodeCounts[v]++;
                q.push_back(v);
            }
        }
        else {
            // LT Model
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
                nodeCounts[v]++;
                q.push_back(v);
                break;
            }
        }
        
        
    }
    rrSets.push_back(vector<int>(visitMark.begin(), visitMark.begin()+nVisitMark));
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
        
    }
}

void EstimateNonTargets::writeToFile(string fileName) {
    ofstream myfile;
    fileName = "NonTargetsEstimateFiles/" + fileName;
    myfile.open (fileName);
    for (int i=0; i<nodeCounts.size(); i++) {
        myfile << nodeCounts[i] << "\n";
        
    }
    myfile.close();
}

void EstimateNonTargets::readFromFile(string fileName) {
    ifstream myFile("NonTargetsEstimateFiles/" + fileName);
    double nonTargets;
    while (myFile >> nonTargets) {
        nodeCounts.push_back(nonTargets);
    }
    
}