/* ==========================================
 * Project02: ROBDD Generation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/10/07
 * ==========================================
 */
#include "binary_decision_diagram.h"
#include <fstream>
#include <iomanip>

using namespace std;

/**
 * Construct BinaryDecisionDiagram instance
 * 
 * truthTable: The truth table to construct the diagram
 * outputIdx: The index of output variable inside truth table
 * verbose: Enable verbose mode or not
 */
BinaryDecisionDiagram::BinaryDecisionDiagram(unique_ptr<TruthTable>& truthTable, int outputIdx, bool verbose)
    : useVerboseMode(verbose)
{
    this->inputLabels = truthTable->inputLabels;
    outputLabel = truthTable->outputLabels[outputIdx];
    ologic_t outputMask = (ologic_t)1 << (truthTable->outputLabels.size() - 1 - outputIdx);

    int inputCount = truthTable->inputLabels.size();

    // n inputs will have 2^n + 1 nodes
    size_t endIndex = 1 << inputCount;
    this->nodes.reserve((1 << inputCount) | 1);
    // node over 2^(n-1) is leaf node
    size_t leafNodeOffset = (1 << inputCount - 1);

    // Boolean 0
    BinaryDecisionDiagramNode node0 = { "0", -1, -1 };
    this->nodes.push_back(node0);
    // Nodes
    for (size_t i = 1; i < endIndex; i++) {
        BinaryDecisionDiagramNode node;
        if (i < leafNodeOffset) {
            node.elseEdge = i << 1;
            node.thenEdge = (i << 1) | 1;
        } else {
            // leaf node
            size_t minterm = (i - leafNodeOffset) << 1;
            node.elseEdge = (truthTable->configurations[minterm].value & outputMask) == 0 ? 0 : endIndex;
            node.thenEdge = (truthTable->configurations[minterm + 1].value & outputMask) == 0 ? 0 : endIndex;
        }
        int labelIdx = log2_64(i);
        node.variableName = this->inputLabels[labelIdx];
        this->nodes.push_back(node);
    }
    // Boolean 1
    BinaryDecisionDiagramNode node1 = { "1", -1, -1 };
    this->nodes.push_back(node1);

    this->verbosePrint();
}

/**
 * Reduce the binary decision diagram
 */
void BinaryDecisionDiagram::reduce()
{
    size_t nodeCount = this->nodes.size();
    bool hasModify = false;
    bool stateHasModify = false;

    do
    {
        hasModify = false;
        // Stage 1: Find state which elseEdge == thenEdge
        do
        {
            stateHasModify = false;
            for (size_t i = 1; i < nodeCount - 1; i++)
            {
                if (nodes[i].elseEdge == -1)
                    continue; // Redundant

                if (nodes[i].elseEdge == nodes[i].thenEdge)
                {
                    stateHasModify = true;
                    hasModify = true;
                    this->replace(i, nodes[i].elseEdge);
                    // Mark as delete
                    nodes[i].elseEdge = -1;
                    nodes[i].thenEdge = -1;
                    this->verbosePrint();
                }
            }
        } while (stateHasModify);
        // Stage 2: Find duplicate state
        do
        {
            stateHasModify = false;
            for (size_t i = 1; i < nodeCount - 1; i++)
            {
                if (nodes[i].elseEdge == -1)
                    continue; // Redundant

                for (size_t j = i + 1; j < nodeCount - 1; j++)
                {
                    if (nodes[j].elseEdge == -1)
                        continue; // Redundant
                    if (nodes[i].elseEdge == nodes[j].elseEdge && nodes[i].thenEdge == nodes[j].thenEdge)
                    {
                        stateHasModify = true;
                        hasModify = true;
                        this->replace(j, i);
                        // Mark as delete
                        nodes[j].elseEdge = -1;
                        nodes[j].thenEdge = -1;
                        this->verbosePrint();
                    }
                }
            }
        } while (stateHasModify);
    } while (hasModify);
}

/**
 * Export diagram to DOT file
 * 
 * fileName: Output file name without extension
 * doImageOutput: Output png image with dot on completion
 */
void BinaryDecisionDiagram::exportDotFile(string fileName, bool doImageOutput)
{
    size_t nodeCount = this->nodes.size();
    ofstream ofs(fileName);

    // Write header
    ofs << "digraph " << this->outputLabel << " {" << endl;

    // Grouping ranks
    vector<vector<int>> ranks(this->inputLabels.size());
    for (size_t i = 1; i < nodeCount - 1; i++) {
        if (nodes[i].elseEdge == -1)
            continue;
        int rank = log2_64(i);
        ranks[rank].push_back(i);
    }
    // Write ranks
    for (const vector<int>& rank : ranks) {
        if (rank.size() == 0)
            continue;
        ofs << "    {rank=same";
        for (const int& idx : rank) {
            ofs << " " << idx;
        }
        ofs << "}" << endl;
    }
    ofs << endl;

    // Write label
    ofs << "    0 [label=\"0\", shape=box]" << endl;
    for (int i = 1; i < nodeCount - 1; i++) {
        if (nodes[i].elseEdge == -1)
            continue;
        ofs << "    " << i << " [label=\"" << nodes[i].variableName << "\"]" << endl;
    }
    ofs << "    " << nodeCount - 1 << " [label=\"1\", shape=box]" << endl;
    ofs << endl;

    // Write lines
    for (int i = 1; i < this->nodes.size() - 1; i++) {
        if (nodes[i].elseEdge == -1)
            continue;
        ofs << "    " << i << " -> " << nodes[i].elseEdge << " [label=\"0\", style=dotted]" << endl;
        ofs << "    " << i << " -> " << nodes[i].thenEdge << " [label=\"1\", style=solid]" << endl;
    }

    // Write footer
    ofs << "}";
    ofs.close();

    // Output image
    if (doImageOutput) {
        string command = "dot -T png " + fileName + " > " + fileName + ".png";
        system(command.c_str());
    }
}

/**
 * Replace state when simplifying the PLA logic
 * 
 * from: State number to replace from
 * to: State number to replace to
 */
void BinaryDecisionDiagram::replace(int from, int to)
{
    for (int i = 1; i < this->nodes.size() - 1; i++) {
        if (this->nodes[i].elseEdge == from) {
            this->nodes[i].elseEdge = to;
        }
        if (this->nodes[i].thenEdge == from) {
            this->nodes[i].thenEdge = to;
        }
    }
}

/**
 * Print current nodes in verbose mode
 */
void BinaryDecisionDiagram::verbosePrint()
{
    if (!this->useVerboseMode) {
        return;
    }

    for (int i = 0; i < this->nodes.size(); i++) {
        cout << setw(2) << i << " "
             << setw(2) << this->nodes[i].variableName << " "
             << setw(2) << this->nodes[i].elseEdge << " "
             << setw(2) << this->nodes[i].thenEdge << endl;
    }
    cout << "===================================" << endl;
}