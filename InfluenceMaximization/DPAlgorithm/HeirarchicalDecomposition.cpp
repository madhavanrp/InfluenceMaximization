//
//  HeirarchicalDecomposition.cpp
//  IMPrototypes
//
//  Created by Madhavan R.P on 6/8/18.
//  Copyright © 2018 Madhavan R.P. All rights reserved.
//

#include "HeirarchicalDecomposition.hpp"

vector<int> HeirarchicalDecomposition::readVertexBuckets(string decompositionFile) {
    ifstream myFile(decompositionFile);
    if(!myFile.good()) {
        throw std::invalid_argument( "Decomposition file does not exist: " + decompositionFile );
    }
    vector<int> vertexBuckets;
    
    int bucket;
    if(myFile.is_open()) {
        while (myFile >> bucket) {
            vertexBuckets.push_back(bucket);
        }
        myFile.close();
    }
    return vertexBuckets;
}

vector<int> HeirarchicalDecomposition::assignOneVertexPerBucket(vector<int> vertexBuckets) {
    set<int> allBuckets;
    vector<bool> bucketOccupied;
    int n = (int)vertexBuckets.size();
    
    for (int i=0; i<n; i++) {
        allBuckets.insert(i);
        bucketOccupied.push_back(false);
    }
    
    for (int b:vertexBuckets) {
        allBuckets.erase(b);
    }
    set<int> availableBuckets = allBuckets;
    
    vector<int> assignedBuckets(n);
    for (int i=0; i<n; i++) {
        int b = vertexBuckets[i];
        if(!bucketOccupied[b]) {
            bucketOccupied[b] = true;
            assignedBuckets[i] = b;
        } else {
            b = *availableBuckets.begin();
            availableBuckets.erase(b);
            assignedBuckets[i] = b;
        }
    }
    return assignedBuckets;
}

vector<int> HeirarchicalDecomposition::getRandomBuckets(int n) {
    vector<int> randomBuckets;
    for (int i=0; i<n; i++) {
        randomBuckets.push_back(i);
    }
    random_shuffle(randomBuckets.begin(), randomBuckets.end());
    return randomBuckets;
}

HeirarchicalDecomposition::HeirarchicalDecomposition(Graph *graph, string decompositionFile, int k) {
    this->partitionFile = decompositionFile;
    this->graph = graph;
    this->timInfluenceCalculator.reset(new TIMInfluenceCalculator(graph, 2));
    this->k = k;
}

void HeirarchicalDecomposition::initializeDataStructuresForDPIM() {
    vector<int> vertexBuckets = readVertexBuckets(this->partitionFile);
    vertexBuckets = assignOneVertexPerBucket(vertexBuckets);
    int n = this->graph->getNumberOfVertices();
    assert(n==vertexBuckets.size());
    this->leaves = vector<int>(n);
    for (int v=0; v<vertexBuckets.size(); v++) {
        int bucket = vertexBuckets[v];
        // v belongs in this bucket
        this->leaves[bucket] = v;
    }
    int height = ceil(log(n)/log(2));
    this->height = height;
    int numberOfNodes = 0;
    for (int h = 0; h<=height; h++) {
        numberOfNodes+=pow(2, h);
    }
    binaryTree = vector<int>(numberOfNodes);
    
    seeds = vector<vector<unordered_set<int>>>(numberOfNodes);
    
    for (int i=0; i<numberOfNodes; i++) {
        seeds[i] = vector<unordered_set<int>>(k+1);
    }
    cout << "\n Value of n is " << n;
    cout << "\n Value of height is  " << height;
    cout << "\n Number of nodes in tree is " << numberOfNodes;
}

void HeirarchicalDecomposition::initializeDataStructuresForDPDM() {
    vector<int> vertexBuckets = readVertexBuckets(this->partitionFile);
    int height = this->height;
    int nLeaves = pow(2, height);
    int nodesPerLeaf = ceil((double)this->leaves.size()/(double)nLeaves);
    for (int i=0; i<nLeaves; i++) {
        this->bucketWithSeeds.push_back(vector<int>());
    }
    int bucket;
    for (int i=0; i<vertexBuckets.size(); i++) {
        bucket = vertexBuckets[i];
        this->bucketWithSeeds[bucket].push_back(i);
    }
}

int HeirarchicalDecomposition::findIndexOfMaximizingSeed(int vertexL, int vertexR, int k) {
    int argmaxJ = -1;
    set<int> solutionSeed;
    double maxDifference = (double)INT_MIN;
    pair<double, double> inf;
    double difference;
    for (int j=0; j<=k; j++) {
        // Find the one that maximizes
        //For now return random
        solutionSeed.insert(this->seeds[vertexL][j].begin(), this->seeds[vertexL][j].end());
        solutionSeed.insert(this->seeds[vertexR][k-j].begin(), this->seeds[vertexR][k-j].end());
        if(solutionSeed.size()>0) {
            inf = timInfluenceCalculator.get()->findInfluenceWithoutUpdatingModel(solutionSeed);
        }
        else {
            inf = make_pair(0, 0);
        }
        difference = inf.first - inf.second;
        if (difference>maxDifference) {
            maxDifference = difference;
            argmaxJ = j;
        }
        solutionSeed.clear();
        assert(solutionSeed.size()==0);
    }
    return argmaxJ;
}

set<int> HeirarchicalDecomposition::maximizeUsingDP() {
    this->initializeDataStructuresForDPIM();
    int height = this->height;
    int v, indexOfNode;
    indexOfNode = pow(2, height) - 1;
    int k = this->k;
    set<int> allNodes;
    for (int bucket=0; bucket<this->leaves.size(); bucket++) {
        // This is the vertex corresponding to the original graph
        v = this->leaves[bucket];
        seeds[indexOfNode][0] = unordered_set<int>();
        seeds[indexOfNode][1] = unordered_set<int>();
        seeds[indexOfNode][1].insert(v);
        allNodes.insert(v);
        indexOfNode++;
    }
    cout << "\n Value of all nodes size is " << allNodes.size();
    for (int h = height-1; h>=0; h--) {
        // For each node at this height
        if(h==0) {
            cout << "\n At the root";
        }
        int nodesAtThisLevel = 0;
        for (int vT=pow(2, h)-1; vT<pow(2, h+1)-1; vT++) {
            int lIndex = 2 *vT + 1;
            int rIndex = lIndex + 1;
            int nLeavesFromCurrent = pow(2, (height - h));
            int c = min(k, nLeavesFromCurrent);
            for (int i=0; i<=c; i++) {
                int j = findIndexOfMaximizingSeed(lIndex, rIndex, i);
                
                seeds[vT][i].insert(seeds[lIndex][j].begin(), seeds[lIndex][j].end());
                seeds[vT][i].insert(seeds[rIndex][i-j].begin(), seeds[rIndex][i-j].end());
//                assert(seeds[vT][i].size()==i);
            }
//            assert(seeds[vT][c].size()>0);
            nodesAtThisLevel++;
            if (nodesAtThisLevel%200==0) {
                cout << "\n Finished number of nodes: " <<nodesAtThisLevel << flush;
            }
        }
        cout << "\n Height is " << h << flush;
    }
    cout <<" \n Seed size = " << seeds[0][k].size();
    set<int> seedSet;
    seedSet.insert(seeds[0][k].begin(), seeds[0][k].end());
    return seedSet;
}

set<int> HeirarchicalDecomposition::divideAndMaximize(int nBuckets) {
    int height = ceil(log(nBuckets)/log(2));
    this->height = height;
    this->initializeDataStructuresForDPDM();
//    int nTree = (int)this->bucketWithSeeds.size();
//    int height = log2(nTree+1) - 1;
    
    vector<int> bestSeed = findBestSeedFromTree(0, 0);
    for (bool edge:*this->timInfluenceCalculator.get()->getTimCoverageTargets()->getEdgeMark()) {
        assert(!edge);
    }
    for (bool n:*this->timInfluenceCalculator.get()->getTimCoverageTargets()->getNodeMark()) {
        assert(n);
    }
    return set<int>(bestSeed.begin(), bestSeed.end());
}

vector<int> HeirarchicalDecomposition::findBestSeedFromCandidates(int k, vector<int> candidateNodes) {
    TIMCoverage *coverageTargets = this->timInfluenceCalculator.get()->getTimCoverageTargets().get();
    set<int> candidates;
    for (int v:candidateNodes) {
        candidates.insert(v);
    }
    return coverageTargets->findTopKNodesFromCandidatesWithoutUpdate(k, this->timInfluenceCalculator.get()->getRRsetsTargets(), candidates).first;
//    return coverageTargets->findTopKNodes(k, this->timInfluenceCalculator.get()->getRRsetsTargets());
}

vector<int> HeirarchicalDecomposition::findBestSeedFromTree(int currentNodeIndex, int currentHeight) {
    int lIndex = 2 *currentNodeIndex + 1;
//    cout << "\n Processing node " << currentNodeIndex +1;
//    cout << "\n At Height " << currentHeight << flush;
    if(currentHeight==this->height) {
        int nNodesAtLevel = pow(2, height);
        int seedIndex = currentNodeIndex % nNodesAtLevel;
        assert(seedIndex<this->bucketWithSeeds.size());
        return findBestSeedFromCandidates(this->k, this->bucketWithSeeds[seedIndex]);
    } else {
        int rIndex = lIndex + 1;
        vector<int> leftSeedVector = findBestSeedFromTree(lIndex, currentHeight+1);
        vector<int> rightSeedVector = findBestSeedFromTree(rIndex, currentHeight+1);
        int k = (int)leftSeedVector.size();
        assert(rightSeedVector.size()==k);
        vector<int> bestSeed;
        int bestInfluence = -1;
        vector<int> currentSeed;
        for (int size=0; size<=k; size++) {
            
            for (int j=0; j<size; j++) {
                currentSeed.push_back(leftSeedVector[j]);
            }
            for (int j=0; j<k-size; j++) {
                currentSeed.push_back(rightSeedVector[j]);
            }
            set<int> c = set<int>(currentSeed.begin(), currentSeed.end());
            int influence = this->timInfluenceCalculator.get()->findInfluenceWithoutUpdatingModel(c).first;
            if(influence>bestInfluence) {
                bestInfluence = influence;
                bestSeed = currentSeed;
                assert(bestSeed.size()==k);
            }
            currentSeed.clear();
        }

        return bestSeed;
    }
}
