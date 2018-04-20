//
//  main.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 8/4/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#include <iostream>
#include "cxxopts.hpp"
#include "InfluenceMaximization/Graph.hpp"
#include "InfluenceMaximization/IMTree.hpp"
#include "InfluenceMaximization/EstimateNonTargets.hpp"
#include "InfluenceMaximization/TIMUtility.hpp"
#include "InfluenceMaximization/Phase2.hpp"
#include "InfluenceMaximization/Diffusion.hpp"
#include "InfluenceMaximization/IMResults/IMResults.h"
#include "InfluenceMaximization/memoryusage.h"
#include <string>
#include <chrono>
#include "InfluenceMaximization/log.h"
#include "InfluenceMaximization/DifferenceApproximator.hpp"
#include "InfluenceMaximization/GenerateGraphLabels.hpp"
#include "InfluenceMaximization/BaselineGreedy.hpp"
#include "InfluenceMaximization/BaselineGreedyTIM.hpp"
#include "InfluenceMaximization/Diffusion.hpp"

#include <iomanip>
#include <ctime>
#include <sstream>

using json = nlohmann::json;

#define PHASE1TIM_PHASE2TIM 1;
#define PHASE1SIM_PHASE2SIM 2;

void setupLogger() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer,sizeof(buffer),"%d-%m-%Y-%I:%M:%S",timeinfo);
    std::string str(buffer);
    FILELog::ReportingLevel() = logDEBUG3;
    string logFileName = "logs/influence-" + str + ".log";
    FILE* log_fd = fopen( logFileName.c_str(), "w" );
    Output2FILE::Stream() = log_fd;
}

Graph *createGraphObject(cxxopts::ParseResult result) {
    string graphFile = result["graph"].as<string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    LabelSetting labelSetting = LabelSettingUniform;
    if(result["labelMethod"].count()>0) {
        labelSetting = static_cast<LabelSetting>(result["labelMethod"].as<int>());
    }
    Graph *graph = new Graph;
    graph->readGraph(graphFile, percentageTargetsFloat, labelSetting);
    return graph;
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
    TIMInfluenceCalculator  timInfluenceCalculator(graph, 2);

    
    clock_t differenceEndTime = clock();
    double differenceTimeTaken = double(differenceEndTime - differenceStartTime) / CLOCKS_PER_SEC;
    
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

void loadResultsFileFrom(cxxopts::ParseResult result) {
    // Necessary paramters
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    string algorithm = result["algorithm"].as<string>();
    IMResults::getInstance().setBudget(budget);
    IMResults::getInstance().setGraphName(graphFileName);
    IMResults::getInstance().setPercentageTargets(percentageTargets);
    IMResults::getInstance().setAlgorithm(algorithm);
    
    // Optional parameters
    if(result["threshold"].count()>0) {
        int nonTargetThreshold = result["threshold"].as<int>();
        IMResults::getInstance().setNonTargetThreshold(nonTargetThreshold);
    }
    IMResults::getInstance().setPropagationProbability("inDegree");
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        IMResults::getInstance().setPropagationProbability(probability);
    }
    
    if(result.count("ntfile")>0) {
        string nonTargetsFileName = result["ntfile"].as<std::string>();
        IMResults::getInstance().setFromFile(true);
        IMResults::getInstance().setNonTargetFileName(nonTargetsFileName);
    }
    if(result.count("labelMethod")>0) {
        LabelSetting setting = static_cast<LabelSetting>(result["labelMethod"].as<int>());
        IMResults::getInstance().setLabelMethod(setting);
    }
}

void loadGraphSizeToResults(Graph *graph) {
    IMResults::getInstance().setNumberOfVertices(graph->getNumberOfVertices());
    IMResults::getInstance().setNumberOfEdges(graph->getNumberOfEdges());
}

void executeTIMTIM(cxxopts::ParseResult result) {
    cout << "\n begin execution tim tim ";
    int budget;
    int nonTargetThreshold;
    string graphFileName;
    int percentageTargets;
    bool fromFile = false;
    string nonTargetsFileName;
    int method = PHASE1TIM_PHASE2TIM;
    bool useIndegree = true;
    float probability = 0;
    budget = result["budget"].as<int>();
    nonTargetThreshold = result["threshold"].as<int>();
    graphFileName = result["graph"].as<std::string>();
    percentageTargets = result["percentage"].as<int>();
    loadResultsFileFrom(result);
    
    if(result.count("method")>0) {
        method = result["method"].as<int>();
    }
    if(result.count("p")>0) {
        probability = result["p"].as<double>();
        useIndegree = false;
    }
    if(result.count("ntfile")>0) {
        nonTargetsFileName = result["ntfile"].as<std::string>();
        fromFile = true;
    }
    
    // Log information
    cout << "\n Conducting experiments for:\n";
    cout <<" Graph: " << graphFileName;
    cout << "\t Budget: " << budget;
    cout << "\t Non Target Threshod: " << nonTargetThreshold;
    cout << "\t Percentage:  " << percentageTargets;
    cout << "\t Method: " << method;
    if(useIndegree) {
        cout << "\t Probability: Indegree";
    } else {
        cout << "\t Probability: " <<  probability;
    }
    if(fromFile) {
        cout << "\n Reading Non targets from file: " << nonTargetsFileName;
    }
    
    FILE_LOG(logDEBUG) << "\n Conducting experiments for:\n";
    FILE_LOG(logDEBUG) <<" Graph: " << graphFileName;
    FILE_LOG(logDEBUG) << "\t Budget: " << budget;
    FILE_LOG(logDEBUG) << "\t Non Target Threshod: " << nonTargetThreshold;
    FILE_LOG(logDEBUG) << "\t Percentage:  " << percentageTargets;
    FILE_LOG(logDEBUG) << "\t Method: " << method;
    if(fromFile) {
        FILE_LOG(logDEBUG) << "\n Reading Non targets from file: " << nonTargetsFileName;
    }

    IMResults::getInstance().setFromFile(fromFile);
    // insert code here...
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    Graph *graph = createGraphObject(result);
    if(!useIndegree) {
        graph->setPropogationProbability(probability);
    }
    loadGraphSizeToResults(graph);
    vector<double> nodeCounts;
    clock_t phase1StartTime = clock();
    cout << "\n Before any estimate non targets creation:";
    disp_mem_usage("");
    
    EstimateNonTargets *estimateNonTargets = NULL;
    if(!fromFile) {
        estimateNonTargets = new EstimateNonTargets(graph);
        if(method==1) {
            nodeCounts = estimateNonTargets->getNonTargetsUsingTIM();
        } else {
            nodeCounts = estimateNonTargets->getNonTargetsUsingSIM();
        }
    } else {
        estimateNonTargets = new EstimateNonTargets();
        estimateNonTargets->readFromFile(nonTargetsFileName);
        nodeCounts = *estimateNonTargets->getAllNonTargetsCount();
        delete estimateNonTargets;
    }
    cout << "\n Non targets file is alive ";
    disp_mem_usage("");
    clock_t phase1EndTime = clock();
    FILE_LOG(logDEBUG) << "Completed Phase 1";
    double phase1TimeTaken = double(phase1EndTime - phase1StartTime) / CLOCKS_PER_SEC;
    IMResults::getInstance().setPhase1Time(phase1TimeTaken);
    if(!fromFile) {
        nonTargetsFileName = graphFileName;
        nonTargetsFileName+="_" + to_string(budget);
        nonTargetsFileName+="_" + to_string(nonTargetThreshold);
        nonTargetsFileName+="_" + to_string(percentageTargets);
        nonTargetsFileName+="_" + to_string(rand() % 1000000);
        nonTargetsFileName+="_1";
        nonTargetsFileName+=".txt";
        estimateNonTargets->writeToFile(nonTargetsFileName);
        cout << "\nWriting Non Targets to file " << nonTargetsFileName;
        cout << "\n";
        IMResults::getInstance().setNonTargetFileName(nonTargetsFileName);
        delete estimateNonTargets;
    }
    
    cout << "\n Non Target file is dead";
    disp_mem_usage("");
    cout << "\n Should be same as before" << flush;
    
    //Start phase 2
    FILE_LOG(logDEBUG) << "Starting phase 2";
    clock_t phase2StartTime = clock();
    Phase2 *phase2= NULL;
    if(method==1) {
        phase2 = new Phase2TIM(graph);
    }
    else {
        phase2 = new Phase2SIM(graph);
    }
    phase2->doPhase2(budget, nonTargetThreshold, nodeCounts);
    IMResults::getInstance().addBestSeedSet(phase2->getTree()->getBestSeedSet(budget));
    clock_t phase2EndTime = clock();
    double phase2TimeTaken = double(phase2EndTime - phase2StartTime) / CLOCKS_PER_SEC;
    FILE_LOG(logDEBUG) << "Completed phase 2";
    IMResults::getInstance().setPhase2Time(phase2TimeTaken);
    IMResults::getInstance().setTotalTimeTaken(phase1TimeTaken + phase2TimeTaken);
    
    
    
    vector<IMSeedSet> allSeedSets = phase2->getTree()->getAllSeeds(budget);
    IMResults::getInstance().addSeedSets(allSeedSets);
    
    //Construct results file name
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(nonTargetThreshold);
    resultFileName+="_" + to_string(percentageTargets);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+="_1";
    resultFileName+=".json";
    
    IMSeedSet bestSeedSet = phase2->getTree()->getBestSeedSet(budget);
    delete phase2;
    graph->clearRandomRRSets();
    TIMInfluenceCalculator  *timInfluenceCalculator = new TIMInfluenceCalculator(graph, 2);
    pair<int, int> influenceOfBestSeedSet = timInfluenceCalculator->findInfluence(bestSeedSet.getSeedSet());
    int targetsActivated = influenceOfBestSeedSet.first;
    int nonTargetsActivated = influenceOfBestSeedSet.second;
    delete timInfluenceCalculator;
    delete graph;
    
    cout << "\n Targets activated = " << targetsActivated;
    cout << "\n Non targets activated = " << nonTargetsActivated;
    
    IMResults::getInstance().setExpectedTargets(make_pair(targetsActivated, nonTargetsActivated));
    IMResults::getInstance().writeToFile(resultFileName);
    FILE_LOG(logDEBUG) << "Writing results to file " << resultFileName;
}

string constructResultFileName(string graphFileName, int budget, int nonTargetThreshold, int percentageTargets, ApproximationSetting setting) {
    string resultFileName = "results/" + graphFileName;
    resultFileName+="_" + to_string(budget);
    resultFileName+="_" + to_string(nonTargetThreshold);
    resultFileName+="_" + to_string(percentageTargets);
    resultFileName+="_" + to_string(rand() % 1000000);
    resultFileName+="_" + to_string(setting);
    resultFileName+=".json";
    return resultFileName;
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
    IMResults::getInstance().setApproximationSetting(setting);
    IMResults::getInstance().setExtendingPermutation(extendPermutation);
    // Setting 1000 as NT threshold. Actually not applicable. TODO: do this better.
    string resultFile = constructResultFileName(graphFileName, budget, 1000, percentageTargets, setting);
    IMResults::getInstance().writeToFile(resultFile);
    delete graph;
}

void executeTIMOnLabelledGraph(cxxopts::ParseResult result, bool modular) {
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    Graph *unlabelledGraph = new Graph;
    unlabelledGraph->readGraph(graphFileName, 1.0f);
    
    clock_t timStartTime = clock();
    int n = unlabelledGraph->getNumberOfVertices();
    double epsilon = (double)EPSILON;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    //    R = 23648871;
    unlabelledGraph->generateRandomRRSets(R, true);
    vector<vector<int>>* rrSets = unlabelledGraph->getRandomRRSets();
    
    vector<vector<int>> lookupTable;
    TIMCoverage *timCoverage = new TIMCoverage(&lookupTable);
    timCoverage->initializeLookupTable(rrSets, n);
    timCoverage->initializeDataStructures(R, n);
    set<int> seedSet;
    if(!modular) {
        seedSet = timCoverage->findTopKNodes(budget, rrSets);
    } else {
        seedSet = timCoverage->findTopKNodesModular(budget);
    }
    
    clock_t timEndTime = clock();
    double timTimeTaken = double(timEndTime - timStartTime) / CLOCKS_PER_SEC;
    
    delete timCoverage;
    delete unlabelledGraph;
    
    Graph *labelledGraph = createGraphObject(result);
    loadResultsFileFrom(result);
    
    TIMInfluenceCalculator  timInfluenceCalculator(labelledGraph, 2);
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
    
    IMResults::getInstance().setTotalTimeTaken(timTimeTaken);
    IMResults::getInstance().setExpectedTargets(influence);
    IMResults::getInstance().addBestSeedSet(imSeedSet);
    
    string resultFile = constructResultFileName(graphFileName, budget, 1000, percentageTargets, setting1);
    IMResults::getInstance().writeToFile(resultFile);
}

void executeBaselineGreedy(cxxopts::ParseResult result) {
    cout << "\n Executing baseline greedy" << flush;
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    int nonTargetThreshold = result["threshold"].as<int>();
    
    loadResultsFileFrom(result);
    
    Graph *graph = createGraphObject(result);
    if(result.count("p")>0) {
        double probability = result["p"].as<double>();
        graph->setPropogationProbability(probability);
    }
    loadGraphSizeToResults(graph);
    
    clock_t baselineStartTime = clock();
    BaselineGreedyTIM baselineGreedyTIM;
    set<int> seedSet = baselineGreedyTIM.findSeedSet(graph, budget, nonTargetThreshold);
    clock_t baselineEndTime = clock();
    double baselineTimeTaken = double(baselineEndTime - baselineStartTime) / CLOCKS_PER_SEC;
    
    
    TIMInfluenceCalculator  timInfluenceCalculator(graph, 2);
    pair<int, int> influence = timInfluenceCalculator.findInfluence(seedSet);
    cout <<"\n Results after Diffusion: ";
    cout << "\nInfluence Targets: " << influence.first;
    cout << "\nInfluence NT: " << influence.second;
    FILE_LOG(logDEBUG) << "\n Time Taken: " << baselineTimeTaken;
    vector<int> orderedSeed = baselineGreedyTIM.getOrderedSeed();
    set<int> greedySeedSet;
    vector<IMSeedSet> allSeedSets;
    for (int i=(int)(orderedSeed.size()-1); i<orderedSeed.size(); i++) {
        greedySeedSet.insert(orderedSeed[i]);
        TIMInfluenceCalculator  timInfluenceCalculatorGreedy(graph, 2);
        pair<int, int> seedSetInfluence = timInfluenceCalculatorGreedy.findInfluence(greedySeedSet);
        IMSeedSet imSeedSet;
        vector<int> seedVector(orderedSeed.begin(), orderedSeed.begin() + i + 1);
        // Reverse this before adding so the last seed is first
        reverse(seedVector.begin(), seedVector.end());
        for (int s: seedVector) {
            imSeedSet.addSeed(s);
        }
        imSeedSet.setTargets(seedSetInfluence.first);
        imSeedSet.setNonTargets(seedSetInfluence.second);
        allSeedSets.push_back(imSeedSet);
    }
    IMResults::getInstance().addSeedSets(allSeedSets);
    IMResults::getInstance().addBestSeedSet(allSeedSets[0]);
    
    IMResults::getInstance().setTotalTimeTaken(baselineTimeTaken);
    string resultFile = constructResultFileName(graphFileName, budget, nonTargetThreshold, percentageTargets, setting1);
    IMResults::getInstance().setExpectedTargets(influence);
    IMResults::getInstance().writeToFile(resultFile);
    delete graph;
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

void executeHeuristic3(cxxopts::ParseResult result) {
    cout << "\n Executing Heuristic 2" << flush;
}

void generateGraphLabels(cxxopts::ParseResult result) {
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    LabelSetting labelSetting = LabelSettingUniform;
    if(result["labelMethod"].count()>0) {
        labelSetting = static_cast<LabelSetting>(result["labelMethod"].as<int>());
    }
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    Graph *graph = new Graph;
    graph->readGraph(graphFileName, 1);
    GenerateGraphLabels(graph, percentageTargetsFloat, labelSetting);
    delete graph;
}

void createLabelFileIfNotExists(cxxopts::ParseResult result) {
    string graphName = result["graph"].as<string>();
    LabelSetting labelMethod = LabelSettingUniform;
    if(result["labelMethod"].count()>0) {
        labelMethod = static_cast<LabelSetting>(result["labelMethod"].as<int>());
    }
    
    float percentage = (float)result["percentage"].as<int>()/(float)100;
    string labelFileName = Graph::constructLabelFileName(graphName, percentage, labelMethod);
    ifstream labelFile(labelFileName);
    if (!labelFile.good()) {
        generateGraphLabels(result);
    }
    
}

int main(int argc, char **argv) {
    cout << "Starting program\n";
    srand(time(0));
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
    ("labelMethod", "Labelling Strategy", cxxopts::value<int>());
    auto result = options.parse(argc, argv);
    string algorithm = result["algorithm"].as<string>();
    if(result["algorithm"].count()>0 && algorithm.compare("generate")==0) {
        generateGraphLabels(result);
    } else{
        createLabelFileIfNotExists(result);
        if(result["algorithm"].count()>0 && algorithm.compare("timtim")==0) {
            executeTIMTIM(result);
        } else if(result["algorithm"].count()>0 && algorithm.compare("tim")==0) {
            cout << "\n Executing just TIM";
            executeTIMOnLabelledGraph(result, false);
            
        } else if(result["algorithm"].count()>0 && algorithm.compare("timmodular")==0 ) {
            executeTIMOnLabelledGraph(result, true);
        } else if(result["algorithm"].count()>0 && algorithm.compare("baseline")==0 ) {
            executeBaselineGreedy(result);
        } else if(result["algorithm"].count()>0 && algorithm.compare("heuristic2")==0 ) {
            executeHeuristic2(result);
        } else if(result["algorithm"].count()>0 && algorithm.compare("heuristic3")==0 ) {
            executeHeuristic3(result);
        }else {
            executeDifferenceAlgorithms(result);
        }
    }
    
    disp_mem_usage("");
    return 0;
}
