/* ==========================================
 * Project05: Logic Simulation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2020/01/01
 * ==========================================
 */
#include "logic_simulator.h"
#include <cstring>
#include <stack>

using namespace std;

/**
 * Construct a LogicSimulator instance
 * 
 * verbose: Enable verbose mode or not
 */
LogicSimulator::LogicSimulator(bool verbose)
    : useVerboseMode(verbose)
{
}

/**
 * Topology sort the gates
 * 
 * gates: Gates to sort
 * offset: Offset of the gate index
 */
vector<SimpleLogicGate> LogicSimulator::topologySort(std::vector<SimpleLogicGate>& gates, int offset)
{
    int count = gates.size() + offset;
    vector<SimpleLogicGate> sortedGates;
    sortedGates.reserve(count);

    vector<bool> visited(count);
    vector<vector<int>> neighbors(count);
    stack<pair<bool, int>> dfsStack;

    // Build neighbor table
    for (int i = 0; i < gates.size(); i++) {
        SimpleLogicGate gate = gates[i];
        switch (gate.type) {
        case SimpleLogicGate::GateType::zero:
        case SimpleLogicGate::GateType::one:
        case SimpleLogicGate::GateType::BUF:
        case SimpleLogicGate::GateType::NOT:
            neighbors[i + offset].push_back(gate.a);
            break;
        case SimpleLogicGate::GateType::NAND2:
        case SimpleLogicGate::GateType::NOR2:
        case SimpleLogicGate::GateType::AND2:
        case SimpleLogicGate::GateType::OR2:
        case SimpleLogicGate::GateType::XOR2:
        case SimpleLogicGate::GateType::XNOR2:
            neighbors[i + offset].push_back(gate.a);
            neighbors[i + offset].push_back(gate.b);
            break;
        case SimpleLogicGate::GateType::NAND3:
        case SimpleLogicGate::GateType::NOR3:
        case SimpleLogicGate::GateType::AND3:
        case SimpleLogicGate::GateType::OR3:
            neighbors[i + offset].push_back(gate.a);
            neighbors[i + offset].push_back(gate.b);
            neighbors[i + offset].push_back(gate.c);
            break;
        }
    }

    // DFS every node
    for (int i = 0; i < gates.size(); i++) {
        if (visited[i + offset] == false) {
            dfsStack.push(make_pair(false, i + offset));
        }
        while (!dfsStack.empty()) {
            pair<bool, int> node = dfsStack.top();
            dfsStack.pop();
            if (node.first) {
                sortedGates.push_back(gates[node.second - offset]);
                continue;
            }
            visited[node.second] = true;
            dfsStack.push(make_pair(true, node.second));

            for (const int& next : neighbors[node.second]) {
                if (next >= offset && !visited[next]) {
                    dfsStack.push(make_pair(false, next));
                }
            }
        }
    }

    if (this->useVerboseMode) {
        cout << "Topology Sorted:" << endl;
        for (size_t i = 0; i < sortedGates.size(); i++) {
            cout << i + 1 << ": " << sortedGates[i].name << endl;
        }
        cout << "===================================" << endl;
    }

    return sortedGates;
}

/**
 * Simulate logic circit
 * 
 * circit: Logic circit to simulate
 * inputs: Inputs for simulation
 * output: Stream to store the outputs
 */
void LogicSimulator::simulate(unique_ptr<SimpleLogicCircit>& circit, unique_ptr<vector<std::string>>& inputs, ostream& output)
{
    int inputCount = circit->inputLabels.size();
    int outputCount = circit->outputLabels.size();
    int gateCount = circit->gates.size();
    int bufferSize = inputCount + gateCount;
    unsigned char* stateBuffer = new unsigned char[bufferSize];

    // Sort gates
    vector<SimpleLogicGate> sortedGates = this->topologySort(circit->gates, inputCount);

    // Write Header
    output << ".i " << inputCount << endl;
    output << ".o " << outputCount << endl;

    for (const string& input : *inputs) {
        this->simulate(sortedGates, input, stateBuffer, bufferSize);

        // Output simulation result
        output << input << " ";
        for (size_t i = 0; i < outputCount; i++) {
            switch (stateBuffer[inputCount + i]) {
            case 0:
                output << '0';
                break;
            case 1:
                output << '1';
                break;
            default:
                output << 'X';
                break;
            }
        }
        output << endl;
    }

    // Write Footer
    output << ".e" << endl;
}

/**
 * Simulate logic circit
 * 
 * sortedGates: Logic gates after topology sort
 * input: Input for simulation
 * stateBuffer: Buffer to store the states
 * bufferSize: Output buffer size
 */
inline void LogicSimulator::simulate(vector<SimpleLogicGate>& sortedGates, const string& input, unsigned char* stateBuffer, int bufferSize)
{
    memset(stateBuffer, 0, bufferSize);
    int offset = input.length();

    for (size_t i = 0; i < input.length(); i++) {
        stateBuffer[i] = input[i] - '0';
    }

    for (const SimpleLogicGate& gate : sortedGates) {
        switch (gate.type) {
        case SimpleLogicGate::GateType::zero:
            stateBuffer[gate.O] = 0;
            break;
        case SimpleLogicGate::GateType::one:
            stateBuffer[gate.O] = 1;
            break;
        case SimpleLogicGate::GateType::BUF:
            stateBuffer[gate.O] = stateBuffer[gate.a] & 0x01;
            break;
        case SimpleLogicGate::GateType::NOT:
            stateBuffer[gate.O] = (~stateBuffer[gate.a]) & 0x01;
            break;
        case SimpleLogicGate::GateType::NAND2:
            stateBuffer[gate.O] = (~(stateBuffer[gate.a] & stateBuffer[gate.b])) & 0x01;
            break;
        case SimpleLogicGate::GateType::NOR2:
            stateBuffer[gate.O] = (~(stateBuffer[gate.a] | stateBuffer[gate.b])) & 0x01;
            break;
        case SimpleLogicGate::GateType::NAND3:
            stateBuffer[gate.O] = (~(stateBuffer[gate.a] & stateBuffer[gate.b] & stateBuffer[gate.c])) & 0x01;
            break;
        case SimpleLogicGate::GateType::NOR3:
            stateBuffer[gate.O] = (~(stateBuffer[gate.a] | stateBuffer[gate.b] | stateBuffer[gate.c])) & 0x01;
            break;
        case SimpleLogicGate::GateType::AND2:
            stateBuffer[gate.O] = (stateBuffer[gate.a] & stateBuffer[gate.b]) & 0x01;
            break;
        case SimpleLogicGate::GateType::OR2:
            stateBuffer[gate.O] = (stateBuffer[gate.a] | stateBuffer[gate.b]) & 0x01;
            break;
        case SimpleLogicGate::GateType::AND3:
            stateBuffer[gate.O] = (stateBuffer[gate.a] & stateBuffer[gate.b] & stateBuffer[gate.c]) & 0x01;
            break;
        case SimpleLogicGate::GateType::OR3:
            stateBuffer[gate.O] = (stateBuffer[gate.a] | stateBuffer[gate.b] | stateBuffer[gate.c]) & 0x01;
            break;
        case SimpleLogicGate::GateType::XOR2:
            stateBuffer[gate.O] = (stateBuffer[gate.a] ^ stateBuffer[gate.b]) & 0x01;
            break;
        case SimpleLogicGate::GateType::XNOR2:
            stateBuffer[gate.O] = (~(stateBuffer[gate.a] ^ stateBuffer[gate.b])) & 0x01;
            break;
        }
    }

    if (this->useVerboseMode) {
        cout << "Execution for [" << input << "]" << endl;
        for (size_t i = offset; i < bufferSize; i++) {
            cout << sortedGates[i - offset].name << ": " << (char)(stateBuffer[i] + '0') << endl;
        }
        cout << "===================================" << endl;
    }
}