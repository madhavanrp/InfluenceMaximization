//
// Created by Madhavan R.P on 2019-05-07.
//
#include "main_utils.h"
#include "InfluenceMaximization/DPAlgorithm/HeirarchicalDecomposition.hpp"

string constructResultFileName(string graphFileName, int budget, int nonTargetThreshold, int percentageTargets) {
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(nonTargetThreshold);
    resultFileName+="_" + to_string(percentageTargets);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+=".json";
    return resultFileName;
}

void executeDPAlgorithm(cxxopts::ParseResult result) {
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    string decompositionFile = result["decompositionFile"].as<std::string>();
    loadResultsFileFrom(result);

    Graph *graph = createGraphObject(result);
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        graph->setPropogationProbability(probability);
    }
    loadGraphSizeToResults(graph);
    clock_t startTime = clock();

    set<int> seedSet;

    HeirarchicalDecomposition hDecomp(graph, decompositionFile, budget);
    if (result["algorithm"].as<std::string>().compare("dp")==0) {
        seedSet = hDecomp.maximizeUsingDP();
    } else {
        int nBuckets = result["nBuckets"].as<int>();
        seedSet = hDecomp.divideAndMaximize(nBuckets);
        IMResults::getInstance().setnBuckets(nBuckets);
    }

    clock_t endTime = clock();
    double totalTimeTaken = double(endTime - startTime) / CLOCKS_PER_SEC;

    TIMInfluenceCalculator timInfluenceCalculator(graph, 2);
    pair<int, int> influence = timInfluenceCalculator.findInfluence(seedSet);
    int targetsActivated = influence.first;
    int nonTargetsActivated = influence.second;

    cout << "\n Targets activated = " << targetsActivated;
    cout << "\n Non targets activated = " << nonTargetsActivated;
    IMSeedSet imSeedSet;
    for(int s: seedSet) {
        imSeedSet.addSeed(s);
    }
    imSeedSet.setTargets(influence.first);
    imSeedSet.setNonTargets(influence.second);

    IMResults::getInstance().setExpectedTargets(influence);
    IMResults::getInstance().addBestSeedSet(imSeedSet);
    IMResults::getInstance().setTotalTimeTaken(totalTimeTaken);

    string resultFile = constructResultFileName(graphFileName, budget, 1000, percentageTargets);
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
    string algorithm = result["algorithm"].as<string>();
    createLabelFileIfNotExists(result);
    if(result["algorithm"].count()>0 && algorithm.compare("dp")==0) {
        executeDPAlgorithm(result);
    } else if(result["algorithm"].count()>0 && algorithm.compare("dpdm")==0) {
        executeDPAlgorithm(result);
    }

    disp_mem_usage("");
    return 0;
}
