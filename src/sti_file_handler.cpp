/* ==========================================
 * Project05: Logic Simulation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2020/01/01
 * ==========================================
 */
#include "sti_file_handler.h"
#include <map>
#include <sstream>

using namespace std;

/**
 * Reads .sti format stream into StateTransitionSet
 * 
 * stream: STI format stream
 */
unique_ptr<vector<string>> StiFileHandler::read(istream& stream)
{
    unique_ptr<vector<string>> inputs(new vector<string>());
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

        if (token[0] != '.') { // .i section
            inputs->push_back(token);
        } else if (token == ".i") {
            if (!(iss >> inputCount)) {
                throw StiFileFormatException(lineNumber, ".i");
            }
        } else if (token == ".e") {
            return move(inputs);
        } else {
            throw StiFileFormatException(lineNumber, token);
        }
    }

    throw StiFileFormatException(lineNumber, ".e", "missing ending command '.e' at the end.");
}