/* ==========================================
 * Project03: Exact Boolean Minimization
 * Author: B10630221 Chang-Ting Kao
 * Date: 2019/11/09
 * ==========================================
 */
#pragma once

#include <set>
#include <vector>

struct PetricksPolynomialProductTerm {
    // Variables in a product term
    std::set<int> variables;

    friend PetricksPolynomialProductTerm operator*(const PetricksPolynomialProductTerm& t1, const PetricksPolynomialProductTerm& t2);
};
/**
 * A product term inside the polynomial for petrick's method
 */
typedef struct PetricksPolynomialProductTerm PetricksPolynomialProductTerm;

/**
 * A polynomial for petrick's method
 */
class PetricksPolynomial {
private:
    // The product terms sum together
    std::vector<PetricksPolynomialProductTerm> terms;

public:
    /**
     * Construct a PetricksPolynomial instance
     */
    PetricksPolynomial();
    /**
     * Construct a PetricksPolynomial instance using terms
     * 
     * terms: The product terms of the polynomial
     */
    PetricksPolynomial(const std::vector<PetricksPolynomialProductTerm>& terms);

    /**
     * Gets the product terms of the polynomial
     */
    const std::vector<PetricksPolynomialProductTerm> getTerms() { return terms; }

    friend PetricksPolynomial operator+(const PetricksPolynomial& p1, const PetricksPolynomialProductTerm& t1);
    friend PetricksPolynomial operator+(const PetricksPolynomial& p1, const PetricksPolynomial& p2);
    friend PetricksPolynomial operator*(const PetricksPolynomial& p1, const PetricksPolynomialProductTerm& t1);
    friend PetricksPolynomial operator*(const PetricksPolynomial& p1, const PetricksPolynomial& p2);
};
