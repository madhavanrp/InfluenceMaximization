//
//  TIMCoverageTests.cpp
//  InfluenceMaximization
//
//  Created by Madhavan R.P on 9/8/17.
//  Copyright © 2017 Madhavan R.P. All rights reserved.
//


#ifndef TIMCoverageTests_cpp
#define TIMCoverageTests_cpp

#include "catch.hpp"
#include "../InfluenceMaximization/TIM-Models/TIMCoverage.hpp"



TIMCoverage* createTIMCoverage() {
    vector<vector<int>> *lookupTable = new vector<vector<int>>();
    for (int i = 0; i<5; i++) {
        (*lookupTable).push_back(vector<int>());
        for (int j = 0; j<10; j++) {
            (*lookupTable)[i].push_back(j);
        }
    }
    return new TIMCoverage(lookupTable);
}
TEST_CASE("TIM Data structures initializes correctly", "TIMCoverage") {
    TIMCoverage timCoverage = TIMCoverage(new vector<vector<int>>());
    
    for (int i = 0; i<100; i++) {
        timCoverage.nodeMark.push_back(i%2==0);
        timCoverage.edgeMark.push_back(i%2==0);
        timCoverage.coverage.push_back(i);
    }
    
    for (int i = 0; i<100; i++) {
        REQUIRE(timCoverage.nodeMark[i]==(i%2==0));
        REQUIRE(timCoverage.edgeMark[i]==(i%2==0));
        REQUIRE(timCoverage.coverage[i]==i);
    }
}
//
TEST_CASE("TIM Data Structures operations", "TIMCoverage") {
    TIMCoverage timCoverage = *createTIMCoverage();
    
    REQUIRE((*timCoverage.lookupTable)[3].size()==10);
    timCoverage.decrementCountForVertex(3, 0);
    REQUIRE((*timCoverage.lookupTable)[3].size()==9);
    REQUIRE((*timCoverage.lookupTable)[3][0]==1);
    
    timCoverage.incrementCountForVertex(2, 40);
    REQUIRE((*timCoverage.lookupTable)[2].size()==11);
    REQUIRE((*timCoverage.lookupTable)[2][10]==40);
}

TEST_CASE("TIM is copied", "TIMCoverage") {
    TIMCoverage *timCoverage = createTIMCoverage();
    for (int i = 0; i<100; i++) {
        timCoverage->nodeMark.push_back(i%2==0);
        timCoverage->edgeMark.push_back(i%2==0);
        timCoverage->coverage.push_back(i);
    }
    TIMCoverage *copy = timCoverage->createCopy();
    timCoverage->edgeMark.erase(timCoverage->edgeMark.begin());
    timCoverage->nodeMark.erase(timCoverage->nodeMark.begin());
    timCoverage->coverage.erase(timCoverage->coverage.begin());

    REQUIRE(timCoverage->edgeMark.size()==99);
    REQUIRE(timCoverage->nodeMark.size()==99);
    REQUIRE(timCoverage->coverage.size()==99);

    REQUIRE(copy->edgeMark.size()==100);
    REQUIRE(copy->nodeMark.size()==100);
    REQUIRE(copy->coverage.size()==100);
    
}

TEST_CASE("Memory management", "TIMCoverage") {
    int originalCount = TIMCoverage::totalCount;
    TIMCoverage *timCoverage = createTIMCoverage();
    
    REQUIRE(TIMCoverage::totalCount==(originalCount+1));
    REQUIRE(timCoverage -> retainCount==1);
    for(int i=0;i<5;i++) {
        timCoverage->retain();
    }
    
    REQUIRE(timCoverage->retainCount==6);
    for(int i=0;i<5;i++) {
        timCoverage->release();
    }
    
    REQUIRE(timCoverage->retainCount==1);
    timCoverage->release();
    REQUIRE(TIMCoverage::totalCount==(originalCount));
    TIMCoverage *copy = timCoverage->createCopy();
    REQUIRE(copy->retainCount==1);
//    delete timCoverage;
    
}

#endif
