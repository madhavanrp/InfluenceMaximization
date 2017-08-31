CXX = /usr/local/bin/g++-4.9
core_SRC = InfluenceMaximization/*.cpp
influence_SRC = $(core_SRC) main.cpp
influence_tests_SRC = $(core_SRC) tests/*.cpp

influence: TARGET = influence
influence: OUTPUT = influence

influence_tests: TARGET = influence_tests
influence_tests: OUTPUT = influence_tests

influence: all
influence_tests: all

all:
	 $(CXX) $($(TARGET)_SRC) -Wall -std=c++11 -O3 -o $(OUTPUT)
