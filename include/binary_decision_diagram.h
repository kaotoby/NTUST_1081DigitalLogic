/* ==========================================
 * Project02: ROBDD Generation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/10/07
 * ==========================================
 */
#pragma once

#include "digital_logic.h"
#include <iostream>
#include <memory>

/**
 * A node inside binary decision diagram
 */
typedef struct  {
    // Variable label
    std::string variableName;
    // Edge to go on FALSE (0)
    int elseEdge;
    // Edge to go on TRUE (1)
    int thenEdge;
} BinaryDecisionDiagramNode;

/**
 * A binary decision diagram
 */
class BinaryDecisionDiagram
{
private:
    // Flag indicate using verbose mode to show debug information
    bool useVerboseMode;
    // Nodes in binary decision diagram
    std::vector<BinaryDecisionDiagramNode> nodes;
    // Label of inputs in logic
    std::vector<std::string> inputLabels;
    // Label of output in logic
    std::string outputLabel;

    /**
     * Replace state when simplifying the PLA logic
     * 
     * from: State number to replace from
     * to: State number to replace to
     */
    void replace(int from, int to);
    /**
     * Prints current nodes in verbose mode
     */
    void verbosePrint();
    
public:
    /**
     * Construct a BinaryDecisionDiagram instance
     * 
     * truthTable: The truth table to construct the diagram
     * outputIdx: The index of output variable inside truth table
     * verbose: Enable verbose mode or not
     */
    BinaryDecisionDiagram(std::unique_ptr<TruthTable>& truthTable, int outputIdx, bool verbose);

    /**
     * Reduce the binary decision diagram
     */
    void reduce();
    /**
     * Export diagram to DOT file
     * 
     * fileName: Output file name without extension
     * doImageOutput: Output png image with dot on completion
     */
    void exportDotFile(std::string fileName, bool doImageOutput);
};