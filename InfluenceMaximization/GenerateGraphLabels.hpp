//
//  GenerateGraphLabels.hpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 12/15/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef GenerateGraphLabels_hpp
#define GenerateGraphLabels_hpp

#include <stdio.h>
#include "Graph.hpp"
#include <math.h>
#include <assert.h>

enum LabelSetting {
    LabelSetting1
};

class GenerateGraphLabels {
    Graph *graph;
    LabelSetting setting;
    float percentage;
    int numberOfTargetsToLabel, numberOfNonTargetsToLabel;
    int totalNumberOfNonTargets;
    vector<bool> labels;
    GenerateGraphLabels(Graph *graph, float percentage, LabelSetting setting);
    void generate();
    void doDFSWithLabel(int currentNode, int currentDepth, int depthLimit);
public:
    GenerateGraphLabels(Graph *graph, float percentage);
};

#endif /* GenerateGraphLabels_hpp */
