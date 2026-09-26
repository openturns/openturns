//                                               -*- C++ -*-
/**
 *  @brief Shared polished Gauss quadrature solver
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_FASTGAUSSQUADRATURE_HXX
#define OPENTURNS_FASTGAUSSQUADRATURE_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastGaussQuadrature
 *
 * Shared polished Gauss quadrature solver used by FastHermite, FastLaguerre
 * and FastJacobi.
 *
 * The symmetric three-term recurrence for orthonormal polynomials is:
 *   x * p_j(x) = b_{j+1} * p_{j+1}(x) + gamma_j * p_j(x) + b_j * p_{j-1}(x)
 * with b_0 = 0, p_{-1} = 0, p_0 = 1.
 *
 * Algorithm (the "polished" method from the report):
 * 1. Build symmetric tridiagonal Jacobi matrix: d[i] = gamma[i], e[i] = b[i+1]
 * 2. LAPACK dstev_ with jobz='N' (eigenvalues only, no eigenvectors)
 * 3. Newton refinement of every node using the three-term recurrence
 * 4. Weights from w_i = 1 / (p_{n-1}(x_i) * p'_n(x_i)), normalized to sum to 1
 *
 * The overall complexity is O(n^2): the eigenvalues-only tridiagonal solve,
 * the n Newton refinements (each of cost O(n)) and the weight recovery all
 * scale quadratically with n, without the O(n^3) eigenvector computation.
 *
 * The returned nodes are strictly increasing and the weights are positive,
 * with sum one (probability density convention).
 *
 * This avoids the eigenvector path entirely, which is the source of accuracy
 * degrades for exponentially decaying weights (Laguerre at large n). The
 * Newton refinements operate in the log domain when rescaling the products
 * p_{n-1}(x_i) p'_n(x_i), so the rule stays accurate when those magnitudes
 * underflow or overflow in double precision (e.g. Gauss-Laguerre at large n).
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 *
 * Reference: Yakimiw, E. (1996).
 *            Accurate computation of weights of Gauss-Laguerre and
 *            Gauss-Hermite quadrature formulae.
 */
namespace FastGaussQuadrature
{
  /** Polished Gauss quadrature: eigenvalues-only tridiagonal solve + Newton
   *  refinement + Christoffel weight recovery.
   *
   *  gamma and b must be the recurrence coefficients of the orthonormal
   *  polynomials, i.e. satisfy x * p_j(x) = b_{j+1} * p_{j+1}(x) +
   *  gamma_j * p_j(x) + b_j * p_{j-1}(x) with b_0 = 0, p_{-1} = 0, p_0 = 1.
   *
   *  @param gamma  diagonal of the Jacobi matrix, length n
   *  @param b      off-diagonal of the Jacobi matrix, length n; b[0] is unused,
   *                b[i] for i=1..n-1 are the sub/super-diagonal entries
   *  @param n      number of quadrature nodes (> 0)
   *  @param nodes  output: refined nodes (length n), strictly increasing
   *  @param weights output: quadrature weights (length n), positive and
   *                 normalized to sum to 1
   */
  OT_API void PolishedSolve(const Scalar * gamma,
                     const Scalar * b,
                     const UnsignedInteger n,
                     Scalar * nodes,
                     Scalar * weights);
} // namespace FastGaussQuadrature

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FASTGAUSSQUADRATURE_HXX */
