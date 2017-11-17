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
TIMCoverage* createLargetTIMCoverage() {
    vector<vector<int>> *lookupTable = new vector<vector<int>>();
    for (int i = 0; i<50000000; i++) {
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
    REQUIRE(timCoverage.countForVertex(3)==10);
    
    timCoverage.decrementCountForVertex(3, 0);
    REQUIRE((*timCoverage.lookupTable)[3].size()==9);
    REQUIRE(timCoverage.countForVertex(3)==9);
    REQUIRE((*timCoverage.lookupTable)[3][0]==1);
    
    timCoverage.incrementCountForVertex(2, 40);
    REQUIRE((*timCoverage.lookupTable)[2].size()==11);
    REQUIRE((*timCoverage.lookupTable)[2][10]==40);
    
    for (int i = 0; i<5; i++) {
        REQUIRE((*timCoverage.lookupTable)[i].size()==timCoverage.countForVertex(i));
    }
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

TEST_CASE("Initialize Data Structures", "TIMCoverage") {
    TIMCoverage *timCoverage = createTIMCoverage();
    int R = (int)timCoverage->lookupTable->size();
    int n = rand() % R;
    timCoverage->initializeDataStructures(R, n);
    REQUIRE(timCoverage->edgeMark.size()==R);
    REQUIRE(timCoverage->nodeMark.size()==n);
    REQUIRE(timCoverage->coverage.size()==n);
    REQUIRE(timCoverage->queue.size()==n);
}

//TEST_CASE("Copy larget tim coverage", "TIMCoverage") {
//    TIMCoverage *timCoverage = createLargetTIMCoverage();
//    int R = (int)timCoverage->lookupTable->size();
//    int n = 613586;
//    timCoverage->initializeDataStructures(R, n);
//    
//    REQUIRE(timCoverage->edgeMark.size()==R);
//    REQUIRE(timCoverage->nodeMark.size()==n);
//    REQUIRE(timCoverage->coverage.size()==n);
//    REQUIRE(timCoverage->queue.size()==n);
//}


TEST_CASE("Offset coverage", "TIMCoverage") {
    vector<vector<int>> rrSets = vector<vector<int>>();
    int R = 50;
    for (int i=0; i< R; i++) {
        rrSets.push_back(vector<int>());
        for(int j=0; j< (R-i) ; j++) {
            rrSets[i].push_back(j);
        }
    }
    vector<vector<int>> lookupTable;
    TIMCoverage *timCoverage = new TIMCoverage(&lookupTable);
    int n = R;
    timCoverage->initializeLookupTable(rrSets, n);
    timCoverage->initializeDataStructures(R, n);
    timCoverage->offsetCoverage(0, -10);
    // 0 should not be the top
    set<int> topNodes = timCoverage->findTopKNodes(1, &rrSets);
    REQUIRE(topNodes.size()==1);
    REQUIRE(topNodes.find(0)==topNodes.end());
    delete timCoverage;
}

TEST_CASE("Add to seed", "TIMCoverage") {
    vector<vector<int>> rrSets = vector<vector<int>>();
    int R = 50;
    for (int i=0; i< R; i++) {
        rrSets.push_back(vector<int>());
        for(int j=0; j< (R-i) ; j++) {
            rrSets[i].push_back(j);
        }
    }
    vector<vector<int>> lookupTable;
    TIMCoverage *timCoverage = new TIMCoverage(&lookupTable);
    int n = R;
    timCoverage->initializeLookupTable(rrSets, n);
    timCoverage->initializeDataStructures(R, n);
    timCoverage->offsetCoverage(0, -10);
    // 0 should not be the top
    int nodeToAdd = 4;
    vector<int> rrSetsCovered = timCoverage->getRRSetsCoveredByVertex(nodeToAdd);
    timCoverage->addToSeed(nodeToAdd, &rrSets);
    for(int rrSet: rrSetsCovered) {
        REQUIRE(timCoverage->edgeMark[rrSet]);
    }
    REQUIRE(!timCoverage->nodeMark[nodeToAdd]);
    delete timCoverage;
}

TEST_CASE("TIM Coverage queue copy", "TIMCoverage") {
    TIMCoverage *timCoverage = createTIMCoverage();
    int R = (int)timCoverage->lookupTable->size();
    int n = 3;
    timCoverage->initializeDataStructures(R, n);
    REQUIRE(timCoverage->queue.size()==n);
    priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator> *copy = new priority_queue<pair<int, int>, vector<pair<int, int>>, QueueComparator>(timCoverage->queue);
    REQUIRE(copy->size()==n);
    copy->pop();
    REQUIRE(copy->size()==n-1);
    copy->top();
    REQUIRE(copy->size()==n-1);
    REQUIRE(timCoverage->queue.size()==n);
    
    delete copy;
}

#endif
