/* ==========================================
 * Project03: Exact Boolean Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/11/09
 * ==========================================
 */
#include "mini.h"
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
        return Mini::main(argc, argv);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}