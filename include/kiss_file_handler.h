/* ==========================================
 * Project04: State Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/12/10
 * ==========================================
 */
#pragma once

#include "digital_logic.h"
#include <exception>
#include <iostream>
#include <memory>

/**
 * Class to handle .kiss format file
 */
class KissFileHandler {
public:
    /**
     * Reads KISS format stream into StateTransitionSet
     * 
     * stream: KISS format stream
     */
    std::unique_ptr<StateTransitionSet> read(std::istream& stream);
    /**
     * Writes the StateTransitionSet into stream in KISS format
     * 
     * ruleSet: The StateTransitionSet to write
     * stream: The steam to write to
     */
    void write(std::unique_ptr<StateTransitionSet>& stateTransitionSet, std::ostream& stream);
};

/**
 * Exception when handling PLA file
 */
class KissFileFormatException : public std::runtime_error {
public:
    /**
     * Construct a KissFileFormatException instance
     * 
     * line: The line in PLA where exception happens
     * token: The token cause the exception
     */
    KissFileFormatException(int line, const std::string token)
        : runtime_error(std::string("kiss:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’")
    {
    }
    /**
     * Construct KissFileFormatException instance
     * 
     * line: The line in PLA where exception happens
     * token: The token cause the exception
     * reason: The reason of the exception
     */
    KissFileFormatException(int line, const std::string token, const std::string reason)
        : runtime_error(std::string("kiss:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’, " + reason)
    {
    }
    /**
     * Construct KissFileFormatException instance
     * 
     * line: The line in PLA where exception happens
     * token: The token cause the exception
     * argIndex: The index of argument that cause the exception
     */
    KissFileFormatException(int line, const std::string token, int argIndex)
        : runtime_error(std::string("kiss:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’, argument index " + std::to_string(argIndex))
    {
    }
};