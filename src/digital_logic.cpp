/* ==========================================
 * Project03: Exact Boolean Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/11/09
 * ==========================================
 */
#include "digital_logic.h"

using namespace std;

/**
 * Converts the input or output value into literal term
 * 
 * value: The input or output value to convert
 * dontCare: The don't care mask of value
 * length: Total count of input or output variables
 */
string literalTerm(uint64_t value, uint64_t dontCare, int length)
{
    string term(length, '0');
    for (int i = length - 1; i >= 0; i--, dontCare >>= 1, value >>= 1) {
        if ((dontCare & 1) > 0) {
            term[i] = '-';
        } else if ((value & 1) > 0) {
            term[i] = '1';
        }
    }
    return term;
}

/**
 * Get file name without file extension
 * 
 * path: File path
 */
string getFileNameWithoutExt(string path)
{
    string str = path;
    int pos = str.find_last_of('/');
    if (pos != string::npos) {
        str = str.substr(pos + 1, str.length() - pos);
    }
    return str.substr(0, str.find_last_of("."));
}

/**
 * Log2 implementation form https://stackoverflow.com/a/11398748
 */
const int tab64[64] = {
    63, 0, 58, 1, 59, 47, 53, 2,
    60, 39, 48, 27, 54, 33, 42, 3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22, 4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16, 9, 12,
    44, 24, 15, 8, 23, 7, 6, 5
};

/**
 * Log2 implementation form https://stackoverflow.com/a/11398748
 * 
 * value: Value to compute
 */
int log2_64(uint64_t value)
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return tab64[((uint64_t)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}