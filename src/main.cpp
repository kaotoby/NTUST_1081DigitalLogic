/* ==========================================
 * Project05: Logic Simulation
 * Author: B10630221 Chang-Ting Kao
 * Date: 2020/01/01
 * ==========================================
 */
#include "lsim.h"
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
        return Lsim::main(argc, argv);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}