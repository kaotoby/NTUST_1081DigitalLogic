/* ==========================================
 * Project05: Logic Simulation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2020/01/01
 * ==========================================
 */
#include "blif_file_handler.h"
#include <algorithm>
#include <map>
#include <sstream>

using namespace std;

/**
 * Construct a new instance of BlifFileHandler
 */
BlifFileHandler::BlifFileHandler()
{
    this->gateTypeMap["zero"] = SimpleLogicGate::GateType::zero;
    this->gateTypeMap["one"] = SimpleLogicGate::GateType::one;
    this->gateTypeMap["NOT"] = SimpleLogicGate::GateType::NOT;
    this->gateTypeMap["NAND2"] = SimpleLogicGate::GateType::NAND2;
    this->gateTypeMap["NOR2"] = SimpleLogicGate::GateType::NOR2;
    this->gateTypeMap["BUF"] = SimpleLogicGate::GateType::BUF;
    this->gateTypeMap["NAND3"] = SimpleLogicGate::GateType::NAND3;
    this->gateTypeMap["NOR3"] = SimpleLogicGate::GateType::NOR3;
    this->gateTypeMap["AND2"] = SimpleLogicGate::GateType::AND2;
    this->gateTypeMap["OR2"] = SimpleLogicGate::GateType::OR2;
    this->gateTypeMap["AND3"] = SimpleLogicGate::GateType::AND3;
    this->gateTypeMap["OR3"] = SimpleLogicGate::GateType::OR3;
    this->gateTypeMap["XOR2"] = SimpleLogicGate::GateType::XOR2;
    this->gateTypeMap["XNOR2"] = SimpleLogicGate::GateType::XNOR2;
}

/**
 * Reads .blif format stream into StateTransitionSet
 * 
 * stream: BLIF format stream
 */
unique_ptr<SimpleLogicCircit> BlifFileHandler::read(istream& stream)
{
    unique_ptr<SimpleLogicCircit> circit(new SimpleLogicCircit());
    int iCount = 0;
    int iCurrent = 0;
    int inputCount = 0;
    int lineNumber = 0;
    string line;
    string token;

    while (getline(stream, line)) {
        lineNumber++;
        istringstream iss(line);
        if (!(iss >> token)) {
            continue; // Ignore empty line
        }
        if (token[0] == '#') {
            // comment
        } else if (token == ".model") {
            string modelName;
            if (!(iss >> modelName)) {
                throw BlifFileFormatException(lineNumber, ".model");
            }
            circit->name = modelName;
        } else if (token == ".inputs") {
            string input;
            while (iss >> input) {
                if (input == "\\") {
                    getline(stream, line);
                    lineNumber++;
                    iss.clear();
                    iss.str(line);
                    continue;
                }
                circit->inputLabels.push_back(input);
                int id = this->variableIdMap.size();
                this->variableIdMap.insert(make_pair(input, id));
            }
        } else if (token == ".outputs") {
            string output;
            while (iss >> output) {
                if (output == "\\") {
                    getline(stream, line);
                    lineNumber++;
                    iss.clear();
                    iss.str(line);
                    continue;
                }
                circit->outputLabels.push_back(output);
                int id = this->variableIdMap.size();
                this->variableIdMap.insert(make_pair(output, id));
            }
        } else if (token == ".gate") {
            string type, mapping;
            if (!(iss >> type)) {
                throw BlifFileFormatException(lineNumber, ".gate");
            }
            map<string, SimpleLogicGate::GateType>::iterator typeIt = this->gateTypeMap.find(type);
            if (typeIt == this->gateTypeMap.end()) {
                throw BlifFileFormatException(lineNumber, type);
            }
            SimpleLogicGate gate;
            gate.type = typeIt->second;
            while (iss >> mapping) {
                size_t index = mapping.find('=');
                string wire = mapping.substr(0, index);
                string target = mapping.substr(index + 1, mapping.length() - index - 1);
                int id;
                map<string, int>::iterator mapIt = this->variableIdMap.find(target);
                if (mapIt == this->variableIdMap.end()) {
                    id = this->variableIdMap.size();
                    this->variableIdMap.insert(make_pair(target, id));
                } else {
                    id = mapIt->second;
                }

                if (wire == "a") {
                    gate.a = id;
                } else if (wire == "b") {
                    gate.b = id;
                } else if (wire == "c") {
                    gate.c = id;
                } else if (wire == "O") {
                    gate.O = id;
                    gate.name = target;
                } else {
                    throw BlifFileFormatException(lineNumber, wire);
                }
            }
            circit->gates.push_back(gate);
        } else if (token == ".end") {
            sort(circit->gates.begin(), circit->gates.end(), [](const SimpleLogicGate& lhs, const SimpleLogicGate& rhs) {
                return lhs.O < rhs.O;
            });
            return move(circit);
        } else {
            throw BlifFileFormatException(lineNumber, token);
        }
    }

    throw BlifFileFormatException(lineNumber, ".end", "missing ending command '.end' at the end.");
}