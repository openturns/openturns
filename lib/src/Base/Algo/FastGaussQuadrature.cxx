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
#include "openturns/FastGaussQuadrature.hxx"
#include "openturns/Point.hxx"
#include "openturns/Lapack.hxx"
#include "openturns/Exception.hxx"
#include <cmath>
#include <algorithm>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastGaussQuadrature
 *
 * Shared polished Gauss quadrature solver.
 *
 * The symmetric three-term recurrence for orthonormal polynomials is:
 *   x * p_j(x) = b_{j+1} * p_{j+1}(x) + gamma_j * p_j(x) + b_j * p_{j-1}(x)
 * with b_0 = 0, p_{-1} = 0, p_0 = 1.
 *
 * The Jacobi matrix J has:
 *   J[i,i]   = gamma[i]       (diagonal)
 *   J[i,i+1] = b[i+1]        (off-diagonal, i = 0..n-2)
 *
 * Nodes are eigenvalues of J; weights are computed from the Christoffel identity
 * or equivalently w_i = 1 / (p_{n-1}(x_i) * p'_n(x_i)).
 */
namespace FastGaussQuadrature
{
  // Evaluate p_{n-1}(x) and the derived p_n(x), p'_n(x) for the recurrence
  //   x * p_j(x) = b_{j+1} * p_{j+1}(x) + gamma_j * p_j(x) + b_j * p_{j-1}(x)
  // with p_{-1} = 0, p_0 = 1.
  // The raw polynomial values grow exponentially with n and overflow double
  // (Hermite around n = 1024, Laguerre around n = 512), so the recurrence is
  // run on values rescaled at every step while accumulating the scale in the
  // log domain. Only the log-magnitudes are returned, together with the signs
  // of p_n(x) and p'_n(x) (signP, signD).
  static void EvaluatePolynomials(const Scalar x,
                                  const Scalar * gamma,
                                  const Scalar * b,
                                  const UnsignedInteger n,
                                  Scalar & logPnm1,
                                  Scalar & logPn,
                                  Scalar & logDpn,
                                  int & signP,
                                  int & signD)
  {
    // Bounds of the mantissas after rescaling, far from the double limits
    const Scalar renormBig = 1.0e150;
    const Scalar renormSmall = 1.0 / renormBig;
    Scalar logScale = 0.0;
    Scalar mpp = 0.0;
    Scalar mpk = 1.0;
    Scalar mdpp = 0.0;
    Scalar mdpk = 0.0;
    for (UnsignedInteger j = 0; j < n - 1; ++j)
    {
      const Scalar bjp1 = b[j + 1];
      Scalar rawP = ((x - gamma[j]) * mpk - b[j] * mpp) / bjp1;
      Scalar rawD = ((x - gamma[j]) * mdpk + mpk - b[j] * mdpp) / bjp1;
      // p_j(x), p_{j+1}(x) and their derivatives scale together, so the
      // recurrence stays exact when they are all rescaled by the same factor
      const Scalar magnitude = std::max(std::abs(mpk), std::max(std::abs(rawP), std::abs(rawD)));
      if (magnitude > renormBig)
      {
        const Scalar factor = magnitude / renormBig;
        mpp /= factor;
        mpk /= factor;
        rawP /= factor;
        mdpp /= factor;
        mdpk /= factor;
        rawD /= factor;
        logScale += std::log(factor);
      }
      else if ((magnitude > 0.0) && (magnitude < renormSmall))
      {
        const Scalar factor = renormBig / magnitude;
        mpp *= factor;
        mpk *= factor;
        rawP *= factor;
        mdpp *= factor;
        mdpk *= factor;
        rawD *= factor;
        logScale -= std::log(factor);
      }
      mpp = mpk;
      mpk = rawP;
      mdpp = mdpk;
      mdpk = rawD;
    }
    // Final recurrence step with b[n] = 0: p_n and p'_n from p_{n-1}, p_{n-2}
    const Scalar pN = (x - gamma[n - 1]) * mpk - b[n - 1] * mpp;
    const Scalar dPN = (x - gamma[n - 1]) * mdpk + mpk - b[n - 1] * mdpp;
    logPnm1 = logScale + std::log(std::abs(mpk));
    logPn = logScale + std::log(std::abs(pN));
    logDpn = logScale + std::log(std::abs(dPN));
    signP = (pN > 0.0) - (pN < 0.0);
    signD = (dPN > 0.0) - (dPN < 0.0);
  }
  void PolishedSolve(const Scalar * gamma,
                     const Scalar * b,
                     const UnsignedInteger n,
                     Scalar * nodes,
                     Scalar * weights)
  {
    if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be > 0";
    if (n == 1)
    {
      nodes[0] = gamma[0];
      weights[0] = 1.0;
      return;
    }

    // Copy diagonal and off-diagonal for LAPACK
    // dstev_ reads and overwrites the diagonal in-place, so the nodes array
    // must be filled with gamma before the call
    Scalar * d = nodes;  // eigenvalues written in-place to nodes
    for (UnsignedInteger i = 0; i < n; ++i)
      d[i] = gamma[i];
    Point e(n - 1);
    for (UnsignedInteger i = 0; i < n - 1; ++i)
      e[i] = b[i + 1];

    // Eigenvalues only (jobz='N')
    char jobz = 'N';
    int ljobz = 1;
    int lwork = std::max(1, 2 * static_cast<int>(n) - 2);
    Point work(lwork);
    Point zDummy(1);
    int info = 0;
    int size = static_cast<int>(n);

    dstev_(&jobz, &size, &d[0], &e[0], &zDummy[0], &size, &work[0], &info, &ljobz);
    if (info != 0) throw InternalException(HERE) << "LAPACK DSTEV error: info=" << info;

    // Newton refinement of every node
    const Scalar tolerance = 1.0e-15;
    Point logFm(n);
    Point logDfm(n);
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      Scalar x = d[i];
      Scalar previousMagnitude = 0.0;
      UnsignedInteger stallCount = 0;
      for (UnsignedInteger iter = 0; iter < 20; ++iter)
      {
        // Newton step p_n(x)/p'_n(x), computed in the log domain so it never
        // overflows nor underflows
        Scalar logPnm1 = 0.0;
        Scalar logPn = 0.0;
        Scalar logDpn = 0.0;
        int signP = 0;
        int signD = 0;
        EvaluatePolynomials(x, gamma, b, n, logPnm1, logPn, logDpn, signP, signD);
        Scalar step = 0.0;
        if ((signP != 0) && (signD != 0))
          step = (signP * signD > 0 ? 1.0 : -1.0) * std::exp(logPn - logDpn);
        x -= step;
        const Scalar magnitude = std::abs(step) / (1.0 + std::abs(x));
        if (magnitude <= tolerance) break;
        if ((iter > 0) && (magnitude >= 0.5 * previousMagnitude))
        {
          ++stallCount;
          if (stallCount >= 2) break;
        }
        else stallCount = 0;
        previousMagnitude = magnitude;
      }
      // Re-evaluate at the final point for the weight computation
      // We need p_{n-1}(x_i) and p'_n(x_i)
      {
        Scalar logPnm1 = 0.0;
        Scalar logPn = 0.0;
        Scalar logDpn = 0.0;
        int signP = 0;
        int signD = 0;
        EvaluatePolynomials(x, gamma, b, n, logPnm1, logPn, logDpn, signP, signD);
        logFm[i] = logPnm1;          // log|p_{n-1}|
        logDfm[i] = logDpn;          // log|p'_n|
      }
      d[i] = x;
    }

    // Christoffel weights w_i = 1 / (p_{n-1}(x_i) * p'_n(x_i)) from the log
    // magnitudes; the product is positive for Gauss rules. Extreme weights
    // may legitimately underflow to zero, their contribution is negligible.
    Scalar totalWeight = 0.0;
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      weights[i] = std::exp(-(logFm[i] + logDfm[i]));
      totalWeight += weights[i];
    }
    // Normalize weights to sum to 1 (pdf convention)
    for (UnsignedInteger i = 0; i < n; ++i)
      weights[i] /= totalWeight;
  }
} // namespace FastGaussQuadrature

END_NAMESPACE_OPENTURNS
