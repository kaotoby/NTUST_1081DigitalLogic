#include "smin.h"
#include <exception>
#include <iostream>

/**
 * Program entry point
 * 
 * argc: Argument count
 * argv: Argument string array
 */
int main(int argc, char** argv)
{
    try {
        return Smin::main(argc, argv);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}