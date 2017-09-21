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
            s["seedSet"] = seedSet.getSeedSet();
            s["targets"] = seedSet.getTargets();
            s["nonTargets"] = seedSet.getNonTargets();
            all.push_back(s);
        }
        
        data["allSeedSets"] = all;
    }
    
    void addBestSeedSet(IMSeedSet bestSeedSet) {
        json best;
        best["seedSet"] = bestSeedSet.getSeedSet();
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
};
#endif /* IMResults_h */
