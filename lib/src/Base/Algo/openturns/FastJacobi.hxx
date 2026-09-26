//                                               -*- C++ -*-
/**
 *  @brief Fast Gauss-Jacobi quadrature via polished tridiagonal eigensolver
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
#ifndef OPENTURNS_FASTJACOBI_HXX
#define OPENTURNS_FASTJACOBI_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastJacobi
 *
 * Fast Gauss-Jacobi quadrature via the polished tridiagonal eigensolver.
 *
 * Computes the n-point Gauss-Jacobi rule for the weight function
 * (1-x)^alpha (1+x)^beta on [-1, 1], where alpha > -1 and beta > -1, i.e.
 * the ANALYSIS parameterization of JacobiFactory. The special case
 * alpha + beta == -1 is handled by a canceled form of the b coefficients.
 *
 * The weights are positive, sum to one and the nodes are in strictly
 * increasing order within (-1, 1).
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 *
 * Reference: Gil, A., Segura, J. and Temme, N. M. (2019).
 *            Noniterative computation of Gauss-Jacobi quadrature.
 */
namespace FastJacobi
{
  /** Compute n Gauss-Jacobi nodes and weights.
   *  @param n number of quadrature nodes (> 0)
   *  @param alpha shape parameter (> -1)
   *  @param beta shape parameter (> -1)
   *  @param nodes output array of length n, strictly increasing (nodes in
   *         (-1, 1))
   *  @param weights output array of length n, positive and normalized to sum
   *         to 1
   */
  OT_API void ComputeNodesAndWeights(const UnsignedInteger n,
                              const Scalar alpha,
                              const Scalar beta,
                              Scalar * nodes,
                              Scalar * weights);
} // namespace FastJacobi

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FASTJACOBI_HXX */
