/* ==========================================
 * Project02: ROBDD Generation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/10/07
 * ==========================================
 */
#pragma once

#include "digital_logic.h"
#include <exception>
#include <iostream>
#include <memory>

/**
 * Class to handle .pla format file
 */
class PlaFileHandler {
public:
    /**
     * Reads PLA format stream into LogicRuleSet
     * 
     * stream: PLA format stream
     */
    std::unique_ptr<LogicRuleSet> read(std::istream& stream);
    /**
     * Writes the LogicRuleSet into stream in PLA format
     * 
     * ruleSet: The LogicRuleSet to write
     * stream: The steam to write to
     */
    void write(std::unique_ptr<LogicRuleSet>& ruleSet, std::ostream& stream);
};

/**
 * Exception when handling PLA file
 */
class PlaFileFormatException : public std::runtime_error {
public:
    /**
     * Construct a PlaFileFormatException instance
     * 
     * line: The line in PLA where exception happens
     * token: The token cause the exception
     */
    PlaFileFormatException(int line, const std::string token)
        : runtime_error(std::string("pla:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’")
    {
    }
    /**
     * Construct PlaFileFormatException instance
     * 
     * line: The line in PLA where exception happens
     * token: The token cause the exception
     * reason: The reason of the exception
     */
    PlaFileFormatException(int line, const std::string token, const std::string reason)
        : runtime_error(std::string("pla:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’, " + reason)
    {
    }
    /**
     * Construct PlaFileFormatException instance
     * 
     * line: The line in PLA where exception happens
     * token: The token cause the exception
     * argIndex: The index of argument that cause the exception
     */
    PlaFileFormatException(int line, const std::string token, int argIndex)
        : runtime_error(std::string("pla:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’, argument index " + std::to_string(argIndex))
    {
    }
};