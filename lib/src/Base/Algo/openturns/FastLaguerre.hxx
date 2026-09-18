//                                               -*- C++ -*-
/**
 *  @brief Fast Gauss-Laguerre quadrature via polished tridiagonal eigensolver
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
#ifndef OPENTURNS_FASTLAGUERRE_HXX
#define OPENTURNS_FASTLAGUERRE_HXX

#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastLaguerre
 *
 * Fast Gauss-Laguerre quadrature via the polished tridiagonal eigensolver.
 *
 * Computes the n-point Gauss-Laguerre rule for the weight function
 * x^{k-1} exp(-x) / Gamma(k) on [0, inf), i.e. the Gamma(k, 1, 0) density,
 * where k > 0 is the shape parameter. This is the rule associated with the
 * ANALYSIS parameterization of LaguerreFactory(k-1).
 *
 * The weights are positive, sum to one and the nodes are in strictly
 * increasing order in [0, inf). The magnitudes p_{n-1}(x) p'_n(x) may
 * underflow or overflow in double precision for large n and/or small k; the
 * solver in FastGaussQuadrature rescales them in the log domain so the rule
 * stays accurate without any limit on n.
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 *
 * Reference: Gil, A., Segura, J. and Temme, N. M. (2018).
 *            Computation of Gauss-Laguerre and Gauss-Hermite
 *            quadrature rules.
 */
namespace FastLaguerre
{
  /** Compute n Gauss-Laguerre nodes and weights for Gamma(k, 1, 0).
   *  @param n number of quadrature nodes (> 0)
   *  @param k shape parameter of the Gamma distribution (must be > 0)
   *  @param nodes output array of length n, strictly increasing (nodes in
   *         [0, inf))
   *  @param weights output array of length n, positive and normalized to sum
   *         to 1
   */
  OT_API void ComputeNodesAndWeights(const UnsignedInteger n,
                              const Scalar k,
                              Scalar * nodes,
                              Scalar * weights);
} // namespace FastLaguerre

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FASTLAGUERRE_HXX */
