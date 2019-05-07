cmake_minimum_required(VERSION 3.14)
project(InfluenceMaximization)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O3")

include_directories(.)
include_directories(InfluenceMaximization)
include_directories(InfluenceMaximization/CIM)
include_directories(InfluenceMaximization/DifferenceAlgorithms)
include_directories(InfluenceMaximization/DPAlgorithm)
include_directories(InfluenceMaximization/Graph)
include_directories(InfluenceMaximization/Heuristics)
include_directories(InfluenceMaximization/IMResults)
include_directories(InfluenceMaximization/ModularKnapsack)
include_directories(InfluenceMaximization/TIM-Models)
include_directories(InfluenceMaximization/Models)
include_directories(SFMT)

file(GLOB Graph_SOURCES
        "InfluenceMaximization/Graph/*.hpp"
        "InfluenceMaximization/Graph/*.cpp")

file(GLOB CIM_SOURCES
        "InfluenceMaximization/CIM/*.cpp"
        "InfluenceMaximization/CIM/*.hpp")
file(GLOB Difference_SOURCES
        "InfluenceMaximization/DifferenceAlgorithms/*.cpp"
        "InfluenceMaximization/DifferenceAlgorithms/*.hpp")
file(GLOB SFMT_SOURCES
        "SFMT/SFMT.h"
        "SFMT/SFMT.c")


set(Common_SOURCES
        ${SFMT_SOURCES}
        InfluenceMaximization/IMResults/IMResults.h
        InfluenceMaximization/log.h
        InfluenceMaximization/memoryusage.h
        InfluenceMaximization/json.hpp
        InfluenceMaximization/Diffusion.hpp
        InfluenceMaximization/TIMInfluenceCalculator.hpp
        InfluenceMaximization/TIMInfluenceCalculator.cpp
        InfluenceMaximization/TIM-Models/TIMCoverage.hpp
        InfluenceMaximization/TIM-Models/TIMCoverage.cpp
        InfluenceMaximization/Models/IMSeedSet.hpp
        cxxopts.hpp
        main_utils.h)

add_executable(CIM
        ${Common_SOURCES}
        ${CIM_SOURCES}
        ${Graph_SOURCES}
        cim_main.cpp)

add_executable(difference
        ${Common_SOURCES}
        ${Graph_SOURCES}
        ${Difference_SOURCES}
        InfluenceMaximization/CIM/EstimateNonTargets.hpp
        InfluenceMaximization/CIM/EstimateNonTargets.cpp
        difference_main.cpp)

set(Heuristic_SOURCES
        InfluenceMaximization/Heuristics/HeuristicsExecuter.hpp
        InfluenceMaximization/Heuristics/HeuristicsExecuter.cpp)

add_executable(heuristic
        ${Common_SOURCES}
        ${Graph_SOURCES}
        ${Heuristic_SOURCES}
        heuristic_main.cpp)

set(DP_SOURCES
        InfluenceMaximization/DPAlgorithm/HeirarchicalDecomposition.hpp
        InfluenceMaximization/DPAlgorithm/HeirarchicalDecomposition.cpp)

add_executable(dpalgorithm
        ${Common_SOURCES}
        ${Graph_SOURCES}
        ${DP_SOURCES}
        dpalgorithm_main.cpp)
