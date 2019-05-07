//
// Created by Madhavan R.P on 2019-05-06.
//

#include <iostream>
#include "main_utils.h"
#include "InfluenceMaximization/Graph/Graph.hpp"
#include "InfluenceMaximization/DifferenceAlgorithms/DifferenceApproximator.hpp"
#include "InfluenceMaximization/IMResults/IMResults.h"

string constructResultFileName(string graphFileName, int budget, int percentageTargets, ApproximationSetting setting) {
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(percentageTargets);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+="_" + to_string(setting);
    resultFileName+=".json";
    return resultFileName;
}

void testApprox(Graph *graph, int budget, ApproximationSetting setting, bool extendPermutation) {
    clock_t differenceStartTime = clock();
    DifferenceApproximator differenceApproximator(graph);
    differenceApproximator.setN(graph->getNumberOfVertices());
    set<int> seedSet;
    if(setting==setting3) {
        if(!extendPermutation) {
            seedSet = differenceApproximator.executeAlgorithmApproximatingOneFunction(setting, budget);
        } else {
            seedSet = differenceApproximator.executeAlgorithmApproximatingOneFunctionExtendPermutation(setting, budget);
        }
    } else if(setting==setting5) {
        seedSet = differenceApproximator.executeAlgorithmModularG(budget);
    } else if(setting==setting6) {
        seedSet = differenceApproximator.executeSupSubProcedure(budget);

    } else if (setting==setting7) {
        seedSet = differenceApproximator.executeGreedyAlgorithmOnDS(budget);
        IMResults::getInstance().addGreedySolutions(differenceApproximator.getGreedySolutions());
    } else {
        if(!extendPermutation) {
            seedSet = differenceApproximator.executeGreedyAlgorithm(setting, budget);
        } else {
            seedSet = differenceApproximator.executeGreedyAlgorithmAdjustingPermutation(setting, budget);
        }
    }


    clock_t differenceEndTime = clock();
    double differenceTimeTaken = double(differenceEndTime - differenceStartTime) / CLOCKS_PER_SEC;
    TIMInfluenceCalculator  timInfluenceCalculator(graph, 2);
    pair<int, int> influence = timInfluenceCalculator.findInfluence(seedSet);
    cout <<"\n Results: ";
    cout << "\nInfluence Targets: " << influence.first;
    cout << "\nInfluence NT: " << influence.second;
    IMSeedSet imSeedSet;
    for(int seed: seedSet) {
        imSeedSet.addSeed(seed);
    }
    imSeedSet.setTargets(influence.first);
    imSeedSet.setNonTargets(influence.second);
    IMResults::getInstance().addBestSeedSet(imSeedSet);
    IMResults::getInstance().setTotalTimeTaken(differenceTimeTaken);
    IMResults::getInstance().setApproximationInfluence(influence);
    IMResults::getInstance().setExpectedTargets(influence);

}

void executeDifferenceAlgorithms(cxxopts::ParseResult result) {
    cout << "\n Executing difference" << flush;
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    ApproximationSetting setting = static_cast<ApproximationSetting>(result["approximation"].as<int>());
    bool extendPermutation = false;
    if(result.count("extend")>0) {
        extendPermutation = result["extend"].as<bool>();
    }
    cout << "\n Conducting experiments for:\n";
    cout <<" Graph: " << graphFileName;
    cout << "\t Budget: " << budget;
    cout << "\t Percentage:  " << percentageTargets;
    cout << "\t Approximation setting: " << setting;
    cout << "\t Extend: " << extendPermutation;
    cout << flush;

    Graph *graph = createGraphObject(result);
    loadResultsFileFrom(result);
    loadGraphSizeToResults(graph);
//    Begin f-g
    testApprox(graph, budget, setting, extendPermutation);
    IMResults::getInstance().setApproximationSetting(static_cast<int>(setting));
    IMResults::getInstance().setExtendingPermutation(extendPermutation);
    // Setting 1000 as NT threshold. Actually not applicable. TODO: do this better.
    string resultFile = constructResultFileName(graphFileName, budget, percentageTargets, setting);
    IMResults::getInstance().writeToFile(resultFile);
    delete graph;
}

int main(int argc, char **argv) {
    cout << "Starting program\n";
    sfmt_t sfmt;
    srand(time(0));
    sfmt_init_gen_rand(&sfmt, rand());

    setupLogger();
    cout << "Setup logger \n";
    cxxopts::Options options("Targeted Influence Maximization", "Experiments and research.");
    options.add_options()
            ("algorithm", "Choose which algorithm to run", cxxopts::value<std::string>())
            ("graph", "Graph file name", cxxopts::value<std::string>())
            ("b,budget", "Budget size", cxxopts::value<int>())
            ("t,threshold", "NT threshold", cxxopts::value<int>())
            ("m,method", "TIM-TIM or SIM-SIM", cxxopts::value<int>())
            ("percentage", "Percentage of Targets", cxxopts::value<int>())
            ("n,ntfile", "Non Targets File name", cxxopts::value<std::string>())
            ("p,probability", "Propogation probability", cxxopts::value<double>())
            ("approximation", " Approximation Settings", cxxopts::value<int>())
            ("e,extend", "Extend the permutation")
            ("labelMethod", "Labelling Strategy", cxxopts::value<int>())
            ("decompositionFile", "Decomposition File", cxxopts::value<string>())
            ("nBuckets", "Number of Buckets", cxxopts::value<int>())
            ("model", "Cascade Model", cxxopts::value<string>());
    auto result = options.parse(argc, argv);
    executeDifferenceAlgorithms(result);
    disp_mem_usage("");
    return 0;
}
