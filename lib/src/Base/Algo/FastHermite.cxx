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
#include "openturns/FastHermite.hxx"
#include "openturns/FastGaussQuadrature.hxx"
#include "openturns/Point.hxx"
#include "openturns/Exception.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastHermite
 *
 * Fast Gauss-Hermite quadrature for the standard probabilist's Hermite
 * polynomials, orthonormal w.r.t. N(0,1).
 *
 * The symmetric recurrence is: x * p_j = sqrt(j+1) * p_{j+1} + 0 * p_j + sqrt(j) * p_{j-1}
 * So: gamma_j = 0, b_j = sqrt(j), b_0 = 0.
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 */
namespace FastHermite
{
  void ComputeNodesAndWeights(const UnsignedInteger n,
                              Scalar * nodes,
                              Scalar * weights)
  {
    if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (n == 1)
    {
      nodes[0] = 0.0;
      weights[0] = 1.0;
      return;
    }

    // Symmetric recurrence coefficients for orthonormal Hermite polynomials:
    // gamma_j = 0, b_j = sqrt(j), b_0 = 0
    Point gamma(n, 0.0);
    Point b(n, 0.0);
    for (UnsignedInteger j = 1; j < n; ++j)
      b[j] = std::sqrt(static_cast<Scalar>(j));

    // Initial guesses: roots of Hermite polynomial from asymptotic formula
    // Szego 6.32: edge nodes via Airy zeros, bulk by cosine interpolation
    static const Scalar airyAbs[] = {
      2.33811, 4.08795, 5.52056, 6.78671, 7.94413,
      9.02265, 10.04017, 11.00852
    };
    const Scalar r = std::sqrt(2.0 * n + 1.0);
    const Scalar s = std::pow(r, 1.0 / 3.0) / std::sqrt(2.0);
    const UnsignedInteger na = std::min(static_cast<UnsignedInteger>(8), n / 2);
    for (UnsignedInteger i = 0; i < na; ++i)
    {
      nodes[n - 1 - i] = r - s * airyAbs[i];
      nodes[i] = -nodes[n - 1 - i];
    }
    const UnsignedInteger m = n - 2 * na;
    if (m > 0)
    {
      const Scalar lo = nodes[na - 1];
      const Scalar hi = nodes[n - na];
      for (UnsignedInteger i = 0; i < m; ++i)
        nodes[na + i] = lo + (hi - lo) * (i + 1.0) / (m + 1.0);
    }

    FastGaussQuadrature::PolishedSolve(&gamma[0], &b[0], n, nodes, weights);
  }
} // namespace FastHermite

END_NAMESPACE_OPENTURNS
