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

void singleDiffusion(set<int> *activatedSet, Graph *graph, set<int> *seedSet, set<int> *alreadyActivated) {
    
    deque<int> queue;
    set<int> unique;
    set<int> visited;
    assert(activatedSet->size()==0);
    if(alreadyActivated!=NULL) {
        activatedSet->insert(alreadyActivated->begin(), alreadyActivated->end());
        assert(activatedSet->size()==alreadyActivated->size());
    }
    for(int seed: *seedSet) {
        if(activatedSet->find(seed)==activatedSet->end()) {
            if(unique.find(seed)==unique.end()) {
                queue.push_front(seed);
                unique.insert(seed);
            }
        }
        while(!queue.empty()) {
            int u = queue.front();
            visited.insert(u);
            queue.pop_front();
            unique.erase(u);
            activatedSet->insert(u);
            for (int v : graph->graph[u]) {
                int randomNumber = rand() % graph->inDegree[v];
                
                if (randomNumber==0) {
                    if (activatedSet->find(v)==activatedSet->end()) {
                            if(unique.find(v)==unique.end()) {
                                queue.push_front(v);
                                unique.insert(v);
                            }
                    }
                }
            }
        }
    }
}

vector<int> performDiffusion(Graph *graph, set<int> seedSet, set<int> *alreadyActivated) {
    int activatedFrequency[graph->n];
    for(int i=0; i<graph->n; i++) {
        activatedFrequency[i] = 0;
    }
    
    int totalActiveSetSize = 0;
    set<int> *activatedSet = new set<int>();
    for (int i=0; i<NUMBER_OF_SIMULATIONS; i++) {
        singleDiffusion(activatedSet, graph, &seedSet, alreadyActivated);
        set <int> :: iterator itr;
        totalActiveSetSize+= activatedSet->size();
        for (itr = activatedSet->begin(); itr != activatedSet->end(); ++itr)
        {
            activatedFrequency[*itr]++;
        }
        activatedSet->clear();
    }
    delete activatedSet;
    
    
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

vector<int> performDiffusion(Graph *graph, set<int> seedSet) {
    return performDiffusion(graph, seedSet, NULL);
}
#endif /* Diffusion_hpp */
