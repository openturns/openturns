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
#include "openturns/FastLegendre.hxx"
#include "openturns/Exception.hxx"
#include <algorithm>

// Use the Bogaert fastgl library directly (already compiled in the Algo library)
#include "fastgl.h"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastLegendre
 *
 * Fast Gauss-Legendre quadrature wrapping Bogaert's fastgl library.
 *
 * Nodes and weights are computed in O(1) per pair via the iteration-free
 * formulas of Bogaert (2014). Nodes are in [-1, 1] and weights sum to 2.
 *
 * Reference: Bogaert, I. (2014).
 *            Iteration-Free Computation of Gauss-Legendre Quadrature Nodes and Weights.
 *            SIAM Journal on Scientific Computing, 36(3), A1008-A1026.
 */
namespace FastLegendre
{
  void ComputeNodesAndWeights(const UnsignedInteger n,
                              Scalar * nodes,
                              Scalar * weights)
  {
    if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (n == 1)
    {
      nodes[0] = 0.0;
      weights[0] = 2.0;
      return;
    }
    for (UnsignedInteger k = 1; k <= n; ++k)
    {
      const fastgl::QuadPair qp = fastgl::GLPair(n, k);
      nodes[k - 1] = std::cos(qp.theta);
      weights[k - 1] = qp.weight;
    }
    // fastgl returns the nodes in strictly decreasing order; reverse them so
    // that all Fast* classes and the standard factories return increasing nodes
    std::reverse(nodes, nodes + n);
    std::reverse(weights, weights + n);
  }
} // namespace FastLegendre

END_NAMESPACE_OPENTURNS
