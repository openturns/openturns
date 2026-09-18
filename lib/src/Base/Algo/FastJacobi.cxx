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
#include "openturns/FastJacobi.hxx"
#include "openturns/FastGaussQuadrature.hxx"
#include "openturns/Point.hxx"
#include "openturns/Exception.hxx"
#include <cmath>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastJacobi
 *
 * Fast Gauss-Jacobi quadrature via the polished tridiagonal eigensolver.
 *
 * The symmetric recurrence for orthonormal Jacobi polynomials with
 * weight (1-x)^alpha (1+x)^beta on [-1,1] is:
 *   gamma_j = (beta^2 - alpha^2) / ((2j+alpha+beta)(2j+alpha+beta+2))
 *   b_j^2 = 4j(j+alpha)(j+beta)(j+alpha+beta) / ((2j+alpha+beta)^2*(2j+alpha+beta+1)*(2j+alpha+beta-1))
 * with b_0 = 0.
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 */
namespace FastJacobi
{
  void ComputeNodesAndWeights(const UnsignedInteger n,
                              const Scalar alpha,
                              const Scalar beta,
                              Scalar * nodes,
                              Scalar * weights)
  {
    if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (alpha <= -1.0) throw InvalidArgumentException(HERE) << "Error: alpha must be > -1";
    if (beta <= -1.0) throw InvalidArgumentException(HERE) << "Error: beta must be > -1";
    if (n == 1)
    {
      nodes[0] = (beta - alpha) / (alpha + beta + 2.0);
      weights[0] = 1.0;
      return;
    }

    // Symmetric recurrence coefficients for orthonormal Jacobi polynomials
    const Scalar ab = alpha + beta;
    Point gamma(n);
    Point b(n, 0.0);
    gamma[0] = (beta - alpha) / (ab + 2.0);
    for (UnsignedInteger j = 1; j < n; ++j)
    {
      const Scalar s = static_cast<Scalar>(j);
      const Scalar t = 2.0 * s + ab;
      gamma[j] = (beta * beta - alpha * alpha) / (t * (t + 2.0));
      if (t == 1.0)
        // canceled form for ab == -1, j == 1
        b[j] = 2.0 / t * std::sqrt((s + alpha) * (s + beta) / (t + 1.0));
      else
        b[j] = 2.0 / t * std::sqrt(s * (s + alpha) * (s + beta) * (s + ab) / ((t + 1.0) * (t - 1.0)));
    }

    FastGaussQuadrature::PolishedSolve(&gamma[0], &b[0], n, nodes, weights);
  }
} // namespace FastJacobi

END_NAMESPACE_OPENTURNS
