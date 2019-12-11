/* ==========================================
 * Project04: State Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/12/10
 * ==========================================
 */
#include "smin.h"
#include "kiss_file_handler.h"
#include "state_transition_graph.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * Program entry point for smin (Project 4)
 * 
 * argc: Argument count
 * argv: Argument string array
 */
int Smin::main(int argc, char** argv)
{
    // Options
    bool doImageOutput = false;
    bool useVerboseMode = false;
    bool outputStg = false;

    if (argc < 4) {
        cerr << "Error: Please specific input and output files." << endl;
        printUsage();
    }

    // Parse argument
    for (int i = 1; i < argc - 3; i++) {
        string arg(argv[i]);
        if (arg[0] == '-') {
            switch (arg[1]) {
            case 'o':
                outputStg = true;
                break;
            case 'm':
                doImageOutput = true;
                break;
            case 'v':
                useVerboseMode = true;
                break;
            default:
                cout << "Invalid argument:" << arg << endl;
                printUsage();
                break;
            }
        } else {
            cout << "Invalid argument:" << arg << endl;
            printUsage();
        }
    }

    ifstream inputSource(argv[argc - 3]);
    if (inputSource.bad()) {
        cerr << "File not found: " << argv[argc - 3] << endl;
        exit(-1);
    }
    string outputKissFileName = argv[argc - 2];
    string outputDotFileName = argv[argc - 1];

    KissFileHandler handler;
    unique_ptr<StateTransitionSet> ruleSet = handler.read(inputSource);
    inputSource.close();
    StateTransitionGraph stg(ruleSet, useVerboseMode);
    if (outputStg) {
        stg.exportDotFile(getFileNameWithoutExt(outputDotFileName) + "_stg.dot", doImageOutput);
    }
    unique_ptr<StateTransitionSet> minizedRuleSet = stg.minimize();
    // Save .dot
    stg.exportDotFile(outputDotFileName, doImageOutput);
    // Save .kiss
    ofstream ouputKiss(outputKissFileName);
    handler.write(minizedRuleSet, ouputKiss);
    ouputKiss.close();

    return 0;
}

/**
 * Print program usage
 */
void Smin::printUsage()
{
    cout << "Usage: smin [-v] [-o] [-m] KISS_IN KISS_OUT DOT_FILE" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "    -m    Output image as well. ('dot' binary must be installed first)" << endl;
    cout << "    -v    Showing all debug information." << endl;
    cout << "    -o    Output stg.dot as well." << endl;
    cout << endl;
    cout << "Author: B10630221 Chang-Ting Kao" << endl;
    cout << endl;
    exit(-1);
}