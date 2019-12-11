/* ==========================================
 * Project04: State Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/12/10
 * ==========================================
 */
#include "state_transition_graph.h"
#include <algorithm>
#include <fstream>

using namespace std;

/**
 * Construct a StateTransitionGraph instance
 * 
 * stateTransitionSet: The state transition set to construct the graph
 * verbose: Enable verbose mode or not
 */
StateTransitionGraph::StateTransitionGraph(unique_ptr<StateTransitionSet>& stateTransitionSet, bool verbose)
    : useVerboseMode(verbose)
{
    this->inputLabels = stateTransitionSet->inputLabels;
    this->outputLabels = stateTransitionSet->outputLabels;
    this->stateLabels = stateTransitionSet->stateLabels;
    this->inputCount = stateTransitionSet->inputLabels.size();
    this->outputCount = stateTransitionSet->outputLabels.size();
    this->stateCount = stateTransitionSet->stateLabels.size();

    for (size_t i = 0; i < this->stateCount; i++) {
        this->nodes[i].resize(this->stateCount);
    }
    for (const StateTransition& StateTransition : stateTransitionSet->transitions) {
        this->nodes[StateTransition.from][StateTransition.input.value].state = StateTransition.to;
        this->nodes[StateTransition.from][StateTransition.input.value].value = StateTransition.output.value;
    }
}

/**
 * Minimize the state transition graph
 */
unique_ptr<StateTransitionSet> StateTransitionGraph::minimize()
{
    // Init empty table
    ImplicationTableEntry** implicationTable = new ImplicationTableEntry*[this->stateCount];
    for (size_t i = 0; i < this->stateCount; ++i) {
        implicationTable[i] = new ImplicationTableEntry[this->stateCount];
    }
    // Step 1: List transition (next state) pairs.
    for (size_t from = 0; from < this->stateCount; from++) {
        for (size_t to = 0; to <= from; to++) {
            if (from == to) {
                implicationTable[from][to].isValid = true;
                continue;
            }
            bool allSame = true;
            for (int i = 0; i < (1 << this->inputCount); i++) {
                if (this->nodes[from][i].value != this->nodes[to][i].value) {
                    allSame = false;
                    break;
                }
            }
            if (allSame) {
                implicationTable[from][to].isValid = true;
                for (int i = 0; i < (1 << this->inputCount); i++) {
                    implicationTable[from][to].pairs.push_back(make_pair(this->nodes[from][i].state, this->nodes[to][i].state));
                }
            } else {
                implicationTable[from][to].isValid = false;
            }
        }
    }
    this->verbosePrintImplicationTable(implicationTable);

    // Step2: Check compatibility of transition pair.
    bool hasChanged;
    do {
        hasChanged = false;
        for (size_t from = 0; from < this->stateCount; from++) {
            for (size_t to = 0; to < from; to++) {
                if (!implicationTable[from][to].isValid) {
                    continue;
                }
                for (size_t i = 0; i < (1 << this->inputCount); i++) {
                    unsigned int temp;
                    unsigned int s1 = implicationTable[from][to].pairs[i].first;
                    unsigned int s2 = implicationTable[from][to].pairs[i].second;
                    if (s1 < s2) {
                        temp = s1;
                        s1 = s2;
                        s2 = temp;
                    }
                    if (!implicationTable[s1][s2].isValid) {
                        // Remove incompatible transition pairs
                        implicationTable[from][to].isValid = false;
                        hasChanged = true;
                        break;
                    }
                }
            }
        }
        this->verbosePrintImplicationTable(implicationTable);
    } while (hasChanged);
    this->verbosePrint();

    // Step3: Merge remaining compatible states.
    for (unsigned int from = 0; from < this->stateCount; from++) {
        for (unsigned int to = 0; to < from; to++) {
            if (!implicationTable[from][to].isValid) {
                continue;
            }
            unsigned int f, t;
            if (from > to) {
                f = from;
                t = to;
            } else {
                f = to;
                t = from;
            }
            // Update implication table
            for (size_t i = 0; i < this->stateCount; i++) {
                implicationTable[f][i].isValid = false;
                implicationTable[i][f].isValid = false;
            }

            // Remove state
            this->nodes.erase(f);
            this->stateLabels[f] = "";
            // Replace state
            for (auto& kvp : this->nodes) {
                for (size_t i = 0; i < (1 << this->inputCount); i++) {
                    if (kvp.second[i].state == f) {
                        kvp.second[i].state = t;
                    }
                }
            }
            this->verbosePrint();
        }
    }

    // Release memory
    for (size_t i = 0; i < this->stateCount; ++i) {
        delete[] implicationTable[i];
    }
    delete[] implicationTable;

    // Build result
    map<unsigned int, unsigned int> indexMap;
    unsigned int index = 0;
    unsigned int realIndex = 0;
    vector<string>::iterator it = this->stateLabels.begin();
    while (it != this->stateLabels.end()) {
        if (it->length() == 0) {
            it = this->stateLabels.erase(it);
        } else {
            ++it;
            indexMap[index] = realIndex++;
        }
        index++;
    }
    for (const auto& map : indexMap) {
        if (map.first == map.second) {
            continue;
        }
        this->nodes[map.second] = this->nodes[map.first];
        this->nodes.erase(map.first);
        for (auto& kvp : this->nodes) {
            for (size_t i = 0; i < (1 << this->inputCount); i++) {
                if (kvp.second[i].state == map.first) {
                    kvp.second[i].state = map.second;
                }
            }
        }
    }

    unique_ptr<StateTransitionSet> ruleSet = unique_ptr<StateTransitionSet>(new StateTransitionSet());
    ruleSet->inputLabels = this->inputLabels;
    ruleSet->outputLabels = this->outputLabels;
    ruleSet->stateLabels = this->stateLabels;
    for (auto const& kvp : this->nodes) {
        for (ilogic_t i = 0; i < (1 << this->inputCount); i++) {
            StateTransition trans;
            trans.from = kvp.first;
            trans.input = { i, 0 };
            trans.output = { kvp.second[i].value, 0 };
            trans.to = kvp.second[i].state;
            ruleSet->transitions.push_back(trans);
        }
    }

    return move(ruleSet);
}

/**
 * Export graph to DOT file
 * 
 * fileName: Output file name without extension
 * doImageOutput: Output png image with dot on completion
 */
void StateTransitionGraph::exportDotFile(std::string fileName, bool doImageOutput)
{
    ofstream ofs(fileName);
    // Write header
    ofs << "digraph STG {" << endl;
    ofs << "    rankdir=LR;" << endl;
    ofs << endl;
    ofs << "    INIT [shape=point];" << endl;

    // Write states
    for (const string& state : this->stateLabels) {
        ofs << "    " << state << " [label=\"" << state << "\"];" << endl;
    }
    ofs << endl;

    // Write transitions
    ofs << "    INIT -> " << this->stateLabels.at(0) << ";" << endl;
    for (const auto& kvp : this->nodes) {
        vector<pair<unsigned int, vector<pair<unsigned int, ologic_t>>>> groups;

        for (size_t i = 0; i < (1 << this->inputCount); i++) {
            unsigned int state = kvp.second[i].state;
            auto it = find_if(groups.begin(), groups.end(),
                [&state](const pair<unsigned int, vector<pair<unsigned int, ologic_t>>>& element) {
                    return element.first == state;
                });
            if (it == groups.end()) {
                vector<pair<unsigned int, ologic_t>> vec;
                vec.push_back(make_pair(i, kvp.second[i].value));
                groups.push_back(make_pair(state, vec));
            } else {
                it->second.push_back(make_pair(i, kvp.second[i].value));
            }
        }
        for (const auto& group : groups) {
            ofs << "    " << this->stateLabels.at(kvp.first)
                << " -> " << this->stateLabels.at(group.first)
                << " [label=\"";
            for (int i = 0; i < group.second.size(); i++) {
                if (i > 0) {
                    ofs << ",";
                }
                ofs << literalTerm(group.second[i].first, 0, this->inputCount) << "/";
                ofs << literalTerm(group.second[i].second, 0, this->outputCount);
            }
            ofs << "\"];" << endl;
        }
    }

    // Write footer
    ofs << "}";
    ofs.close();

    // Output image
    if (doImageOutput) {
        string command = "dot -T png " + fileName + " > " + fileName + ".png";
        int rtn = system(command.c_str());
    }
}

/**
 * Prints current nodes in verbose mode
 * 
 * implicationTable: The implication table
 */
void StateTransitionGraph::verbosePrint()
{
    if (!this->useVerboseMode) {
        return;
    }

    for (auto const& kvp : this->nodes) {
        cout << this->stateLabels.at(kvp.first) << "\t |";
        for (int i = 0; i < (1 << this->inputCount); i++) {
            cout << this->stateLabels.at(kvp.second[i].state) << "\t" << (int)kvp.second[i].value << "\t |";
        }
        cout << endl;
    }
    cout << "===================================" << endl;
}

/**
 * Prints current implication table in verbose mode
 * 
 * implicationTable: The implication table
 */
void StateTransitionGraph::verbosePrintImplicationTable(ImplicationTableEntry** implicationTable)
{
    if (!this->useVerboseMode) {
        return;
    }

    for (size_t from = 0; from < this->stateCount; from++) {
        for (size_t i = 0; i < (1 << this->inputCount); i++) {
            for (size_t to = 0; to < from; to++) {
                if (implicationTable[from][to].isValid) {
                    cout << this->stateLabels.at(implicationTable[from][to].pairs[i].first)
                         << "-"
                         << this->stateLabels.at(implicationTable[from][to].pairs[i].second);
                } else {
                    cout << " - ";
                }
                cout << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    cout << "===================================" << endl;
}