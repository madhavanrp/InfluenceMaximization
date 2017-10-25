//
//  DifferenceApproximator.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/23/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef DifferenceApproximator_hpp
#define DifferenceApproximator_hpp

#include <stdio.h>
#include <vector>
#include <set>
#include "FunctionEvaluators.hpp"
#include "Graph.hpp"
#include <algorithm>

using namespace std;
class ModularApproximation {
    vector<int> *permutation;
    vector<int> *reverseMap;
    int n;
    int *approximations;
    TIMEvaluator *timEvaluator;
public:
    ModularApproximation(vector<int> permutation);
    ~ModularApproximation();
    vector<int> getPerumutation();
    vector<int> getReverseMap();
    TIMEvaluator *getTIMEvaluator();
    int* getApproximations();
    
    void createTIMEvaluator(Graph *graph);
    void constructReverseMap();
    void calculateApproximation(int element, set<int> *vertices);
    void findAllApproximations();
    int evaluateFunction(set<int> elements);
    int evaluateFunction(int element);
};

class DifferenceApproximator {
    vector<int> *permutation;
    int n;
    Graph *graph;
public:
    DifferenceApproximator(Graph *graph);
    ~DifferenceApproximator();
    void setN(int n);
    vector<int> generatePermutation();
};

#endif /* DifferenceApproximator_hpp */
