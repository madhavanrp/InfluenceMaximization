//
//  ApproximationSetting.h
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 10/30/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef ApproximationSetting_h
#define ApproximationSetting_h
/*
 The submodular functions f and g, for expected influence of targets and non targets respectively are approximated.
 For each element in base set, approximate (f-g)(i)
 */

enum ApproximationSetting {
    // f(i) - g(i) = f(i|1..i-1) - g(i)
    setting1,
    // f(i) - g(i) = f(i|1..i-1) - g(i|1..i-1)
    setting2,
    // returns -g(i|1..i-1)
    setting3,
    // returns f(i) - g(i) = f(i) - g(i|1..i-1)
    setting4
};

#endif /* ApproximationSetting_h */
