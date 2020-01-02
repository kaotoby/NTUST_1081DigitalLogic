/* ==========================================
 * Project05: Logic Simulation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2020/01/01
 * ==========================================
 */
#pragma once

#include "digital_logic.h"
#include <exception>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <map>

/**
 * Class to handle .blif format file
 */
class BlifFileHandler {
public:
    /**
     * Construct a new instance of BlifFileHandler
     */
    BlifFileHandler();
    /**
     * Reads .blif format stream into SimpleLogicCircit
     * 
     * stream: .blif format stream
     */
    std::unique_ptr<SimpleLogicCircit> read(std::istream& stream);
private:
    // Conversion map of string and SimpleLogicGate
    std::map<std::string, SimpleLogicGate::GateType> gateTypeMap;
    // Variable to id map
    std::map<std::string, int> variableIdMap;
};

/**
 * Exception when handling .blif format file
 */
class BlifFileFormatException : public std::runtime_error {
public:
    /**
     * Construct a BlifFileFormatException instance
     * 
     * line: The line in .blif where exception happens
     * token: The token cause the exception
     */
    BlifFileFormatException(int line, const std::string token)
        : runtime_error(std::string("blif:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’")
    {
    }
    /**
     * Construct BlifFileFormatException instance
     * 
     * line: The line in .blif where exception happens
     * token: The token cause the exception
     * reason: The reason of the exception
     */
    BlifFileFormatException(int line, const std::string token, const std::string reason)
        : runtime_error(std::string("blif:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’, " + reason)
    {
    }
    /**
     * Construct BlifFileFormatException instance
     * 
     * line: The line in .blif where exception happens
     * token: The token cause the exception
     * argIndex: The index of argument that cause the exception
     */
    BlifFileFormatException(int line, const std::string token, int argIndex)
        : runtime_error(std::string("blif:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’, argument index " + std::to_string(argIndex))
    {
    }
};