//
//  EstimateNonTargets.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/29/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "EstimateNonTargets.hpp"

EstimateNonTargets::EstimateNonTargets(Graph graph) {
    this->graph = graph;
    for(int i=0;i<graph.n;i++) {
        this->nodeCounts.push_back(0);
        this->visitMark.push_back(0);
        this->visited.push_back(false);
    }
    n = graph.n;
}

EstimateNonTargets::EstimateNonTargets() {
    
}

int EstimateNonTargets::getNonTargets(int vertex) {
//    return getNonTargetsUsingTIM();
    return 0;
}

vector<int> EstimateNonTargets::getNonTargetsUsingTIM() {
    int n = graph.n;
    double epsilon = EPSILON;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    
    cout << "R is " << R;
    cout << "\n Non targets number is " << graph.nonTargets.size();
    generateRandomRRSets(R, false);
    writeToFile();
    return nodeCounts;
}

void EstimateNonTargets::generateRandomRRSets(int R, bool label) {
//    R = 10;
    clock_t begin = clock();
    while(rrSets.size()<R) {
        rrSets.push_back(vector<int>());
    }
    int randomVertex;
    if(graph.nonTargets.size()>0) {
        for(int i=0;i<R;i++) {
            randomVertex = graph.nonTargets[rand() % graph.nonTargets.size()];
            assert(!graph.labels[randomVertex]);
            generateRandomRRSet(randomVertex, i);
        }
    }
    int i=0;
    int maxNode = -1;
    int maxInfluence = -1;
    while(i<n) {
//        cout << "\n Phase1 : Vertex  " << i << " covered RR sets is " << nodeCounts[i];
//       nodeCounts[i] = round( (float)nodeCounts[i] * (float)graph.nonTargets.size()/(float)R);
        
        if(nodeCounts[i]==0) {
            assert(graph.labels[i]);
        }
        if(nodeCounts[i] > maxInfluence) {
            maxNode = i;
            maxInfluence = nodeCounts[i];
        }
        i++;
        
    }
    cout << "\nMax node is " << maxNode;
    cout << "\n Max influence is " << maxInfluence;
    
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout <<"\n Generated " << R << " RR sets\n";
    cout << "Elapsed time " << elapsed_secs;
    cout<< " \n Time per RR Set is " << elapsed_secs/R;
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
        for(int j=0; j<(int)graph.graphTranspose[u].size(); j++){
            int v=graph.graphTranspose[u][j];
            int randInt = rand() % graph.inDegree[u];
            if(randInt!=0)
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

void EstimateNonTargets::writeToFile() {
    ofstream myfile;
    myfile.open ("NonTargetsEstimateFiles/nonTargets.txt");
    for (int i=0; i<nodeCounts.size(); i++) {
        myfile << nodeCounts[i] << "\n";
        
    }
    myfile.close();
}

void EstimateNonTargets::readFromFile() {
    ifstream myFile("NonTargetsEstimateFiles/nonTargets.txt");
    int nonTargets;
    while (myFile >> nonTargets) {
        nodeCounts.push_back(nonTargets);
    }
    
}
