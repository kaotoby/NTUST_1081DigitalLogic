/* ==========================================
 * Project03: Exact Boolean Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/11/09
 * ==========================================
 */
#include "mini.h"
#include "pla_file_handler.h"
#include "quine_mccluskey_algorithm.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

/**
 * Program entry point for mini (Project 3)
 * 
 * argc: Argument count
 * argv: Argument string array
 */
int Mini::main(int argc, char** argv)
{
    // Options
    bool useVerboseMode = false;
    bool outputAllSolutions = false;

    if (argc < 3) {
        cerr << "Error: Please specific input and output files." << endl;
        printUsage();
    }

    // Parse argument
    for (int i = 1; i < argc - 2; i++) {
        string arg(argv[i]);
        if (arg == "-v") {
            useVerboseMode = true;
        } else if (arg == "-a") {
            outputAllSolutions = true;
        } else {
            cerr << "Invalid argument:" << arg << endl;
            printUsage();
        }
    }

    ifstream inputSource(argv[argc - 2]);
    if (inputSource.bad()) {
        cerr << "File not found: " << argv[argc - 2] << endl;
        exit(-2);
    }
    string outputFileName = argv[argc - 1];

    // Main logic
    PlaFileHandler handler;
    unique_ptr<LogicRuleSet> ruleSet = handler.read(inputSource);
    inputSource.close();
    unique_ptr<TruthTable> truthTable = ruleSet->truthTable();

    QuineMcCluskeyAlgorithm quineMcCluskey(ruleSet->inputLabels.size(), useVerboseMode);
    vector<unique_ptr<LogicRuleSet>> solutions = quineMcCluskey.simplify(truthTable, 0);
    if (outputAllSolutions) {
        for (int i = 0; i < solutions.size(); i++) {
            string fileName = getFileNameWithoutExt(outputFileName);
            ofstream outputFile(fileName + "." + to_string(i + 1) + ".pla");
            handler.write(solutions.at(i), outputFile);
            outputFile.close();
        }
    } else {
        // Output first solution
        ofstream outputFile(outputFileName);
        handler.write(solutions.at(0), outputFile);
        outputFile.close();
    }

    return 0;
}

/**
 * Print program usage
 */
void Mini::printUsage()
{
    cout << "Usage: mini [-v] [-a] INPUT_FILE OUTPUT_FILE" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "    -v    Showing debug information." << endl;
    cout << "    -a    Output all solutions." << endl;
    cout << endl;
    cout << "Author: B10630221 Chang-Ting Kao" << endl;
    cout << endl;
    exit(-1);
}
