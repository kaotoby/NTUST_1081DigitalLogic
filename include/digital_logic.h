/* ==========================================
 * Project03: Exact Boolean Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/11/09
 * ==========================================
 */
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#define MAX_INPUT_COUNT 16
#define MAX_OUTPUT_COUNT 1

/**
 * Type to store input states
 */
typedef uint16_t ilogic_t;
/**
 * Type to store output states
 */
typedef uint8_t ologic_t;

struct Input {
    // Input value
    ilogic_t value;
    // Mask of don't care bits
    ilogic_t dontCare;

    bool operator==(const Input& o) const
    {
        return value == o.value && dontCare == o.dontCare;
    }
    bool operator<(const Input& o) const
    {
        return (value < o.value) || ((value == o.value) && (dontCare < o.dontCare));
    }
};
/**
 * Represents the input of boolean logic
 */
typedef struct Input Input;

struct Output {
    // Output value
    ologic_t value;
    // Mask of don't care bits
    ologic_t dontCare;

    bool operator==(const Output& o) const
    {
        return value == o.value && dontCare == o.dontCare;
    }
    bool operator<(const Output& o) const
    {
        return (value < o.value) || ((value == o.value) && (dontCare < o.dontCare));
    }
};
/**
 * Represents the output of boolean logic
 */
typedef struct Output Output;

struct LogicRule {
    // Input of the rule
    Input input;
    // Ouput of the rule
    Output output;

    bool operator==(const LogicRule& o) const
    {
        return input == o.input && output == o.output;
    }
    bool operator<(const LogicRule& o) const
    {
        return (input < o.input) || ((input == o.input) && (output < o.output));
    }
};
/**
 * Represents a boolean logic rule with input and output
 */
typedef struct LogicRule LogicRule;

/**
 * A truth table of a boolean logic
 */
typedef struct
{
    // Label of inputs in logic
    std::vector<std::string> inputLabels;
    // Label of outputs in logic
    std::vector<std::string> outputLabels;
    // The configuration of truth table
    std::vector<Output> configurations;
} TruthTable;

/**
 * A set of logic rules
 */
typedef struct
{
    // Label of inputs in logic
    std::vector<std::string> inputLabels;
    // Label of outputs in logic
    std::vector<std::string> outputLabels;
    // The rules in the set
    std::vector<LogicRule> rules;

    /**
     * Convert the rule set in truth table format
     */
    std::unique_ptr<TruthTable> truthTable()
    {
        int length = inputLabels.size();
        std::unique_ptr<TruthTable> table(new TruthTable());
        table->inputLabels = inputLabels;
        table->outputLabels = outputLabels;
        table->configurations.resize(1 << length);

        for (const LogicRule& rule : rules) {
            // Handle rule without don't care terms
            if (rule.input.dontCare == 0) {
                table->configurations[rule.input.value] = rule.output;
                continue;
            }

            // Handle rule with don't care terms
            ilogic_t value = rule.input.value;
            ilogic_t dontCare = rule.input.dontCare;
            std::vector<int> dontCareBits;
            dontCareBits.reserve(length);

            // Get don't care bits indexes
            for (int i = 0; i < length; i++, dontCare >>= 1) {
                if ((dontCare & 1) > 0) {
                    dontCareBits.push_back(i);
                }
            }
            // Build every possible states of don't care bits
            for (ilogic_t state = 0; state < ((ilogic_t)1 << dontCareBits.size()); state++) {
                for (int i = 0; i < dontCareBits.size(); i++) {
                    // Apply current state of don't care bits to value
                    if ((state & ((ilogic_t)1 << i)) > 0) {
                        value |= (ilogic_t)1 << dontCareBits[i];
                    } else {
                        value &= ~((ilogic_t)1 << dontCareBits[i]);
                    }
                }
                // Put to table
                table->configurations[value] = rule.output;
            }
        }

        return move(table);
    }
} LogicRuleSet;

/**
 * A state transition
 */
typedef struct
{
    // Input of the rule
    Input input;
    // Ouput of the rule
    Output output;
    // From state
    unsigned int from;
    // To state
    unsigned int to;
} StateTransition;

/**
 * A set of state transitions
 */
typedef struct
{
    // Label of inputs in logic
    std::vector<std::string> inputLabels;
    // Label of outputs in logic
    std::vector<std::string> outputLabels;
    // Label of state in logic
    std::vector<std::string> stateLabels;
    // The rules in the set
    std::vector<StateTransition> transitions;
} StateTransitionSet;

/**
 * Converts the input or output value into literal term
 * 
 * value: The input or output value to convert
 * dontCare: The don't care mask of value
 * length: Total count of input or output variables
 */
std::string literalTerm(uint64_t value, uint64_t dontCare, int length);

/**
 * Get file name without file extension
 * 
 * path: File path
 */
std::string getFileNameWithoutExt(std::string path);

/**
 * Log2 implementation form https://stackoverflow.com/a/11398748
 * 
 * value: Value to compute
 */
int log2_64(uint64_t value);
