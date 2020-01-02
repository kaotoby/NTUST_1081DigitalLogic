/* ==========================================
 * Project05: Logic Simulation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2020/01/01
 * ==========================================
 */
#pragma once

#include <exception>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

/**
 * Class to handle .sti format file
 */
class StiFileHandler {
public:
    /**
     * Reads .sti format stream into vector of string
     * 
     * stream: .sti format stream
     */
    std::unique_ptr<std::vector<std::string>> read(std::istream& stream);
};

/**
 * Exception when handling .sti format file
 */
class StiFileFormatException : public std::runtime_error {
public:
    /**
     * Construct a StiFileFormatException instance
     * 
     * line: The line in .sti where exception happens
     * token: The token cause the exception
     */
    StiFileFormatException(int line, const std::string token)
        : runtime_error(std::string("sti:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’")
    {
    }
    /**
     * Construct StiFileFormatException instance
     * 
     * line: The line in .sti where exception happens
     * token: The token cause the exception
     * reason: The reason of the exception
     */
    StiFileFormatException(int line, const std::string token, const std::string reason)
        : runtime_error(std::string("sti:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’, " + reason)
    {
    }
    /**
     * Construct StiFileFormatException instance
     * 
     * line: The line in .sti where exception happens
     * token: The token cause the exception
     * argIndex: The index of argument that cause the exception
     */
    StiFileFormatException(int line, const std::string token, int argIndex)
        : runtime_error(std::string("sti:") + std::to_string(line) + " \033[1;31merror:\033[0m Parsing error at token ‘" + token + "’, argument index " + std::to_string(argIndex))
    {
    }
};