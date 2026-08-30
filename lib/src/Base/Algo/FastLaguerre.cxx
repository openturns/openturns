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
#include "openturns/FastLaguerre.hxx"
#include "openturns/FastGaussQuadrature.hxx"
#include "openturns/Point.hxx"
#include "openturns/Exception.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastLaguerre
 *
 * Fast Gauss-Laguerre quadrature via the polished tridiagonal eigensolver.
 *
 * The symmetric recurrence for orthonormal Laguerre polynomials with
 * weight x^{k-1} exp(-x) is:
 *   gamma_j = 2*j + k
 *   b_j = sqrt(j * (j + k - 1)),  b_0 = 0
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 */
namespace FastLaguerre
{
  void ComputeNodesAndWeights(const UnsignedInteger n,
                              const Scalar k,
                              Scalar * nodes,
                              Scalar * weights)
  {
    if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (k <= 0.0) throw InvalidArgumentException(HERE) << "Error: k must be > 0";
    if (n == 1)
    {
      nodes[0] = k;
      weights[0] = 1.0;
      return;
    }

    // Symmetric recurrence coefficients for orthonormal Laguerre polynomials:
    // gamma_j = 2*j + k, b_j = sqrt(j * (j + k - 1)), b_0 = 0
    Point gamma(n);
    Point b(n, 0.0);
    for (UnsignedInteger j = 0; j < n; ++j)
      gamma[j] = 2.0 * j + k;
    for (UnsignedInteger j = 1; j < n; ++j)
      b[j] = std::sqrt(j * (j + k - 1.0));

    // Initial guesses: asymptotic formula (fg_core.py)
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      const Scalar s = static_cast<Scalar>(i + 1);
      nodes[i] = M_PI * M_PI * std::pow(s - (k - 1.0) / 4.0 - 0.25, 2.0) / (4.0 * n + 2.0 * k);
    }

    FastGaussQuadrature::PolishedSolve(&gamma[0], &b[0], n, nodes, weights);
  }
} // namespace FastLaguerre

END_NAMESPACE_OPENTURNS
