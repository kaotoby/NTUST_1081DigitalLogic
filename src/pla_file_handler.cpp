/* ==========================================
 * Project02: ROBDD Generation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/10/07
 * ==========================================
 */
#include "pla_file_handler.h"
#include <sstream>

using namespace std;

/**
 * Reads PLA format stream into LogicRuleSet
 * 
 * stream: PLA format stream
 */
unique_ptr<LogicRuleSet> PlaFileHandler::read(istream& stream)
{
    unique_ptr<LogicRuleSet> ruleSet(new LogicRuleSet());
    int pCount = 0;
    int pCurrent = 0;
    int inputCount = 0;
    int outputCount = 0;
    int lineNumber = 0;

    string line;
    string token;
    string inputStr, outputStr;
    while (getline(stream, line)) {
        lineNumber++;
        istringstream iss(line);
        if (!(iss >> token)) {
            continue; // Ignore empty line
        }

        if (token[0] != '.' && pCurrent < pCount) { // .p section
            inputStr = token;
            if (!(iss >> outputStr) || inputStr.length() != inputCount || outputStr.length() != outputCount) {
                throw PlaFileFormatException(lineNumber, ".p", pCurrent);
            }
            LogicRule rule = LogicRule();
            // Convert literal term to number term for input
            for (int i = 0; i < inputCount; i++) {
                rule.input.value <<= 1;
                rule.input.dontCare <<= 1;
                switch (inputStr[i]) {
                case '0':
                    break;
                case '1':
                    rule.input.value |= 1;
                    break;
                case '-':
                    rule.input.dontCare |= 1;
                    break;
                default:
                    throw PlaFileFormatException(lineNumber, ".p", pCurrent);
                }
            }
            // Convert literal term to number term for output
            for (int i = 0; i < outputCount; i++) {
                rule.output.value <<= 1;
                rule.output.dontCare <<= 1;
                switch (outputStr[i]) {
                case '0':
                    break;
                case '1':
                    rule.output.value |= 1;
                    break;
                case '-':
                    rule.output.dontCare |= 1;
                    break;
                default:
                    throw PlaFileFormatException(lineNumber, ".p", pCurrent);
                }
            }

            ruleSet->rules.push_back(rule);
            pCurrent++;
        } else if (token == ".i") {
            if (!(iss >> inputCount)) {
                throw PlaFileFormatException(lineNumber, ".i");
            }
            if (inputCount > MAX_INPUT_COUNT) {
                throw PlaFileFormatException(lineNumber, ".i", "must be less or equal to " + to_string(MAX_INPUT_COUNT));
            }
            ruleSet->inputLabels.reserve(inputCount);
        } else if (token == ".o") {
            if (!(iss >> outputCount)) {
                throw PlaFileFormatException(lineNumber, ".o");
            }
            if (outputCount > MAX_OUTPUT_COUNT) {
                throw PlaFileFormatException(lineNumber, ".i", "must be less or equal to " + to_string(MAX_OUTPUT_COUNT));
            }
            ruleSet->outputLabels.reserve(outputCount);
        } else if (token == ".ilb") {
            if (inputCount == 0) {
                throw PlaFileFormatException(lineNumber, ".ilb", "missing command '.i' before token");
            }
            for (int i = 0; i < inputCount; i++) {
                if (!(iss >> token)) {
                    throw PlaFileFormatException(lineNumber, ".ilb", i);
                }
                ruleSet->inputLabels.push_back(token);
            }
        } else if (token == ".ob") {
            if (outputCount == 0) {
                throw PlaFileFormatException(lineNumber, ".ob", "missing command '.o' before token");
            }
            for (int i = 0; i < outputCount; i++) {
                if (!(iss >> token)) {
                    throw PlaFileFormatException(lineNumber, ".ob", i);
                }
                ruleSet->outputLabels.push_back(token);
            }
        } else if (token == ".p") {
            if (!(iss >> pCount)) {
                throw PlaFileFormatException(lineNumber, ".p");
            }
            ruleSet->rules.reserve(pCount);
        } else if (token == ".e") {
            return move(ruleSet);
        } else {
            throw PlaFileFormatException(lineNumber, token);
        }
    }

    throw PlaFileFormatException(lineNumber, ".e", "missing ending command '.e' at the end.");
}

/**
 * Writes the LogicRuleSet into stream in PLA format
 * 
 * ruleSet: The LogicRuleSet to write
 * stream: The steam to write to
 */
void PlaFileHandler::write(unique_ptr<LogicRuleSet>& ruleSet, ostream& stream)
{
    int inputCount = ruleSet->inputLabels.size();
    int outputCount = ruleSet->outputLabels.size();
    stream << ".i " << inputCount << endl;
    stream << ".o " << outputCount << endl;
    stream << ".ilb";
    for (const string& label : ruleSet->inputLabels) {
        stream << " " << label;
    }
    stream << endl;
    stream << ".ob";
    for (const string& label : ruleSet->outputLabels) {
        stream << " " << label;
    }
    stream << endl;
    stream << ".p " << ruleSet->rules.size() << endl;
    for (const LogicRule& rule : ruleSet->rules) {
        stream << literalTerm(rule.input.value, rule.input.dontCare, inputCount) << " "
               << literalTerm(rule.output.value, rule.output.dontCare, outputCount) << endl;
    }
    stream << ".e" << endl;
}