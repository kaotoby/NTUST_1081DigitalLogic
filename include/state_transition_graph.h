/* ==========================================
 * Project04: State Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/12/10
 * ==========================================
 */
#pragma once

#include "digital_logic.h"
#include <iostream>
#include <map>
#include <memory>

/**
 * A entry inside implication table
 */
typedef struct {
    // Transition pairs
    std::vector<std::pair<unsigned int, unsigned int>> pairs;
    // Is valid
    bool isValid;
} ImplicationTableEntry;

/**
 * A node inside binary decision diagram
 */
typedef struct {
    // Output value
    ologic_t value;
    // State
    unsigned int state;
} OutputState;

/**
 * A state transition graph
 */
class StateTransitionGraph {
private:
    // Flag indicate using verbose mode to show debug information
    bool useVerboseMode;
    // Count of input
    int stateCount;
    // Count of input
    int inputCount;
    // Count of output
    int outputCount;
    // Label of input in logic
    std::vector<std::string> inputLabels;
    // Label of output in logic
    std::vector<std::string> outputLabels;
    // Label of states in logic
    std::vector<std::string> stateLabels;
    // Nodes in state transition graph
    std::map<unsigned int, std::vector<OutputState>> nodes;

    /**
     * Prints current nodes in verbose mode
     */
    void verbosePrint();
    /**
     * Prints current implication table in verbose mode
     * 
     * implicationTable: The implication table
     */
    void verbosePrintImplicationTable(ImplicationTableEntry** implicationTable);

public:
    /**
     * Construct a StateTransitionGraph instance
     * 
     * stateTransitionSet: The state transition set to construct the graph
     * verbose: Enable verbose mode or not
     */
    StateTransitionGraph(std::unique_ptr<StateTransitionSet>& stateTransitionSet, bool verbose);

    /**
     * Minimize the state transition graph
     */
    std::unique_ptr<StateTransitionSet> minimize();
    /**
     * Export graph to DOT file
     * 
     * fileName: Output file name without extension
     * doImageOutput: Output png image with dot on completion
     */
    void exportDotFile(std::string fileName, bool doImageOutput);
};