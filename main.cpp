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

void testApprox(Graph *graph, int budget, ApproximationSetting setting, bool extendPermutation) {
    DifferenceApproximator differenceApproximator(graph);
    differenceApproximator.setN(graph->n);
    set<int> seedSet;
    vector<int> permutation = differenceApproximator.generatePermutation();
    ModularApproximation modularApprox(permutation, setting);
    modularApprox.createTIMEvaluator(graph);
    modularApprox.findAllApproximations();
    if(setting==setting3) {
        if(!extendPermutation) {
            seedSet = differenceApproximator.executeAlgorithmApproximatingOneFunction(setting, budget);
        } else {
            seedSet = differenceApproximator.executeAlgorithmApproximatingOneFunctionExtendPermutation(setting, budget);
        }
    } else {
        if(!extendPermutation) {
            seedSet = differenceApproximator.executeGreedyAlgorithm(graph, &modularApprox, budget);
        } else {
            seedSet = differenceApproximator.executeGreedyAlgorithmAdjustingPermutation(setting, budget);
        }
    }
    pair<int, int> influence = findInfluenceUsingDiffusion(graph, seedSet, NULL);
    cout <<"\n Results after Diffusion: ";
    cout << "\nInfluence Targets: " << influence.first;
    cout << "\nInfluence NT: " << influence.second;
    IMResults::getInstance().setApproximationInfluence(influence);

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
    Graph *graph = new Graph;
    graph->readGraph(graphFileName, percentageTargetsFloat);
    if(!useIndegree) {
        graph->setPropogationProbability(probability);
    }
    loadGraphSizeToResults(graph);
    vector<double> nodeCounts;
    clock_t phase1StartTime = clock();
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
    
    TIMInfluenceCalculator  timInfluenceCalculator(graph, 2);
    pair<int, int> influenceOfBestSeedSet = timInfluenceCalculator.findInfluence(bestSeedSet.getSeedSet());
    int targetsActivated = influenceOfBestSeedSet.first;
    int nonTargetsActivated = influenceOfBestSeedSet.second;
    
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
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
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

    Graph *graph = new Graph;
    graph->readGraph(graphFileName, percentageTargetsFloat);
//    Begin f-g
    clock_t differenceStartTime = clock();
    

    testApprox(graph, budget, setting, extendPermutation);
    clock_t differenceEndTime = clock();
    double differenceTimeTaken = double(differenceEndTime - differenceStartTime) / CLOCKS_PER_SEC;
    IMResults::getInstance().setApproximationTime(differenceTimeTaken);
    IMResults::getInstance().setApproximationSetting(setting);
    IMResults::getInstance().setExtendingPermutation(extendPermutation);
    // Setting 1000 as NT threshold. Actually not applicable. TODO: do this better.
    string resultFile = constructResultFileName(graphFileName, budget, 1000, percentageTargets, setting);
    IMResults::getInstance().writeToFile(resultFile);
}

void executeTIMOnLabelledGraph(cxxopts::ParseResult result) {
    int budget = result["budget"].as<int>();
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    Graph *unlabelledGraph = new Graph;
    unlabelledGraph->readGraph(graphFileName, 1.0f);
    int n = unlabelledGraph->n;
    double epsilon = (double)EPSILON;
    int R = (8+2 * epsilon) * n * (2 * log(n) + log(2))/(epsilon * epsilon);
    //    R = 23648871;
    unlabelledGraph->generateRandomRRSets(R, true);
    vector<vector<int>> rrSets = unlabelledGraph->getRandomRRSets();
    
    vector<vector<int>> lookupTable;
    TIMCoverage *timCoverage = new TIMCoverage(&lookupTable);
    timCoverage->initializeLookupTable(rrSets, n);
    timCoverage->initializeDataStructures(R, n);
    timCoverage->offsetCoverage(0, -10);
    // 0 should not be the top
    set<int> topNodes = timCoverage->findTopKNodes(budget, &rrSets);
    delete timCoverage;
    
    Graph *labelledGraph = new Graph;
    labelledGraph->readGraph(graphFileName, percentageTargetsFloat);
    pair<int, int> influence = findInfluenceUsingDiffusion(labelledGraph, topNodes);
    int targetsActivated = influence.first;
    int nonTargetsActivated = influence.second;
    
    cout << "\n Targets activated = " << targetsActivated;
    cout << "\n Non targets activated = " << nonTargetsActivated;
    IMResults::getInstance().setExpectedTargets(influence);
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
    
    Graph *graph = new Graph;
    graph->readGraph(graphFileName, percentageTargetsFloat);
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

void generateGraphLabels(cxxopts::ParseResult result) {
    string graphFileName = result["graph"].as<std::string>();
    int percentageTargets = result["percentage"].as<int>();
    float percentageTargetsFloat = (float)percentageTargets/(float)100;
    Graph *graph = new Graph;
    graph->readGraph(graphFileName, percentageTargetsFloat);
    GenerateGraphLabels(graph, percentageTargetsFloat);
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
    ("n,ntfile", "Non Targets File name", cxxopts::value<int>())
    ("p,probability", "Propogation probability", cxxopts::value<double>())
    ("approximation", " Approximation Settings", cxxopts::value<int>())
    ("e,extend", "Extend the permutation");
    auto result = options.parse(argc, argv);
    string algorithm = result["algorithm"].as<string>();
    if(result["algorithm"].count()>0 && algorithm.compare("generate")==0) {
        generateGraphLabels(result);
    } else if(result["algorithm"].count()>0 && algorithm.compare("timtim")==0) {
        executeTIMTIM(result);
    } else if(result["algorithm"].count()>0 && algorithm.compare("tim")==0) {
        cout << "\n Executing just TIM";
        executeTIMOnLabelledGraph(result);
        
    } else if(result["algorithm"].count()>0 && algorithm.compare("baseline")==0 ) {
        executeBaselineGreedy(result);
    }else {
        executeDifferenceAlgorithms(result);
    }
    
    disp_mem_usage("");
    return 0;
}
