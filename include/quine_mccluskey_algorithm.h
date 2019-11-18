/* ==========================================
 * Project03: Exact Boolean Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/11/09
 * ==========================================
 */
#pragma once

#include "digital_logic.h"
#include "petricks_polynomial.h"
#include <iostream>
#include <memory>
#include <set>

/**
 * A table entry of Quine-McCluskey algorithm
 */
typedef struct {
    Input configuration;
    std::set<int> minterms;
    bool isPrimeImplicant;
    bool isDontCare;
} QuineMcCluskeyTableEntry;

/**
 * Quine-McCluskey algorithm to simplify boolean logic
 */
class QuineMcCluskeyAlgorithm {
private:
    // Flag indicate using verbose mode to show debug information
    bool useVerboseMode;
    // The count of input variables
    int inputCount;

    /**
     * Get prime implicants form minterms 
     * 
     * minterms: The set of minterms
     * dontCares: The set of don't cares
     */
    std::vector<std::unique_ptr<QuineMcCluskeyTableEntry>> getPrimeImplicants(std::set<ilogic_t>& minterms, std::set<ilogic_t>& dontCares);
    /**
     * Use petricks method to pick from Quine-McCluskey chart
     */
    std::vector<PetricksPolynomialProductTerm> petricksMethod(std::vector<std::unique_ptr<QuineMcCluskeyTableEntry>>& primeImplicants, std::set<ilogic_t>& minterms);

    /**
     * Generate a name from number for reading
     * https://www.geeksforgeeks.org/find-excel-column-name-given-number/
     *
     * n: Number for generation
     */
    std::string nameFromNumber(uint64_t n);
    /**
     * Prints Quine-McCluskey table in verbose mode
     * 
     * groups: Groups in Quine-McCluskey table to print
     */
    void verbosePrintQuineMcCluskeyTable(const std::vector<std::vector<std::unique_ptr<QuineMcCluskeyTableEntry>>>& groups);
    /**
     * Prints prime implicants in verbose mode
     * 
     * primeImplicants: Prime implicants to print
     */
    void verbosePrintPrimeImplicants(const std::vector<std::unique_ptr<QuineMcCluskeyTableEntry>>& primeImplicants);
    /**
     * Prints process of petrick's method in verbose mode
     * 
     * terms: Product terms of petrick's method
     * primeImplicants: Prime implicants of Quine-McCluskey chart
     */
    void verbosePrintPetricksPolynomial(const std::vector<PetricksPolynomialProductTerm> terms, const std::vector<std::unique_ptr<QuineMcCluskeyTableEntry>>& primeImplicants);
    /**
     * Prints solutions of petrick's method in verbose mode
     * 
     * terms: Product terms of petrick's method
     * primeImplicants: Prime implicants of Quine-McCluskey chart
     * inputLabels: Label of inputs in logic
     */
    void verbosePrintPetricksPolynomialSolution(const std::vector<PetricksPolynomialProductTerm> terms, const std::vector<std::unique_ptr<QuineMcCluskeyTableEntry>>& primeImplicants, const std::vector<std::string>& inputLabels);

public:
    /**
     * Construct a QuineMcCluskeyAlgorithm instance
     * 
     * inputCount: The count of input variables
     * verbose: Enable verbose mode or not
     */
    QuineMcCluskeyAlgorithm(int inputCount, bool verbose);

    /**
     * Simplify the boolean logic
     * 
     * truthTable: The truth table to simplify
     * outputIdx: The index of output variable inside truth table
     */
    std::vector<std::unique_ptr<LogicRuleSet>> simplify(std::unique_ptr<TruthTable>& truthTable, int outputIdx);
};