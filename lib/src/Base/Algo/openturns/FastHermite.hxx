//                                               -*- C++ -*-
/**
 *  @brief Fast Gauss-Hermite quadrature via polished tridiagonal eigensolver
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
#ifndef OPENTURNS_FASTHERMITE_HXX
#define OPENTURNS_FASTHERMITE_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastHermite
 *
 * Fast Gauss-Hermite quadrature for the standard probabilist's Hermite
 * polynomials, orthonormal w.r.t. N(0,1).
 *
 * Uses the polished method: eigenvalues-only tridiagonal solve + Newton
 * refinement + Christoffel weights. The recurrence coefficients are
 * gamma_j = 0, b_j = sqrt(j).
 *
 * The weight function is the standard normal density exp(-x^2/2)/sqrt(2*pi),
 * so the weights are positive, sum to one and the nodes are in strictly
 * increasing order. The largest node is O(sqrt(2n)), so the rule stays well
 * within double range even for thousands of points. The magnitudes
 * p_{n-1}(x) and p'_n(x) grow exponentially with n and would overflow in
 * double precision for large rules (observed around n = 1024); the solver in
 * FastGaussQuadrature rescales them in the log domain during the Newton
 * refinement and the weight recovery, so the rule stays accurate without any
 * limit on n.
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 *
 * Reference: Townsend, A., Trogdon, T. and Olver, S. (2016).
 *            Fast computation of Gauss quadrature nodes and weights
 *            on the whole real line.
 *            IMA Journal of Numerical Analysis, 36(2), 802-824.
 */
namespace FastHermite
{
  /** Compute n standard Gauss-Hermite nodes and weights.
   *  @param n number of quadrature nodes (> 0)
   *  @param nodes output array of length n, strictly increasing
   *  @param weights output array of length n, positive and normalized to sum
   *         to 1
   */
  OT_API void ComputeNodesAndWeights(const UnsignedInteger n,
                              Scalar * nodes,
                              Scalar * weights);
} // namespace FastHermite

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FASTHERMITE_HXX */
