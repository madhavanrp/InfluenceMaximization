//
//  Graph.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/4/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include "Graph.hpp"
using namespace std;
void Graph::readGraph(string fileName) {
    ifstream myFile("graphs/graph_ic.inf");
    string s;
    if(myFile.is_open()) {
        myFile >> n >> m;
        for(int i =0;i<n;i++) {
            graph.push_back(vector<int>());
            visited.push_back(false);
            inDegree.push_back(0);
        }
        int from, to;
        int maxDegree = 0;
        double p = 0;
        while (myFile >> from >> to >> p) {
            graph[from].push_back(to);
            inDegree[to] = inDegree[to]+1;
            if(inDegree[to] > maxDegree) {
                maxDegree = inDegree[to];
            }
        }
        cout<<"\nMax degree is " << maxDegree << flush;
        myFile.close();
    }
    
    graphTranspose = constructTranspose(graph);
    visitMark = vector<int>(n);
    labels = vector<bool>(n);
    readLabels("graphs/graph_ic.inf_0.8_labels.txt");
    cout<<"\n label is " << labels[3147];
    cout << "\n label is " << labels[10910];
}


void Graph::readLabels(string fileName) {
    ifstream myFile(fileName);
    bool labelBool;
    if(myFile.is_open()) {
        int vertex;
        char label;
        while (myFile >> vertex >> label) {
            labelBool = (tolower(label)=='a');
            labels[vertex] = labelBool;
            if(!labelBool) {
                nonTargets.push_back(vertex);
            }
        }
        myFile.close();
    }
}

void Graph::generateRandomRRSets(int R, bool label) {
    int totalSize = 0;
    clock_t begin = clock();
    while(rrSets.size()<R) {
        rrSets.push_back(vector<int>());
    }
    for(int i=0;i<R;i++) {
        int randomVertex;
        randomVertex = rand() % n;
//        if(label) {
//            randomVertex = rand() % n;
//            while(labels[randomVertex]!=label) {
//                randomVertex = rand() % n;
//            }
//        } else {
//            randomVertex = rand() % nonTargets.size();
//        }
        generateRandomRRSet(randomVertex, i);
        totalSize+=rrSets[i].size();
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout <<"\n Generated " << R << " RR sets\n";
    cout << "Elapsed time " << elapsed_secs;
    cout<< " \n Time per RR Set is " << elapsed_secs/R;
    cout<< "\n Total Size is " << totalSize;
    cout<<"\n Average size is " << (float)totalSize/(float)R;
}

void Graph::clearRandomRRSets() {
    rrSets.clear();
}

vector<int> Graph::generateRandomRRSet(int randomVertex, int rrSetID) {
    q.clear();
    rrSets[rrSetID].push_back(randomVertex);
    q.push_back(randomVertex);
    int nVisitMark = 0;
    visitMark[nVisitMark++] = randomVertex;
    visited[randomVertex] = true;
    while(!q.empty()) {
        int expand=q.front();
        q.pop_front();
        for(int j=0; j<(int)graphTranspose[expand].size(); j++){
            //int u=expand;
            int v=graphTranspose[expand][j];
            int randInt = rand() % inDegree[v];
            if(randInt!=0)
                continue;
            if(visited[v])
                continue;
            if(!visited[v])
            {
                visitMark[nVisitMark++]=v;
                visited[v]=true;
            }
            q.push_back(v);
            rrSets[rrSetID].push_back(v);
        }
    }
    for(int i=0;i<nVisitMark;i++) {
        visited[visitMark[i]] = false;
        
    }
    return rrSets[rrSetID];
    
}

int Graph::BuildHypergraphNode(int uStart, int hyperiiid, bool addHyperEdge){
    int n_visit_edge=0;
    if(addHyperEdge)
    {
        rrSets[hyperiiid].push_back(uStart);
    }
    
    int n_visit_mark=0;
    //for(int i=0; i<12; i++) ASSERT((int)visit[i].size()==n);
    //for(int i=0; i<12; i++) ASSERT((int)visit_mark[i].size()==n);
    //hyperiiid ++;
    q.clear();
    q.push_back(uStart);
    visitMark[n_visit_mark++]=uStart;
    visited[uStart]=true;
    while(!q.empty()) {
        int u=q.front();
        q.pop_front();
        for(int j=0; j<(int)graphTranspose[u].size(); j++){
            int v=graphTranspose[u][j];
            n_visit_edge++;
            int inD = inDegree[u];
            int coin = rand() % inD;
            if(coin!=0)
                continue;
            if(visited[v])
                continue;
            if(!visited[v])
            {
                visitMark[n_visit_mark++]=v;
                visited[v]=true;
            }
            q.push_back(v);
            //#pragma omp  critical
            //if(0)
            
            rrSets[hyperiiid].push_back(v);
            
        }
        
    }
    for(int i=0; i<n_visit_mark; i++)
        visited[visitMark[i]]=false;
    return n_visit_edge;
}
vector<vector<int>> Graph::constructTranspose(vector<vector<int>> someGraph) {
    vector<vector<int>> transposedGraph = vector<vector<int>>();
    for(int i=0;i<someGraph.size();i++) {
        transposedGraph.push_back(vector<int>());
    }
    for(int i=0; i<someGraph.size();i++) {
        for(int v:someGraph[i]) {
            transposedGraph[v].push_back(i);
        }
    }
    return transposedGraph;
}
