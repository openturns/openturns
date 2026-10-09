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
#include "openturns/ResourceMap.hxx"
#include <cmath>
#include <algorithm>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastLaguerre details
 *
 * Iterative Gil-Segura-Temme path (Prufer fixed-point sweeps in
 * z = sqrt(x), Taylor stepping, continued-fraction starts, scaled
 * weights). Reimplemented from the equations of Gil, A., Segura, J. and
 * Temme, N. M. (2019). Fast, reliable and unrestricted iterative
 * computation of Gauss-Hermite and Gauss-Laguerre quadratures.
 * Numer. Math. 143(3).
 */

// Fixed-point tolerance of the Prufer sweeps (tighter than the paper's
// 1e-12; 3e-15 validated against the multiprecision oracle)
static const Scalar LaguerreFixedPointTolerance = 3.0e-15;
// Overflow guard scale of the Taylor stepping (paper's choice)
static const Scalar LaguerreTaylorScale = 1.0e-280;

// Log-derivative ratio dot(y)/y at x by degree recurrence (n < 10)
static Scalar RecurrenceStartRatio(const UnsignedInteger n,
                                   const Scalar alpha,
                                   const Scalar x)
{
  Scalar r = alpha + 1.0 - x;
  for (UnsignedInteger i = 1; i < n; ++i)
    r = ((2.0 * i + alpha + 1.0 - x) - (i + alpha) / r) / (i + 1.0);
  const Scalar z = std::sqrt(x);
  if (std::abs(r) > 1.0e-200)
    return (alpha + 0.5) / z - z + 2.0 / z * (n - (n + alpha) / r);
  return 1.0e200;
}

// Log-derivative ratio by continued fraction
static Scalar ContinuedFractionStartRatio(const UnsignedInteger n,
                                          const Scalar alpha,
                                          const Scalar x)
{
  Scalar alphai = alpha + 1.0;
  const Scalar bp = -(1.0 + alpha / x);
  const Scalar ap = -(alpha + n) / x;
  Scalar b = -(1.0 + alphai / x);
  Scalar a = -(alphai + n) / x;
  Scalar E = b;
  Scalar F = b + a / b;
  Scalar cf = ap / (bp + a / b);
  Scalar del = std::abs(E / F - 1.0);
  UnsignedInteger guard = 0;
  while ((del > 5.0e-16) && (guard < 10000))
  {
    ++guard;
    alphai += 1.0;
    b = -(1.0 + alphai / x);
    a = -(alphai + n) / x;
    E = b + a / E;
    F = b + a / F;
    cf = cf * (E / F);
    del = std::abs(E / F - 1.0);
  }
  const Scalar z = std::sqrt(x);
  return (alpha + 0.5) / z - z + 2.0 / z * (-alpha + (alpha + n) / cf);
}

static Scalar StartLogDerivative(const UnsignedInteger n,
                                 const Scalar alpha,
                                 const Scalar x)
{
  if (n < 10) return RecurrenceStartRatio(n, alpha, x);
  return ContinuedFractionStartRatio(n, alpha, x);
}

// Taylor step of y and y' from z0 to z0 + h, scaled against overflow.
// Returns false when the series does not converge within the term cap
// (step outside the radius of convergence): the caller falls back.
static Bool TaylorStep(const UnsignedInteger n,
                       const Scalar alpha,
                       const Scalar z0,
                       const Scalar h,
                       const Scalar f0,
                       const Scalar f1,
                       Scalar & y,
                       Scalar & yd)
{
  static const UnsignedInteger TaylorMaxTerms = 1000;
  Scalar sf0 = f0 * LaguerreTaylorScale;
  Scalar sf1 = f1 * LaguerreTaylorScale;
  const Scalar L = 2.0 * n + alpha + 1.0;
  const Scalar qv = 0.25 - alpha * alpha + 2.0 * L * z0 * z0 - z0 * z0 * z0 * z0;
  const Scalar qv1 = 4.0 * z0 * (L - z0 * z0);
  const Scalar qv2 = 4.0 * (L - 3.0 * z0 * z0);
  const Scalar qv3 = -24.0 * z0;
  const Scalar qv4 = -24.0;
  const Scalar pv = z0 * z0;
  const Scalar pv1 = 2.0 * z0;
  const Scalar pv2 = 2.0;
  Scalar fm1 = 0.0;
  Scalar fm2 = 0.0;
  Scalar fm3 = 0.0;
  Scalar fm4 = 0.0;
  Scalar coe = 1.0;
  Scalar sd = sf1;
  coe = coe * h;
  Scalar sf = sf0 + coe * sf1;
  Scalar error = 1.0;
  UnsignedInteger j = 0;
  while (((error > 1.0e-25) || (j < 10)) && (j < TaylorMaxTerms))
  {
    const Scalar c0 = j;
    const Scalar c1 = c0 * (j - 1.0) / 2.0;
    const Scalar c2 = c1 * (j - 2.0) / 3.0;
    const Scalar c3 = c2 * (j - 3.0) / 4.0;
    const Scalar f2 = -1.0 / pv * (c0 * pv1 * sf1 + (c1 * pv2 + qv) * sf0 + c0 * qv1 * fm1 + c1 * qv2 * fm2 + c2 * qv3 * fm3 + c3 * qv4 * fm4);
    const Scalar ad2 = coe * f2;
    sd = sd + ad2;
    coe = coe * h / (j + 2.0);
    sf = sf + coe * f2;
    error = std::abs(ad2 / sd);
    fm4 = fm3;
    fm3 = fm2;
    fm2 = fm1;
    fm1 = sf0;
    sf0 = sf1;
    sf1 = f2;
    ++j;
  }
  y = sf / LaguerreTaylorScale;
  yd = sd / LaguerreTaylorScale;
  return (j < TaylorMaxTerms) && (error <= 1.0e-25);
}

// Iterative Prufer sweeps in z = sqrt(x): forward sweep from the lower
// bound, then backward sweep; scaled weights we = 1/yd^2 with elementary
// log-factor bookkeeping, sum-normalized at the end.
// Returns false when the sweep misses nodes (large alpha at small n: the
// first forward steps overshoot the sparse nodes above the starting point),
// in which case the caller falls back to the polished eigensolver.
static Bool ComputeNodesAndWeightsIterative(const UnsignedInteger n,
                                            const Scalar k,
                                            Point & nodes,
                                            Point & weights)
{
  const Scalar alpha = k - 1.0;
  Point xc;
  Point we;
  Point expi;
  const Scalar xr = (2.0 * n * n + n * (alpha - 1.0) + 2.0 * (alpha + 1.0) + 2.0 * (n - 1.0) * std::sqrt(n * n + (n + 2.0) * (alpha + 1.0))) / (n + 2.0);
  const Scalar prod = (alpha + 1.0) / (n + 2.0) * (n * (alpha + 5.0) + 2.0 * (alpha - 1.0));
  const Scalar xl = prod / xr;
  const Scalar zl = std::sqrt(xl);
  const Scalar zr = std::sqrt(xr);
  Scalar x = (std::abs(alpha) > 0.5) ? std::sqrt(alpha * alpha - 0.25) : xl;
  const Scalar xini = x;
  Scalar z = std::sqrt(x);
  const Scalar L = 2.0 * n + alpha + 1.0;
  Scalar hph = StartLogDerivative(n, alpha, x);
  Scalar f0 = 1.0 / hph;
  Scalar f1 = 1.0;
  Scalar hf = 1.0 / hph;
  const Scalar w2Init = (0.25 - alpha * alpha + 2.0 * L * z * z - z * z * z * z) / (z * z);
  if (!(w2Init > 0.0)) return false;
  Scalar w = std::sqrt(w2Init);
  Scalar h = -std::atan(w * hf) / w;
  if (h < 0.0) h += M_PI / w;
  Scalar z0 = z;
  z = z + h;
  UnsignedInteger ij = 0;
  UnsignedInteger i = 0;
  SignedInteger j = -3;
  Scalar expis = 0.0;
  expi.add(expis);
  while (j < 0)
  {
    j += 2;
    while ((z < zr) && (z > zl))
    {
      ++i;
      Scalar err = 1.0;
      while ((err > LaguerreFixedPointTolerance) && (z < zr) && (z > zl))
      {
        if (((j < 0) && (i > 2)) || (j > 0))
        {
          Scalar y = 0.0;
          Scalar yd = 0.0;
          if (!TaylorStep(n, alpha, z0, h, f0, f1, y, yd)) return false;
          f0 = y;
          f1 = yd;
          hf = y / yd;
        }
        else
        {
          hph = StartLogDerivative(n, alpha, z * z);
          hf = 1.0 / hph;
          f0 = 1.0 / hph;
          f1 = 1.0;
        }
        const Scalar w2 = (0.25 - alpha * alpha + 2.0 * L * z * z - z * z * z * z) / (z * z);
        if (w2 > 0.0)
        {
          w = std::sqrt(w2);
          h = -std::atan(w * hf) / w;
        }
        else
        {
          w = std::sqrt(-w2);
          h = -std::atanh(w * hf) / w;
        }
        z0 = z;
        z = z + h;
        err = std::abs(h / z);
      }
      if ((err < LaguerreFixedPointTolerance) && (z < zr) && (z > zl))
      {
        xc.add(z * z);
        if (i > 1)
        {
          expis = (alpha + 0.5) * std::log(xc[xc.getSize() - 1] / xc[0]) + xc[0] - xc[xc.getSize() - 1];
          expi.add(expis);
        }
        Scalar yd = 0.0;
        {
          Scalar y = 0.0;
          if (!TaylorStep(n, alpha, z0, h, f0, f1, y, yd)) return false;
        }
        we.add(1.0 / (yd * yd));
        f0 = 0.0;
        f1 = yd;
      }
      else
      {
        --i;
      }
      const Scalar w2 = (0.25 - alpha * alpha + 2.0 * L * z * z - z * z * z * z) / (z * z);
      if (w2 > 0.0)
      {
        z0 = z;
        w = std::sqrt(w2);
        h = -j * M_PI / w;
        z = z + h;
      }
      else
      {
        z = 0.0;
      }
    }
    // Continued fraction for the first two zeros (needs at least 3 nodes)
    if (xc.getSize() < 3) return false;
    z0 = std::sqrt(xc[2]);
    h = std::sqrt(xc[1]) - std::sqrt(xc[2]);
    f0 = 0.0;
    f1 = 1.0 / std::sqrt(we[2]);
    {
      Scalar y = 0.0;
      Scalar yd = 0.0;
      if (!TaylorStep(n, alpha, z0, h, f0, f1, y, yd)) return false;
      we[1] = 1.0 / (yd * yd);
      z0 = std::sqrt(xc[1]);
      h = std::sqrt(xc[0]) - std::sqrt(xc[1]);
      f0 = 0.0;
      f1 = yd;
      if (!TaylorStep(n, alpha, z0, h, f0, f1, y, yd)) return false;
      we[0] = 1.0 / (yd * yd);
      f0 = 0.0;
      f1 = yd;
    }
    if (j < 0)
    {
      expis = 0.0;
      ij = i;
      x = xini;
      z = std::sqrt(x);
      z0 = std::sqrt(xc[0]);
      h = z - z0;
      {
        Scalar y = 0.0;
        Scalar yd = 0.0;
        if (!TaylorStep(n, alpha, z0, h, f0, f1, y, yd)) return false;
        f0 = y;
        f1 = yd;
      }
      hf = f0 / f1;
      const Scalar w2 = (0.25 - alpha * alpha + 2.0 * L * z * z - z * z * z * z) / (z * z);
      if (!(w2 > 0.0)) return false;
      w = std::sqrt(w2);
      h = -std::atan(w * hf) / w;
      if (h > 0.0) h -= M_PI / w;
      z0 = z;
      z = z + h;
    }
  }
  // Reassemble descending tail + ascending head, sum-normalize.
  // Any structural defect falls back to the polished eigensolver.
  if (xc.getSize() != n) return false;
  if (we.getSize() != n) return false;
  if (expi.getSize() != n) return false;
  nodes = Point(n, 0.0);
  weights = Point(n, 0.0);
  Scalar total = 0.0;
  for (UnsignedInteger q = 0; q < n; ++q)
    total += we[q] * std::exp(expi[q]);
  if (!(total > 0.0)) return false;
  UnsignedInteger p = 0;
  for (SignedInteger q = static_cast<SignedInteger>(i) - 1; q >= static_cast<SignedInteger>(ij); --q)
  {
    nodes[p] = xc[q];
    weights[p] = we[q] * std::exp(expi[q]) / total;
    ++p;
  }
  for (UnsignedInteger q = 0; q < ij; ++q)
  {
    nodes[p] = xc[q];
    weights[p] = we[q] * std::exp(expi[q]) / total;
    ++p;
  }
  if (p != n) return false;
  for (UnsignedInteger q = 0; q < n; ++q)
  {
    // Zero weights are accepted: extreme weights may legitimately underflow
    // to zero (same convention as the polished solver); NaN, infinite and
    // negative weights are rejected
    if (!std::isfinite(nodes[q])) return false;
    if (!std::isfinite(weights[q])) return false;
    if (!(weights[q] >= 0.0)) return false;
    if ((q > 0) && !(nodes[q] > nodes[q - 1]))
    {
      LOGDEBUG(OSS() << "Reject k=" << k << " n=" << n << " q=" << q << " xq=" << nodes[q] << " xqm=" << nodes[q - 1]);
      return false;
    }
  }
  return true;
}

/**
 * @namespace FastLaguerre
 *
 * Fast Gauss-Laguerre quadrature via the polished tridiagonal eigensolver
 * for small rules, and the Gil-Segura-Temme iterative path (Prufer
 * fixed-point sweeps in z = sqrt(x), Taylor stepping, continued-fraction
 * starts, scaled weights) for large rules, dispatched on the
 * FastLaguerre-IterativeThreshold ResourceMap key (default 8, minimum 5).
 *
 * The symmetric recurrence for orthonormal Laguerre polynomials with
 * weight x^{k-1} exp(-x) is:
 *   gamma_j = 2*j + k
 *   b_j = sqrt(j * (j + k - 1)),  b_0 = 0
 *
 * Reference: Golub, G. H. and Welsch, J. H. (1969).
 *            Calculation of Gauss Quadrature Rules.
 *            Mathematics of Computation, 23(106), 221-230.
 *
 * Reference: Gil, A., Segura, J. and Temme, N. M. (2019).
 *            Fast, reliable and unrestricted iterative computation of
 *            Gauss-Hermite and Gauss-Laguerre quadratures.
 *            Numer. Math. 143(3).
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

    // Large rules use the Gil-Segura-Temme iterative path, small ones the
    // polished eigensolver; threshold from the ResourceMap (minimum 5,
    // the iterative sweeps assume at least 3 found nodes).
    // Above the threshold the relative accuracy is better than 5e-13 and
    // the iterative path is faster.
    // A failed iterative sweep (incomplete node count or invalid rule)
    // falls back to the polished eigensolver.
    const UnsignedInteger iterativeThreshold = ResourceMap::GetAsUnsignedInteger("FastLaguerre-IterativeThreshold");
    if ((n >= iterativeThreshold) && (n >= 5))
    {
      Point trialNodes;
      Point trialWeights;
      if (ComputeNodesAndWeightsIterative(n, k, trialNodes, trialWeights))
      {
        std::copy(trialNodes.begin(), trialNodes.end(), nodes);
        std::copy(trialWeights.begin(), trialWeights.end(), weights);
        return;
      }
    }
    FastGaussQuadrature::PolishedSolve(gamma.data(), b.data(), n, nodes, weights);
  }
} // namespace FastLaguerre

END_NAMESPACE_OPENTURNS
