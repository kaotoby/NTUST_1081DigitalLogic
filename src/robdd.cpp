/* ==========================================
 * Project02: ROBDD Generation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/10/07
 * ==========================================
 */
#include "robdd.h"
#include "binary_decision_diagram.h"
#include "pla_file_handler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/**
 * Program entry point for robdd (Project 2)
 * 
 * argc: Argument count
 * argv: Argument string array
 */
int Robdd::main(int argc, char** argv)
{
    // Options
    bool doImageOutput = false;
    bool useVerboseMode = false;
    bool outputObdd = false;

    if (argc < 3) {
        cerr << "Error: Please specific input and output files." << endl;
        printUsage();
    }

    // Parse argument
    for (int i = 1; i < argc - 2; i++) {
        string arg(argv[i]);
        if (arg[0] == '-') {
            switch (arg[1]) {
            case 'o':
                outputObdd = true;
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

    ifstream inputSource(argv[argc - 2]);
    if (inputSource.bad()) {
        cerr << "File not found: " << argv[argc - 2] << endl;
        exit(-1);
    }
    string outputFileName = argv[argc - 1];

    PlaFileHandler handler;
    unique_ptr<LogicRuleSet> ruleSet = handler.read(inputSource);
    inputSource.close();
    unique_ptr<TruthTable> truthTable = ruleSet->truthTable();
    BinaryDecisionDiagram bdd(truthTable, 0, useVerboseMode);
    if (outputObdd) {
        bdd.exportDotFile(getFileNameWithoutExt(outputFileName) + "_obdd.dot", doImageOutput);
    }
    bdd.reduce();
    bdd.exportDotFile(outputFileName, doImageOutput);

    return 0;
}

/**
 * Print program usage
 */
void Robdd::printUsage()
{
    cout << "Usage: robdd [-v] [-o] PLA_FILE DOT_FILE" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "    -m    Output image as well. ('dot' binary must be installed first)" << endl;
    cout << "    -v    Showing all debug information." << endl;
    cout << "    -o    Output obdd.dot as well." << endl;
    cout << endl;
    cout << "Author: B10630221 Chang-Ting Kao" << endl;
    cout << endl;
    exit(-1);
}