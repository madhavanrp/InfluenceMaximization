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
}

EstimateNonTargets::EstimateNonTargets() {
    
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
    while(rrSets.size()<R) {
        rrSets.push_back(vector<int>());
    }
    int randomVertex;
    if(graph->getNumberOfNonTargets()>0) {
        vector<int> *nonTargets = graph->getNonTargets();
        for(int i=0;i<R;i++) {
            randomVertex = (*nonTargets)[rand() % graph->getNumberOfNonTargets()];
            assert(!graph->labels[randomVertex]);
            generateRandomRRSet(randomVertex, i);
        }
    }
    int i=0;
    int maxNode = -1;
    int maxInfluence = -1;
    while(i<n) {
//        cout << "\n Phase1 : Vertex  " << i << " covered RR sets is " << nodeCounts[i];
       nodeCounts[i] = (double)nodeCounts[i] * (double)graph->getNumberOfNonTargets()/(double)R;
        
        if(nodeCounts[i]==0) {
            assert(graph->labels[i]);
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

vector<int> EstimateNonTargets::generateRandomRRSet(int randomVertex, int rrSetID) {
    q.clear();
    rrSets[rrSetID].push_back(randomVertex);
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    nodeCounts[randomVertex]++;
    while(!q.empty()) {
        int u=q.front();
        q.pop_front();
        for(int j=0; j<(int)graph->graphTranspose[u].size(); j++){
            int v = graph->graphTranspose[u][j];
            if(!graph->flipCoinOnEdge(v, u))
                continue;
            if(visited[v])
                continue;
            
            visitMark[nVisitMark++]=v;
            visited[v]=true;
            nodeCounts[v]++;
            q.push_back(v);
            rrSets[rrSetID].push_back(v);
        }
    }
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
        
    }
    return rrSets[rrSetID];
    
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
