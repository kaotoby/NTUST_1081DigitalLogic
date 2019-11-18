/* ==========================================
 * Project03: Exact Boolean Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/11/09
 * ==========================================
 */
#include "quine_mccluskey_algorithm.h"
#include "petricks_polynomial.h"
#include <bitset>
#include <iterator>
#include <map>

using namespace std;

/**
 * Construct a QuineMcCluskeyAlgorithm instance
 * 
 * inputCount: The count of input variables
 * verbose: Enable verbose mode or not
 */
QuineMcCluskeyAlgorithm::QuineMcCluskeyAlgorithm(int inputCount, bool verbose)
    : inputCount(inputCount)
    , useVerboseMode(verbose)
{
}

/**
 * Simplify the boolean logic
 * 
 * truthTable: The truth table to simplify
 * outputIdx: The index of output variable inside truth table
 */
vector<unique_ptr<LogicRuleSet>> QuineMcCluskeyAlgorithm::simplify(unique_ptr<TruthTable>& truthTable, int outputIdx)
{
    ologic_t outputMask = (ologic_t)1 << (truthTable->outputLabels.size() - 1 - outputIdx);

    // Build minterms and don't cares from truth table
    set<ilogic_t> minterms;
    set<ilogic_t> dontCares;
    for (size_t i = 0; i < truthTable->configurations.size(); i++) {
        ologic_t dontCare = (truthTable->configurations[i].dontCare & outputMask);
        ologic_t output = (truthTable->configurations[i].value & outputMask);
        if (output > 0) {
            minterms.insert(i);
        } else if (dontCare > 0) {
            dontCares.insert(i);
        }
    }

    // Step 1: finding prime implicants
    vector<unique_ptr<QuineMcCluskeyTableEntry>> primeImplicants = this->getPrimeImplicants(minterms, dontCares);
    this->verbosePrintPrimeImplicants(primeImplicants);

    // Step 2: prime implicant chart (Petrick's method)
    vector<PetricksPolynomialProductTerm> solutions = this->petricksMethod(primeImplicants, minterms);
    this->verbosePrintPetricksPolynomialSolution(solutions, primeImplicants, truthTable->inputLabels);

    // Convert data structure
    vector<unique_ptr<LogicRuleSet>> ruleSets;
    ruleSets.reserve(solutions.size());
    for (const PetricksPolynomialProductTerm& solution : solutions) {
        unique_ptr<LogicRuleSet> ruleSet(new LogicRuleSet());
        ruleSet->inputLabels = truthTable->inputLabels;
        ruleSet->outputLabels = truthTable->outputLabels;
        ruleSet->rules.reserve(solution.variables.size());
        for (const int& var : solution.variables) {
            LogicRule rule;
            rule.input = primeImplicants[var]->configuration;
            rule.output = { 1, 0 };
            ruleSet->rules.push_back(rule);
        }
        ruleSets.push_back(move(ruleSet));
    }

    return move(ruleSets);
}

/**
 * Get prime implicants form minterms 
 * 
 * minterms: The set of minterms
 * dontCares: The set of don't cares
 */
vector<unique_ptr<QuineMcCluskeyTableEntry>> QuineMcCluskeyAlgorithm::getPrimeImplicants(set<ilogic_t>& minterms, set<ilogic_t>& dontCares)
{
    vector<unique_ptr<QuineMcCluskeyTableEntry>> primeImplicants;

    set<Input> configurations;
    vector<vector<unique_ptr<QuineMcCluskeyTableEntry>>> groups(this->inputCount + 1);

    // Build first literal term of Quine–McCluskey table
    for (const ilogic_t& i : minterms) {
        bitset<64> bs(i);
        int group = bs.count();
        unique_ptr<QuineMcCluskeyTableEntry> entry(new QuineMcCluskeyTableEntry());
        Input configuration = { i, 0 };
        entry->isDontCare = false;
        entry->isPrimeImplicant = true;
        entry->configuration = configuration;
        entry->minterms.emplace(i);
        groups[group].push_back(move(entry));

        configurations.insert(configuration);
    }
    for (const ilogic_t& i : dontCares) {
        bitset<64> bs(i);
        int group = bs.count();
        unique_ptr<QuineMcCluskeyTableEntry> entry(new QuineMcCluskeyTableEntry());
        Input configuration = { i, 0 };
        entry->isDontCare = true;
        entry->isPrimeImplicant = true;
        entry->configuration = configuration;
        entry->minterms.emplace(i);
        groups[group].push_back(move(entry));

        configurations.insert(configuration);
    }

    this->verbosePrintQuineMcCluskeyTable(groups);

    // Build other literal term of Quine–McCluskey table
    for (int literal = this->inputCount - 1; literal >= 0; literal--) {
        vector<vector<unique_ptr<QuineMcCluskeyTableEntry>>> newGroups(literal + 1);
        for (int group = 0; group <= literal; group++) {
            // First term has 2^0 minterm, second term has 2^1 minterm...
            int mintermCount = (1 << (this->inputCount - literal));

            // Try to reduce
            for (const unique_ptr<QuineMcCluskeyTableEntry>& entryL : groups[group]) {
                for (const unique_ptr<QuineMcCluskeyTableEntry>& entryH : groups[group + 1]) {
                    if (entryL->configuration.dontCare != entryH->configuration.dontCare) {
                        continue; // Don't cares are different, can't reduce
                    }
                    ilogic_t diff = (entryL->configuration.value ^ entryH->configuration.value);
                    bitset<64> bs(diff);
                    if (bs.count() > 1) {
                        continue; // More then 1 variables are different, can't reduce
                    }

                    // It can be reduced, so those entries are not prime implicant any more
                    entryL->isPrimeImplicant = false;
                    entryH->isPrimeImplicant = false;

                    Input configuration = {
                        (ilogic_t)(entryL->configuration.value & entryH->configuration.value),
                        (ilogic_t)(entryL->configuration.dontCare | diff)
                    };
                    // Avoid duplicate term
                    if (configurations.find(configuration) != configurations.end()) {
                        continue;
                    }
                    configurations.insert(configuration);

                    unique_ptr<QuineMcCluskeyTableEntry> entry(new QuineMcCluskeyTableEntry());
                    // If both term are don't care, than new term is also don't care
                    entry->isDontCare = entryL->isDontCare && entryH->isDontCare;
                    entry->isPrimeImplicant = true;
                    entry->configuration = configuration;
                    // Merge minterms
                    for (const int& minterm : entryL->minterms) {
                        entry->minterms.emplace(minterm);
                    }
                    for (const int& minterm : entryH->minterms) {
                        entry->minterms.emplace(minterm);
                    }

                    newGroups[group].push_back(move(entry));
                }
            }
        }
        // Print table
        this->verbosePrintQuineMcCluskeyTable(newGroups);

        // Find prime implicant
        for (auto& group : groups) {
            for (auto& entry : group) {
                if (entry->isPrimeImplicant) {
                    primeImplicants.push_back(move(entry));
                }
            }
        }
        groups = move(newGroups);
    }

    // Add prime implicants in last literal
    for (vector<unique_ptr<QuineMcCluskeyTableEntry>>& group : groups) {
        for (unique_ptr<QuineMcCluskeyTableEntry>& entry : group) {
            primeImplicants.push_back(move(entry));
        }
    }

    return move(primeImplicants);
}

/**
 * Use petricks method to pick from Quine-McCluskey chart
 */
vector<PetricksPolynomialProductTerm> QuineMcCluskeyAlgorithm::petricksMethod(vector<unique_ptr<QuineMcCluskeyTableEntry>>& primeImplicants, set<ilogic_t>& minterms)
{
    // Based on the prime implicant chart, build a product of sums of the rows where each row is added, and columns are multiplied together
    map<int, vector<PetricksPolynomialProductTerm>> rows;
    for (size_t i = 0; i < primeImplicants.size(); i++) {
        for (const int& minterm : primeImplicants[i]->minterms) {
            PetricksPolynomialProductTerm term;
            term.variables.insert(i);
            rows[minterm].push_back(term);
        }
    }

    // Use the distributive law to turn that expression into a sum of products
    PetricksPolynomial petricksPolynomial;
    for (const map<int, vector<PetricksPolynomialProductTerm>>::value_type& row : rows) {
        // Ignore don't care term
        if (minterms.find(row.first) != minterms.end()) {
            PetricksPolynomial poly(row.second);
            if (petricksPolynomial.getTerms().size() > 0) {
                petricksPolynomial = petricksPolynomial * poly;
            } else {
                petricksPolynomial = poly;
            }
        }
    }

    this->verbosePrintPetricksPolynomial(petricksPolynomial.getTerms(), primeImplicants);

    // Choose products with fewest terms
    size_t minTerms = SIZE_MAX;
    vector<PetricksPolynomialProductTerm> productsWithFewestTerms;
    for (const PetricksPolynomialProductTerm& term : petricksPolynomial.getTerms()) {
        size_t size = term.variables.size();
        if (size < minTerms) {
            productsWithFewestTerms.clear();
            minTerms = size;
        }
        if (size == minTerms) {
            productsWithFewestTerms.push_back(term);
        }
    }

    this->verbosePrintPetricksPolynomial(productsWithFewestTerms, primeImplicants);

    // Choose term or terms with fewest total literals
    size_t minLiterals = SIZE_MAX;
    vector<PetricksPolynomialProductTerm> termsWithFewestTotalLiterals;
    for (const PetricksPolynomialProductTerm& term : productsWithFewestTerms) {
        size_t count = 0;
        for (const int& variable : term.variables) {
            // Count total literals (non don't care literals)
            bitset<64> bs(primeImplicants[variable]->configuration.dontCare);
            count += (this->inputCount - bs.count());
        }

        if (count < minLiterals) {
            termsWithFewestTotalLiterals.clear();
            minLiterals = count;
        }
        if (count == minLiterals) {
            termsWithFewestTotalLiterals.push_back(term);
        }
    }

    return termsWithFewestTotalLiterals;
}

/**
 * Generate a name from number for reading
 * https://www.geeksforgeeks.org/find-excel-column-name-given-number/
 *
 * n: Number for generation
 */
string QuineMcCluskeyAlgorithm::nameFromNumber(uint64_t n)
{
    string str = "";
    uint64_t i = 0;

    while (n > 0) {
        // Find remainder
        uint64_t rem = n % 26;
        // If remainder is 0, then a 'Z' must be there in output
        if (rem == 0) {
            str = 'Z' + str;
            n = (n / 26) - 1;
        } else {
            // If remainder is non-zero
            char ch = (rem - 1) + 'A';
            str = ch + str;
            n = n / 26;
        }
    }
    return str;
}

/**
 * Prints Quine-McCluskey table in verbose mode
 * 
 * groups: Groups in Quine-McCluskey table to print
 */
void QuineMcCluskeyAlgorithm::verbosePrintQuineMcCluskeyTable(const vector<vector<unique_ptr<QuineMcCluskeyTableEntry>>>& groups)
{
    if (!this->useVerboseMode) {
        return;
    }

    for (const vector<unique_ptr<QuineMcCluskeyTableEntry>>& entries : groups) {
        for (const unique_ptr<QuineMcCluskeyTableEntry>& entry : entries) {
            // Literal and group number
            bitset<64> group(entry->configuration.value);
            bitset<64> literal(entry->configuration.dontCare);
            cout << this->inputCount - literal.count() << "-" << group.count() << "\t";
            // Minterms
            cout << "m";
            for (set<int>::iterator it = entry->minterms.begin(); it != entry->minterms.end(); ++it) {
                if (it != entry->minterms.begin()) {
                    cout << ",";
                }
                cout << *it;
            }
            // Literal term
            cout << "\t" << literalTerm(entry->configuration.value, entry->configuration.dontCare, this->inputCount);
            // Don't care
            if (entry->isDontCare) {
                cout << " (DC)";
            }
            cout << endl;
        }
    }
    cout << "===================================" << endl;
}

/**
 * Prints prime implicants in verbose mode
 * 
 * primeImplicants: Prime implicants to print
 */
void QuineMcCluskeyAlgorithm::verbosePrintPrimeImplicants(const vector<unique_ptr<QuineMcCluskeyTableEntry>>& primeImplicants)
{
    if (!this->useVerboseMode) {
        return;
    }

    cout << "Prime Implicants" << endl;
    cout << "-----------------------------------" << endl;
    for (size_t i = 0; i < primeImplicants.size(); i++) {
        const unique_ptr<QuineMcCluskeyTableEntry>& entry = primeImplicants[i];

        // Literals for prime implicants
        cout << this->nameFromNumber(i + 1) << "\t";
        // Literal and group number
        bitset<64> group(entry->configuration.value);
        bitset<64> literal(entry->configuration.dontCare);
        cout << this->inputCount - literal.count() << "-" << group.count() << "\t";
        // Minterms
        cout << "m";
        for (set<int>::iterator it = entry->minterms.begin(); it != entry->minterms.end(); ++it) {
            if (it != entry->minterms.begin()) {
                cout << ",";
            }
            cout << *it;
        }
        // Literal term
        cout << "\t" << literalTerm(entry->configuration.value, entry->configuration.dontCare, this->inputCount);
        // Don't care
        if (entry->isDontCare) {
            cout << " (DC)";
        }
        cout << endl;
    }
    cout << "===================================" << endl;
}

/**
 * Prints process of petrick's method in verbose mode
 * 
 * terms: Product terms of petrick's method
 * primeImplicants: Prime implicants of Quine-McCluskey chart
 */
void QuineMcCluskeyAlgorithm::verbosePrintPetricksPolynomial(const vector<PetricksPolynomialProductTerm> terms, const vector<unique_ptr<QuineMcCluskeyTableEntry>>& primeImplicants)
{
    if (!this->useVerboseMode) {
        return;
    }

    for (const PetricksPolynomialProductTerm& term : terms) {
        // Count of terms
        cout << term.variables.size() << "\t";
        // Count of literals
        size_t count = 0;
        for (const int& variable : term.variables) {
            bitset<64> bs(primeImplicants[variable]->configuration.dontCare);
            count += (this->inputCount - bs.count());
        }
        cout << count << "\t";
        // Literals terms
        for (set<int>::iterator it = term.variables.begin(); it != term.variables.end(); ++it) {
            if (it != term.variables.begin()) {
                cout << ".";
            }
            cout << this->nameFromNumber(*it + 1);
        }
        cout << endl;
    }
    cout << "===================================" << endl;
}

/**
 * Prints solutions of petrick's method in verbose mode
 * 
 * terms: Product terms of petrick's method
 * primeImplicants: Prime implicants of Quine-McCluskey chart
 * inputLabels: Label of inputs in logic
 */
void QuineMcCluskeyAlgorithm::verbosePrintPetricksPolynomialSolution(const std::vector<PetricksPolynomialProductTerm> terms, const std::vector<std::unique_ptr<QuineMcCluskeyTableEntry>>& primeImplicants, const std::vector<std::string>& inputLabels)
{
    if (!this->useVerboseMode) {
        return;
    }

    cout << "Petrick's Method Solutions" << endl;
    cout << "-----------------------------------" << endl;
    for (const PetricksPolynomialProductTerm& term : terms) {
        // Count of literals
        size_t count = 0;
        for (const int& variable : term.variables) {
            bitset<64> bs(primeImplicants[variable]->configuration.dontCare);
            count += (this->inputCount - bs.count());
        }
        cout << count << "\t";
        // Terms
        for (set<int>::iterator it = term.variables.begin(); it != term.variables.end(); ++it) {
            if (it != term.variables.begin()) {
                cout << ".";
            }
            cout << this->nameFromNumber(*it + 1);
        }
        cout << "\t";
        // Literals terms
        for (set<int>::iterator it = term.variables.begin(); it != term.variables.end(); ++it) {
            if (it != term.variables.begin()) {
                cout << " + ";
            }
            Input configuration = primeImplicants[*it]->configuration;
            for (int i = 0; i < this->inputCount; i++) {
                ilogic_t mask = (ilogic_t)1 << (this->inputCount - 1 - i);
                if ((configuration.dontCare & mask) > 0) {
                    continue;
                }
                cout << inputLabels[i];
                if ((configuration.value & mask) == 0) {
                    cout << "'";
                }
            }
        }
        cout << endl;
    }
    cout << "===================================" << endl;
}
