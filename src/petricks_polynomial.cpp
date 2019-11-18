/* ==========================================
 * Project03: Exact Boolean Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/11/09
 * ==========================================
 */
#include "petricks_polynomial.h"
#include <algorithm>
#include <set>

using namespace std;

/**
 * Construct a PetricksPolynomial instance
 */
PetricksPolynomial::PetricksPolynomial()
{
}
/**
 * Construct a PetricksPolynomial instance using terms
 * 
 * terms: The product terms of the polynomial
 */
PetricksPolynomial::PetricksPolynomial(const vector<PetricksPolynomialProductTerm>& terms)
    : terms(terms)
{
}

PetricksPolynomialProductTerm operator*(const PetricksPolynomialProductTerm& t1, const PetricksPolynomialProductTerm& t2)
{
    struct PetricksPolynomialProductTerm t = t1;
    for (const int& v : t2.variables) {
        // XX = X (using set)
        t.variables.insert(v);
    }
    return t;
}

PetricksPolynomial operator+(const PetricksPolynomial& p1, const PetricksPolynomialProductTerm& t1)
{
    PetricksPolynomial p = p1;
    for (PetricksPolynomialProductTerm& term : p.terms) {
        const PetricksPolynomialProductTerm* left;
        const PetricksPolynomialProductTerm* right;
        if (term.variables.size() > t1.variables.size()) {
            left = &term;
            right = &t1;
        } else {
            left = &t1;
            right = &term;
        }
        // Check right is subset of left
        if (includes(left->variables.begin(), left->variables.end(), right->variables.begin(), right->variables.end())) {
            // X+X = X, X+XY = X
            term = *right;
            return p;
        }
    }
    p.terms.push_back(t1);
    return p;
}

PetricksPolynomial operator+(const PetricksPolynomial& p1, const PetricksPolynomial& p2)
{
    PetricksPolynomial p = p1;
    for (const PetricksPolynomialProductTerm& term : p2.terms) {
        p = p + term;
    }
    return p;
}

PetricksPolynomial operator*(const PetricksPolynomial& p1, const PetricksPolynomialProductTerm& t1)
{
    PetricksPolynomial p = p1;
    for (PetricksPolynomialProductTerm& term : p.terms) {
        term = term * t1;
    }
    return p;
}

PetricksPolynomial operator*(const PetricksPolynomial& p1, const PetricksPolynomial& p2)
{
    PetricksPolynomial p;
    for (const PetricksPolynomialProductTerm& term : p2.terms) {
        p = p + (p1 * term);
    }
    return p;
}