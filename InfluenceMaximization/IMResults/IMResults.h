//
//  IMResults.h
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 9/17/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//

#ifndef IMResults_h
#define IMResults_h

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
    
};
#endif /* IMResults_h */
