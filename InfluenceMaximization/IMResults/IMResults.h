//
//  IMResults.h
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 9/17/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef IMResults_h
#define IMResults_h

#include "../IMTree.hpp"
#include "../ApproximationSetting.hpp"
using json = nlohmann::json;

using namespace std;

class IMResults {
public:
    
    static IMResults& getInstance()
    {
        static IMResults instance;
        return instance;
    }
private:
    json data;
    IMResults() {}                    

public:
    IMResults(IMResults const&) = delete;
    void operator=(IMResults const&) = delete;
    
    void setPhase1Time(double timeTaken) {
        data["phase1Time"] = timeTaken;
    }
    
    void setPhase2Time(double timeTaken) {
        data["phase2Time"] = timeTaken;
    }
    
    void writeToFile(string filePath) {
        std::ofstream o(filePath);
        o << std::setw(4) << data << std::endl;
    }
    
    void addSeedSets(vector<IMSeedSet> allSeedSets) {
        
        vector<json> all;
        for(IMSeedSet seedSet:allSeedSets) {
            json s;
            vector<int> seedVector = seedSet.getSeedVector();
            reverse(seedVector.begin(), seedVector.end());
            s["seedSet"] = seedVector;
            s["targets"] = seedSet.getTargets();
            s["nonTargets"] = seedSet.getNonTargets();
            all.push_back(s);
        }
        
        data["allSeedSets"] = all;
    }
    
    void addBestSeedSet(IMSeedSet bestSeedSet) {
        json best;
        vector<int> seedVector = bestSeedSet.getSeedVector();
        reverse(seedVector.begin(), seedVector.end());
        best["seedSet"] = seedVector;
        best["targets"] = bestSeedSet.getTargets();
        best["nonTargets"] = bestSeedSet.getNonTargets();
        data["bestSeedSet"] = best;
    }
    
    void setNonTargetFileName(string nonTargetFileName) {
        data["nonTargetFileName"] = nonTargetFileName;
    }
    
    void setFromFile(bool fromFile) {
        data["fromFile"] = fromFile;
    }
    
    void setPhase1RRSets(int numberOfRRSets) {
        data["phase1RRSets"] = numberOfRRSets;
    }
    
    void setPhase2RRSets(int numberOfRRSets) {
        data["phase2RRSets"] = numberOfRRSets;
    }
    
    void setExpectedTargets(pair<int,int> influence) {
        data["expectedTargets"] = influence.first;
        data["expectedNonTargets"] = influence.second;
    }
    
    void setApproximationInfluence(pair<int, int> influence) {
        data["approximationTargetsExpected"] = influence.first;
        data["approximationNonTargetsExpected"] = influence.second;
    }
    void setApproximationValue(int value) {
        data["approximationValue"] = value;
    }
    void setApproximationTime(double value) {
        data["approximationTime"] = value;
    }
    void setApproximationSetting(ApproximationSetting setting) {
        data["approximationSetting"] = static_cast<int>(setting);
    }
    void setExtendingPermutation(bool extend) {
        data["extend"] = extend;
    }
    
    void setPropagationProbability(double value) {
        data["propagationProbability"] = value;
    }
    
    void setPropagationProbability(string value) {
        data["propagationProbability"] = value;
    }
    
    void setBudget(int budget) {
        data["budget"] = budget;
    }
    
    void setNonTargetThreshold(int threshold) {
        data["nonTargetThreshold"] = threshold;
    }
    
    void setPercentageTargets(int percentage) {
        data["percentageTargets"] = percentage;
    }
    
    void setPercentageNonTargets(int percentage) {
        data["percentageNonTargets"] = percentage;
    }
    
    void setAlgorithm(string algorithm) {
        data["algorithm"] = algorithm;
    }
    
    void setTotalTimeTaken(double timeTaken) {
        data["totalTimeTaken"] = timeTaken;
    }
    
    void setGraphName(string graphName) {
        data["graphName"] = graphName;
    }
    
    void setNumberOfVertices(int n) {
        data["numberOfVertices"] = n;
    }
    
    void setNumberOfEdges(int m) {
        data["numberOfEdges"] = m;
    }
};
#endif /* IMResults_h */
