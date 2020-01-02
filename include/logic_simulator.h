/* ==========================================
 * Project05: Logic Simulation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2020/01/01
 * ==========================================
 */
#pragma once

#include "digital_logic.h"
#include <iostream>
#include <memory>

class LogicSimulator {
private:
    // Flag indicate using verbose mode to show debug information
    bool useVerboseMode;

    /**
     * Topology sort the gates
     * 
     * gates: Gates to sort
     * offset: Offset of the gate index
     */
    std::vector<SimpleLogicGate> topologySort(std::vector<SimpleLogicGate>& gates, int offset);
    /**
     * Simulate logic circit
     * 
     * circit: Logic circit to simulate
     * inputs: Input for simulation
     * outputFile: Stream to store the outputs
     */
    std::string simulate(std::unique_ptr<SimpleLogicCircit>& circit, std::string& input);
    /**
     * Simulate logic circit
     * 
     * sortedGates: Logic gates after topology sort
     * input: Input for simulation
     * stateBuffer: Buffer to store the states
     * bufferSize: Output buffer size
     */
    inline void simulate(std::vector<SimpleLogicGate>& sortedGates, const std::string& input, unsigned char* stateBuffer, int bufferSize);

public:
    /**
     * Construct a LogicSimulator instance
     * 
     * verbose: Enable verbose mode or not
     */
    LogicSimulator(bool verbose);

    /**
     * Simulate logic circit
     * 
     * circit: Logic circit to simulate
     * inputs: Inputs for simulation
     * outputFile: Stream to store the outputs
     */
    void simulate(std::unique_ptr<SimpleLogicCircit>& circit, std::unique_ptr<std::vector<std::string>>& inputs, std::ostream& output);
};