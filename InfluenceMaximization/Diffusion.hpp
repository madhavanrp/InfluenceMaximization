//
//  Diffusion.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 9/15/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef Diffusion_hpp
#define Diffusion_hpp

#include <stdio.h>
#include <vector>
#include <set>
#include <cmath>
#include <deque>
#include "TIM-Models/TIMCoverage.hpp"
#include <assert.h>

#include "Graph.hpp"
using namespace std;

#define NUMBER_OF_SIMULATIONS 20000

inline void singleDiffusion(set<int> *activatedSet, Graph *graph, set<int> *seedSet, set<int> *alreadyActivated, vector<int> *visitMark, vector<bool> *visited, deque<int> *queue) {
    
    int nVisitMark = 0;
    if(alreadyActivated!=NULL) {
//        activatedSet->insert(alreadyActivated->begin(), alreadyActivated->end());
    }
    for(int seed: *seedSet) {
        if(activatedSet->find(seed)==activatedSet->end()) {
            if(!(*visited)[seed]) {
                queue->push_front(seed);
            }
        }
        while(!queue->empty()) {
            int u = queue->front();
            (*visited)[u] = true;
            (*visitMark)[nVisitMark] = u;
            nVisitMark++;
            queue->pop_front();
            activatedSet->insert(u);
            for (int v : graph->graph[u]) {
                bool activeEdge = graph->flipCoinOnEdge(u, v);
                if (activeEdge) {
                    if(!(*visited)[v])
                    if (activatedSet->find(v)==activatedSet->end()) {
                        queue->push_front(v);
                    }
                }
            }
        }
    }
    
    for(int i=0; i<nVisitMark; i++) {
        (*visited)[(*visitMark)[i]] = false;
    }
}

inline vector<int> performDiffusion(Graph *graph, set<int> seedSet, set<int> *alreadyActivated) {
    int activatedFrequency[graph->n];
    vector<int> visitMark;
    vector<bool> visited;
    for(int i=0; i<graph->n; i++) {
        activatedFrequency[i] = 0;
        visitMark.push_back(0);
        visited.push_back(false);
    }
    
    int totalActiveSetSize = 0;
    set<int> *activatedSet = new set<int>();
    deque<int> *diffusionQueue = new deque<int>();
    for (int i=0; i<NUMBER_OF_SIMULATIONS; i++) {
        singleDiffusion(activatedSet, graph, &seedSet, alreadyActivated, &visitMark, &visited, diffusionQueue);
        diffusionQueue->clear();
        set <int> :: iterator itr;
        totalActiveSetSize+= activatedSet->size();
        for (itr = activatedSet->begin(); itr != activatedSet->end(); ++itr)
        {
            activatedFrequency[*itr]++;
        }
        activatedSet->clear();
    }
    delete activatedSet;
    delete diffusionQueue;
    
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> queue;
    
    int averageActiveSetSize = round((double)totalActiveSetSize/(double)NUMBER_OF_SIMULATIONS);
//    cout << "\n Total activated set size = " << totalActiveSetSize;
//    cout << "\n Average activated set size = " << averageActiveSetSize;
    for(int i=0; i<graph->n; i++) {
        queue.push(make_pair(i, activatedFrequency[i]));
    }
    
    vector<int> averageActivatedSet;
    for(int i=0; i<averageActiveSetSize; i++) {
        averageActivatedSet.push_back(queue.top().first);
//        cout << "\n For i = " << i << " Best is " << queue.top().second << " and node is  " << queue.top().first;
        queue.pop();
    }
    return averageActivatedSet;
}

inline pair<int, int> findInfluenceUsingDiffusion(Graph *graph, set<int> seedSet, set<int> *alreadyActivated) {
    vector<int>activatedSet = performDiffusion(graph, seedSet, alreadyActivated);
    int targetsActivated = 0;
    int nonTargetsActivated = 0;
    for(int activeNode:activatedSet) {
        if(graph->labels[activeNode]) targetsActivated++;
        else nonTargetsActivated++;
    }
    return make_pair(targetsActivated, nonTargetsActivated);
}

inline pair<int, int> findInfluenceUsingDiffusion(Graph *graph, set<int> seedSet) {
    return findInfluenceUsingDiffusion(graph, seedSet, NULL);
}

inline pair<pair<int, int>, set<int>> findActivatedSetAndInfluenceUsingDiffusion(Graph *graph, set<int> seedSet, set<int> *alreadyActivated) {
    vector<int>activatedSet = performDiffusion(graph, seedSet, alreadyActivated);
    set<int>activated;
    
    int targetsActivated = 0;
    int nonTargetsActivated = 0;
    for(int activeNode:activatedSet) {
        if(graph->labels[activeNode]) targetsActivated++;
        else nonTargetsActivated++;
        activated.insert(activeNode);
    }
    return make_pair(make_pair(targetsActivated, nonTargetsActivated), activated);
}

#endif /* Diffusion_hpp */
