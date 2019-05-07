//
// Created by Madhavan R.P on 2019-05-07.
//

#include "InfluenceMaximization/TIMInfluenceCalculator.hpp"
#include "InfluenceMaximization/TIM-Models/TIMCoverage.hpp"
#include "main_utils.h"
#include "InfluenceMaximization/Heuristics/HeuristicsExecuter.hpp"

string constructResultFileName(string graphFileName, int budget, int nonTargetThreshold, int percentageTargets) {
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(nonTargetThreshold);
    resultFileName+="_" + to_string(percentageTargets);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+=".json";
    return resultFileName;
}

void executeHeuristic2(cxxopts::ParseResult result) {
    cout << "\n Executing Heuristic 2" << flush;
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;

    loadResultsFileFrom(result);

    Graph *graph = new Graph;
    graph->readGraph(graphFileName, percentageTargetsFloat);
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        graph->setPropogationProbability(probability);
    }
    loadGraphSizeToResults(graph);


    int n = graph->getNumberOfVertices();
    double epsilon = (double)2;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    graph->generateRandomRRSetsWithoutVisitingNonTargets(R);
    vector<vector<int>>* rrSets = graph->getRandomRRSets();
    vector<vector<int>> lookupTable;
    TIMCoverage *timCoverage = new TIMCoverage(&lookupTable);
    timCoverage->initializeLookupTable(rrSets, n);
    timCoverage->initializeDataStructures(R, n);

    set<int> seedSet = timCoverage->findTopKNodes(budget, rrSets);
    delete timCoverage;
    graph->clearRandomRRSets();
    assert(graph->getRandomRRSets()->size()==0);
    TIMInfluenceCalculator  timInfluenceCalculator(graph, 2);

    pair<int, int> influence = timInfluenceCalculator.findInfluence(seedSet);
    cout << "\n Influence: ";
    cout << "\n Targets: " << influence.first;
    cout << "\n Non Targets: " << influence.second;
}

void executeHeuristic(cxxopts::ParseResult result) {
    cout << "\n Executing Heuristic: " << result["algorithm"].as<std::string>() << flush;
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    int nonTargetThreshold = result["threshold"].as<int>();
    loadResultsFileFrom(result);

    Graph *graph = createGraphObject(result);
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        graph->setPropogationProbability(probability);
    }
    loadGraphSizeToResults(graph);

    HeuristicsExecuter h;
    string algorithm = result["algorithm"].as<std::string>();
    set<int> seedSet;
    if(algorithm.compare("heuristic3")==0) {
        seedSet = h.executeNonTargetMinimizer(graph, budget, nonTargetThreshold);
    } else {
        seedSet = h.maximizeOnlyOverTargets(graph, budget, nonTargetThreshold);
    }
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
    if(result["algorithm"].count()>0 && algorithm.compare("heuristic2")==0 ) {
        executeHeuristic2(result);
    } else if(result["algorithm"].count()>0 && algorithm.compare("heuristic3")==0 ) {
        executeHeuristic(result);
    } else if(result["algorithm"].count()>0 && algorithm.compare("heuristic4")==0 ) {
        executeHeuristic(result);
    }

    disp_mem_usage("");
    return 0;
}