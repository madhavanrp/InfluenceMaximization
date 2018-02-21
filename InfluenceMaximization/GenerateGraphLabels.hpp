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
#include "TIM-Models/TIMCoverage.hpp"
#include "TIMInfluenceCalculator.hpp"
#include "Diffusion.hpp"

enum LabelSetting {
    LabelSetting1,
    // Find a seed set using TIM. Set all the nodes influenced by that to be non targets.
    LabelSettingTIMNonTargets
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
    void generateWithSetting1(int numberOfTargets, int numberOfNonTargets);
    void generateWithTIMNonTargets(int numberOfTargets, int numberOfNonTargets);
    void doDFSWithLabel(int currentNode, int currentDepth, int depthLimit);
public:
    GenerateGraphLabels(Graph *graph, float percentage);
};

#endif /* GenerateGraphLabels_hpp */
