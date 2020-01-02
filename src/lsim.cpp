/* ==========================================
 * Project05: Logic Simulation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2020/01/01
 * ==========================================
 */
#include "lsim.h"
#include "blif_file_handler.h"
#include "sti_file_handler.h"
#include "logic_simulator.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * Program entry point for lsim (Project 5)
 * 
 * argc: Argument count
 * argv: Argument string array
 */
int Lsim::main(int argc, char** argv)
{
    // Options
    bool useVerboseMode = false;
    
    if (argc < 4) {
        cerr << "Error: Please specific input and output files." << endl;
        printUsage();
    }
    
    // Parse argument
    for (int i = 1; i < argc - 3; i++) {
        string arg(argv[i]);
        if (arg == "-v") {
            useVerboseMode = true;
        } else {
            cerr << "Invalid argument:" << arg << endl;
            printUsage();
        }
    }

    ifstream blifSource(argv[argc - 3]);
    if (blifSource.fail()) {
        cerr << "File not found: " << argv[argc - 3] << endl;
        exit(-1);
    }
    ifstream stiSource(argv[argc - 2]);
    if (stiSource.fail()) {
        cerr << "File not found: " << argv[argc - 2] << endl;
        exit(-1);
    }
    ofstream outputResFile(argv[argc - 1]);

    // Read file
    BlifFileHandler blifHandler;
    unique_ptr<SimpleLogicCircit> circit = blifHandler.read(blifSource);
    blifSource.close();
    StiFileHandler stiHandler;
    unique_ptr<vector<string>> inputs = stiHandler.read(stiSource);
    stiSource.close();
    // Simulate
    LogicSimulator simulator(useVerboseMode);
    simulator.simulate(circit, inputs, outputResFile);
    outputResFile.close();

    return 0;
}

/**
 * Print program usage
 */
void Lsim::printUsage()
{
    cout << "Usage: lsim [-v] PROGRAM BLIF_FILE STIMULI_FILE RESPONSE_FILE" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "    -v    Showing all debug information." << endl;
    cout << endl;
    cout << "Author: B10630221 Chang-Ting Kao" << endl;
    cout << endl;
    exit(-1);
}