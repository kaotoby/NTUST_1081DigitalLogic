/* ==========================================
 * Project04: State Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/12/10
 * ==========================================
 */
#include "kiss_file_handler.h"
#include <map>
#include <sstream>

using namespace std;

struct RawStateTransition {
    string input;
    string output;
    string from;
    string to;
};

/**
 * Reads KISS format stream into StateTransitionSet
 * 
 * stream: KISS format stream
 */
unique_ptr<StateTransitionSet> KissFileHandler::read(istream& stream)
{
    unique_ptr<StateTransitionSet> ruleSet(new StateTransitionSet());
    int pCount = 0;
    int pCurrent = 0;
    int inputCount = 0;
    int outputCount = 0;
    int lineNumber = 0;
    int stateCount = 0;
    bool startTag = false;

    string line;
    string token;
    string initialState;
    string inputStr, fromState, toState, outputStr;
    vector<struct RawStateTransition> transitions;
    map<string, unsigned int> states;
    while (getline(stream, line)) {
        lineNumber++;
        istringstream iss(line);
        if (!(iss >> token)) {
            continue; // Ignore empty line
        }

        if (token == ".start_kiss") {
            startTag = true;
            continue;
        }
        if (!startTag) {
            throw KissFileFormatException(lineNumber, ".start_kiss", "missing begining command '.start_kiss' at the start.");
        }

        if (token[0] != '.' && pCurrent < pCount) { // .p section
            inputStr = token;
            if (!(iss >> fromState >> toState >> outputStr) || inputStr.length() != inputCount || outputStr.length() != outputCount) {
                throw KissFileFormatException(lineNumber, ".p", pCurrent);
            }
            // 1st pass
            struct RawStateTransition trans;
            trans.input = inputStr;
            trans.from = fromState;
            trans.to = toState;
            trans.output = outputStr;
            transitions.push_back(trans);
            pCurrent++;
        } else if (token == ".i") {
            if (!(iss >> inputCount)) {
                throw KissFileFormatException(lineNumber, ".i");
            }
            if (inputCount > MAX_INPUT_COUNT) {
                throw KissFileFormatException(lineNumber, ".i", "must be less or equal to " + to_string(MAX_INPUT_COUNT));
            }
            ruleSet->inputLabels.resize(inputCount);
        } else if (token == ".o") {
            if (!(iss >> outputCount)) {
                throw KissFileFormatException(lineNumber, ".o");
            }
            if (outputCount > MAX_OUTPUT_COUNT) {
                throw KissFileFormatException(lineNumber, ".i", "must be less or equal to " + to_string(MAX_OUTPUT_COUNT));
            }
            ruleSet->outputLabels.resize(outputCount);
        } else if (token == ".p") {
            if (!(iss >> pCount)) {
                throw KissFileFormatException(lineNumber, ".p");
            }
            ruleSet->transitions.reserve(pCount);
            transitions.reserve(pCount);
        } else if (token == ".s") {
            if (!(iss >> stateCount)) {
                throw KissFileFormatException(lineNumber, ".s");
            }
            ruleSet->stateLabels.reserve(stateCount);
        } else if (token == ".r") {
            if (stateCount == 0) {
                throw KissFileFormatException(lineNumber, ".r", "missing command '.s' before token");
            }
            if (!(iss >> fromState)) {
                throw KissFileFormatException(lineNumber, ".r");
            }
            states[fromState] = 0;
            ruleSet->stateLabels.push_back(fromState);
        } else if (token == ".ilb") {
            if (inputCount == 0) {
                throw KissFileFormatException(lineNumber, ".ilb", "missing command '.i' before token");
            }
            for (int i = 0; i < inputCount; i++) {
                if (!(iss >> token)) {
                    throw KissFileFormatException(lineNumber, ".ilb", i);
                }
                ruleSet->inputLabels[i] = token;
            }
        } else if (token == ".ob") {
            if (outputCount == 0) {
                throw KissFileFormatException(lineNumber, ".ob", "missing command '.o' before token");
            }
            for (int i = 0; i < outputCount; i++) {
                if (!(iss >> token)) {
                    throw KissFileFormatException(lineNumber, ".ob", i);
                }
                ruleSet->outputLabels[i] = token;
            }
        } else if (token == ".end_kiss") {
            // 2nd pass
            for (const struct RawStateTransition& raw : transitions) {
                if (states.find(raw.from) == states.end()) {
                    unsigned int index = ruleSet->stateLabels.size();
                    states[raw.from] = index;
                    ruleSet->stateLabels.push_back(raw.from);
                }
            }
            for (const struct RawStateTransition& raw : transitions) {
                if (states.find(raw.to) == states.end()) {
                    unsigned int index = ruleSet->stateLabels.size();
                    states[raw.from] = index;
                    ruleSet->stateLabels.push_back(raw.from);
                }
            }
            for (const struct RawStateTransition& raw : transitions) {
                StateTransition trans = StateTransition();
                trans.from = states.find(raw.from)->second;
                trans.to = states.find(raw.to)->second;
                // Convert literal term to number term for input
                for (int i = 0; i < inputCount; i++) {
                    trans.input.value <<= 1;
                    trans.input.dontCare <<= 1;
                    switch (raw.input[i]) {
                    case '0':
                        break;
                    case '1':
                        trans.input.value |= 1;
                        break;
                    case '-':
                        trans.input.dontCare |= 1;
                        break;
                    default:
                        throw KissFileFormatException(lineNumber, ".p", pCurrent);
                    }
                }
                // Convert literal term to number term for output
                for (int i = 0; i < outputCount; i++) {
                    trans.output.value <<= 1;
                    trans.output.dontCare <<= 1;
                    switch (raw.output[i]) {
                    case '0':
                        break;
                    case '1':
                        trans.output.value |= 1;
                        break;
                    case '-':
                        trans.output.dontCare |= 1;
                        break;
                    default:
                        throw KissFileFormatException(lineNumber, ".p", pCurrent);
                    }
                }

                ruleSet->transitions.push_back(trans);
            }

            return move(ruleSet);
        } else {
            throw KissFileFormatException(lineNumber, token);
        }
    }

    throw KissFileFormatException(lineNumber, ".end_kiss", "missing ending command '.end_kiss' at the end.");
}

/**
 * Writes the StateTransitionSet into stream in KISS format
 * 
 * ruleSet: The StateTransitionSet to write
 * stream: The steam to write to
 */
void KissFileHandler::write(std::unique_ptr<StateTransitionSet>& stateTransitionSet, std::ostream& stream)
{
    int inputCount = stateTransitionSet->inputLabels.size();
    int outputCount = stateTransitionSet->outputLabels.size();
    int stateCount = stateTransitionSet->stateLabels.size();
    stream << ".start_kiss" << endl;
    stream << ".i " << inputCount << endl;
    stream << ".o " << outputCount << endl;
    stream << ".p " << stateTransitionSet->transitions.size() << endl;
    stream << ".s " << stateCount << endl;
    stream << ".r " << stateTransitionSet->stateLabels.at(0) << endl;
    for (const StateTransition& transition : stateTransitionSet->transitions) {
        stream << literalTerm(transition.input.value, transition.input.dontCare, inputCount) << " "
               << stateTransitionSet->stateLabels.at(transition.from) << " "
               << stateTransitionSet->stateLabels.at(transition.to) << " "
               << literalTerm(transition.output.value, transition.output.dontCare, outputCount) << endl;
    }
    stream << ".end_kiss" << endl;
}