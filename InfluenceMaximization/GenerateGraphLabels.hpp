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

enum LabelSetting {
    LabelSetting1
};

class GenerateGraphLabels {
    Graph *graph;
    LabelSetting setting;
    float percentage;
    GenerateGraphLabels(Graph *graph, float percentage, LabelSetting setting);
    void generate();
public:
    GenerateGraphLabels(Graph *graph, float percentage);
};

#endif /* GenerateGraphLabels_hpp */
