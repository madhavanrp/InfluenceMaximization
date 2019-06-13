//
// Created by Madhavan R.P on 2019-06-11.
//

#include "main_utils.h"
#include "InfluenceMaximization/Graph/Graph.hpp"
#include "InfluenceMaximization/IMResults/IMResults.h"
#include "InfluenceMaximization/Attitude/AttitudeCalculator.hpp"

string constructResultFileName(string graphFileName, int budget, int percentageTargets) {
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(percentageTargets);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+=".json";
    return resultFileName;
}

void executeGreedy(Graph *graph, int budget) {
    clock_t startTime = clock();
    set<int> seedSet;
//    AttitudeCalculator *attitudeCalculator = new AttitudeCalculator(graph);
//
//    vector<vector<int>> *rrSets = attitudeCalculator->getRRsets();
//    seedSet = attitudeCalculator->getTIMCoverage().get()->findTopKNodes(budget, rrSets);
//    rrSets = NULL;
//    clock_t endTime = clock();
//    double totalTimeTaken = double(endTime - startTime) / CLOCKS_PER_SEC;
//    int covered = attitudeCalculator->getTIMCoverage().get()->getNumberOfRRSetsCovered();
//
//    int att = round((double) covered * attitudeCalculator->getScalingFactor());
//
//    delete attitudeCalculator;
//    cout << "\n Precalculated attitude: "<<att << flush;
//    TIMInfluenceCalculator  *timInfluenceCalculator = new TIMInfluenceCalculator(graph, 2);
//    pair<int, int> influence = timInfluenceCalculator->findInfluence(seedSet);
//    delete timInfluenceCalculator;
//
//    cout <<"\n Results: ";
//    cout << "\nInfluence Targets: " << influence.first;
//    cout << flush;
//
//    attitudeCalculator = new AttitudeCalculator(graph);
//    int attitude = attitudeCalculator->findAttitude(seedSet);
//    delete attitudeCalculator;
//    cout << "\n Attitude: "<< attitude;
//    cout << flush;

    //Now executing the normal influence maximization
    cout << "\n\n Compare against standard influence maximizing seed set";
    TIMInfluenceCalculator *timInfluenceCalculator = new TIMInfluenceCalculator(graph, 2);
    seedSet = timInfluenceCalculator->getTimCoverageTargets()->findTopKNodes(budget, timInfluenceCalculator->getRRsetsTargets());
    delete timInfluenceCalculator;

    AttitudeCalculator *attitudeCalculator = new AttitudeCalculator(graph);
    int attitude = attitudeCalculator->findAttitude(seedSet);
    delete attitudeCalculator;

    cout << "\n Attitude: "<< attitude;
    cout << flush;
//    IMSeedSet imSeedSet;
//    for(int seed: seedSet) {
//        imSeedSet.addSeed(seed);
//    }
//    imSeedSet.setTargets(influence.first);
//    imSeedSet.setNonTargets(influence.second);
//    IMResults::getInstance().addBestSeedSet(imSeedSet);
//    IMResults::getInstance().setTotalTimeTaken(totalTimeTaken);
//    IMResults::getInstance().setApproximationInfluence(influence);
//    IMResults::getInstance().setExpectedTargets(influence);

}

void executeGreedyAttitudeMaximization(cxxopts::ParseResult result) {
    cout << "\n Executing Attitude" << flush;
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = 100;

    cout << "\n Conducting experiments for:\n";
    cout <<" Graph: " << graphFileName;
    cout << "\t Budget: " << budget;
    cout << "\t Percentage:  " << percentageTargets;
    cout << flush;



    Graph *graph = createGraphObject(result);
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        graph->setPropogationProbability(probability);
    }
    if(result.count("trivalency")>0) {
        bool trivalency = result["trivalency"].as<bool>();
        if(trivalency) {
            graph->generateEdgeProbabilitiesTrivalencyModel();
        }
    }
    loadResultsFileFrom(result);
    loadGraphSizeToResults(graph);

    executeGreedy(graph, budget);
    string resultFile = constructResultFileName(graphFileName, budget, percentageTargets);
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
            ("model", "Cascade Model", cxxopts::value<string>())
                    ("trivalency", "Trivalency Model");
    auto result = options.parse(argc, argv);
    executeGreedyAttitudeMaximization(result);
    disp_mem_usage("");
    return 0;
}