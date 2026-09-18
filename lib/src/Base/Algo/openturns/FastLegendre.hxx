//                                               -*- C++ -*-
/**
 *  @brief Fast Gauss-Legendre quadrature wrapping Bogaert's fastgl library
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
#ifndef OPENTURNS_FASTLEGENDRE_HXX
#define OPENTURNS_FASTLEGENDRE_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/Point.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastLegendre
 *
 * Fast Gauss-Legendre quadrature wrapping Bogaert's fastgl library.
 *
 * Nodes and weights are computed in O(n) total time via the iteration-free
 * formulas of Bogaert (2014), one (node, weight) pair per coefficient.
 * Nodes are in [-1, 1] and weights sum to 2 (Lebesgue measure), i.e. twice
 * the weights of LegendreFactory which is normalized as a probability
 * measure. This is the same 1-D marginal rule used by the GaussLegendre
 * quadrature class.
 *
 * As in the other Fast* families, the nodes are returned in strictly
 * increasing order.
 *
 * Reference: Bogaert, I. (2014).
 *            Iteration-Free Computation of Gauss-Legendre Quadrature Nodes and Weights.
 *            SIAM Journal on Scientific Computing, 36(3), A1008-A1026.
 */
namespace FastLegendre
{
  /** Compute n standard Gauss-Legendre nodes and weights on [-1, 1].
   *  @param n number of quadrature nodes (> 0)
   *  @param nodes output array of length n, in strictly increasing order
   *  @param weights output array of length n, positive and summing to 2
   */
  OT_API void ComputeNodesAndWeights(const UnsignedInteger n,
                              Scalar * nodes,
                              Scalar * weights);
} // namespace FastLegendre

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FASTLEGENDRE_HXX */
