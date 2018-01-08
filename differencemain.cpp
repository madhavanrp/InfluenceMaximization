//
//  differencemain.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 12/1/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include <iostream>

#include <string>
#include <chrono>

#include <iomanip>
#include <ctime>
#include <sstream>

#include "cxxopts.hpp"
using namespace std;

int main(int argc,  char * argv[]) {
    cout << "Setup logger \n";
    cxxopts::Options options("Targeted Influence Maximization", "Experiments and research.");
    //    options.add_options()
    //    ("alg", "Choose which algorithm to run", cxxopts::value<std::string>());
    options.add_options()
    ("d,debug", "Enable debugging")
    ("f,file", "File name", cxxopts::value<std::string>())
    ;
    cout << "Setup cxx opts\n ";
    auto result = options.parse(argc, argv);
    return 0;
}
