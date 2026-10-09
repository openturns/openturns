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
#include "openturns/ResourceMap.hxx"
#include "openturns/SpecFunc.hxx"
#include <cmath>
#include <boost/math/special_functions/airy.hpp>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @namespace FastHermite details
 *
 * Asymptotic Townsend-Trogdon-Olver path (Airy expansion in theta,
 * Newton refinement, physicist convention mapped to probabilist).
 * Reimplemented from the equations of Townsend, A., Trogdon, T. and
 * Olver, S. (2016). Fast computation of Gauss quadrature nodes and
 * weights on the whole real line. IMA J. Numer. Anal. 36(2), 802-824.
 * Airy functions are used here directly through Boost (evaluation only);
 * they move to SpecFunc if the path is kept.
 */

    // Large rules use the Townsend-Trogdon-Olver Airy path, small ones the
    // polished eigensolver; threshold from the ResourceMap

// Exact first 10 Airy Ai roots (used by the Gatteschi initial guesses)
static const Scalar AiryRootsExact[10] = {-2.338107410459762, -4.087949444130970, -5.520559828095555, -6.786708090071765, -7.944133587120863, -9.022650853340979, -10.040174341558084, -11.008524303733260, -11.936015563236262, -12.828776752865757};

// Initial guesses: Gatteschi (Airy, outer nodes) patched with Tricomi
// (trigonometric, inner nodes) at 0.4985*m on the half rule; physicist
// scale, half rule (m ~ n/2, increasing inner to outer)
static void HermiteInitialGuesses(const UnsignedInteger n,
                                  Point & x0)
{
  const UnsignedInteger m = (n % 2 == 1) ? (n - 1) / 2 : n / 2;
  const Scalar a = (n % 2 == 1) ? 0.5 : -0.5;
  const Scalar nu = 4.0 * m + 2.0 * a + 2.0;
  Point airyrts(m);
  for (UnsignedInteger k = 1; k <= m; ++k)
  {
    const Scalar t = 3.0 / 8.0 * M_PI * (4.0 * k - 1.0);
    airyrts[k - 1] = -std::pow(t, 2.0 / 3.0) * (1.0 + 5.0 / 48.0 / (t * t) - 5.0 / 36.0 / (t * t * t * t) + (77125.0 / 82944.0) / (t * t * t * t * t * t) - 108056875.0 / 6967296.0 / std::pow(t, 8) + 162375596875.0 / 334430208.0 / std::pow(t, 10));
  }
  for (UnsignedInteger k = 0; k < std::min(m, static_cast<UnsignedInteger>(10)); ++k)
    airyrts[k] = AiryRootsExact[k];
  Point xAiry(m);
  for (UnsignedInteger k = 0; k < m; ++k)
  {
    const Scalar r = airyrts[k];
    const Scalar value = nu + std::pow(2.0, 2.0 / 3.0) * r * std::pow(nu, 1.0 / 3.0)
      + 1.0 / 5.0 * std::pow(2.0, 4.0 / 3.0) * r * r * std::pow(nu, -1.0 / 3.0)
      + (11.0 / 35.0 - a * a - 12.0 / 175.0 * r * r * r) / nu
      + (16.0 / 1575.0 * r + 92.0 / 7875.0 * r * r * r * r) * std::pow(2.0, 2.0 / 3.0) * std::pow(nu, -5.0 / 3.0)
      - (15152.0 / 3031875.0 * r * r * r * r * r + 1088.0 / 121275.0 * r * r) * std::pow(2.0, 1.0 / 3.0) * std::pow(nu, -7.0 / 3.0);
    xAiry[m - 1 - k] = (value > 0.0) ? std::sqrt(value) : 0.0;
  }
  Point xSin(m);
  {
    Point Tnk0(m, M_PI / 2.0);
    for (UnsignedInteger k = 0; k < m; ++k)
    {
      const Scalar rhs = (4.0 * m - 4.0 * (k + 1.0) + 3.0) / nu * M_PI;
      for (UnsignedInteger iter = 0; iter < 7; ++iter)
      {
        const Scalar val = Tnk0[k] - std::sin(Tnk0[k]) - rhs;
        const Scalar dval = 1.0 - std::cos(Tnk0[k]);
        Tnk0[k] -= val / dval;
      }
    }
    for (UnsignedInteger k = 0; k < m; ++k)
    {
      const Scalar tnk = std::pow(std::cos(Tnk0[k] / 2.0), 2);
      xSin[k] = std::sqrt(nu * tnk - (5.0 / (4.0 * (1.0 - tnk) * (1.0 - tnk)) - 1.0 / (1.0 - tnk) - 1.0 + 3.0 * a * a) / 3.0 / nu);
    }
  }
  const UnsignedInteger cut = static_cast<UnsignedInteger>(0.4985 * m);
  x0 = Point(m);
  for (UnsignedInteger k = 0; k < m; ++k)
    x0[k] = (k < cut) ? xSin[k] : xAiry[k];
  if (n % 2 == 1)
  {
    // Odd rule: prepend the exact center node, Newton keeps it in place
    Point y0(m + 1, 0.0);
    for (UnsignedInteger k = 0; k < m; ++k)
      y0[k + 1] = x0[k];
    x0 = y0;
  }
}

// Hermite polynomial and scaled derivative by the Airy expansion in theta
static void EvaluateAiryAsymptotics(const UnsignedInteger n,
                                    const Point & theta,
                                    Point & vals,
                                    Point & ders)
{
  const UnsignedInteger nt = theta.getSize();
  const Scalar musq = 2.0 * n + 1.0;
  vals = Point(nt);
  ders = Point(nt);
  for (UnsignedInteger i = 0; i < nt; ++i)
  {
    const Scalar t = theta[i];
    const Scalar cosT = std::cos(t);
    const Scalar sinT = std::sin(t);
    const Scalar eta = 0.5 * t - 0.25 * 2.0 * cosT * sinT;
    const Scalar chi = -std::pow(3.0 * eta / 2.0, 2.0 / 3.0);
    const Scalar phi = std::pow(-chi / (sinT * sinT), 0.25);
    const Scalar arg = std::pow(musq, 2.0 / 3.0) * chi;
    const Scalar Airy0 = boost::math::airy_ai(arg);
    const Scalar Airy1 = boost::math::airy_ai_prime(arg);
    const Scalar a0 = 1.0;
    const Scalar b0 = 1.0;
    const Scalar a1 = 15.0 / 144.0;
    const Scalar b1 = -7.0 / 5.0 * a1;
    const Scalar a2 = 5.0 * 7.0 * 9.0 * 11.0 / 2.0 / 144.0 / 144.0;
    const Scalar b2 = -13.0 / 11.0 * a2;
    const Scalar a3 = 7.0 * 9.0 * 11.0 * 13.0 * 15.0 * 17.0 / 6.0 / 144.0 / 144.0 / 144.0;
    const Scalar b3 = -19.0 / 17.0 * a3;
    const Scalar u0 = 1.0;
    const Scalar u1 = (cosT * cosT * cosT - 6.0 * cosT) / 24.0;
    const Scalar u2 = (-9.0 * std::pow(cosT, 4) + 249.0 * cosT * cosT + 145.0) / 1152.0;
    const Scalar u3 = (-4042.0 * std::pow(cosT, 9) + 18189.0 * std::pow(cosT, 7) - 28287.0 * std::pow(cosT, 5) - 151995.0 * std::pow(cosT, 3) - 259290.0 * cosT) / 414720.0;
    const Scalar constF = 2.0 * std::sqrt(M_PI) * std::pow(musq, 1.0 / 6.0) * phi;
    Scalar val = Airy0;
    const Scalar B0 = -(a0 * std::pow(phi, 6) * u1 + a1 * u0) / (chi * chi);
    val += B0 * Airy1 / std::pow(musq, 4.0 / 3.0);
    const Scalar A1 = (b0 * std::pow(phi, 12) * u2 + b1 * std::pow(phi, 6) * u1 + b2 * u0) / (chi * chi * chi);
    val += A1 * Airy0 / (musq * musq);
    const Scalar B1 = -(std::pow(phi, 18) * u3 + a1 * std::pow(phi, 12) * u2 + a2 * std::pow(phi, 6) * u1 + a3 * u0) / std::pow(chi, 5);
    val += B1 * Airy1 / std::pow(musq, 4.0 / 3.0 + 2.0);
    vals[i] = constF * val;
    const Scalar constD = std::sqrt(2.0 * M_PI) * std::pow(musq, 1.0 / 3.0) / phi;
    const Scalar v0 = 1.0;
    const Scalar v1 = (cosT * cosT * cosT + 6.0 * cosT) / 24.0;
    const Scalar v2 = (15.0 * std::pow(cosT, 4) - 327.0 * cosT * cosT - 143.0) / 1152.0;
    const Scalar v3 = (259290.0 * cosT + 238425.0 * std::pow(cosT, 3) - 36387.0 * std::pow(cosT, 5) + 18189.0 * std::pow(cosT, 7) - 4042.0 * std::pow(cosT, 9)) / 414720.0;
    const Scalar C0 = -(b0 * std::pow(phi, 6) * v1 + b1 * v0) / chi;
    Scalar dval = C0 * Airy0 / std::pow(musq, 2.0 / 3.0);
    const Scalar D0 = a0 * v0;
    dval += D0 * Airy1;
    const Scalar C1 = -(std::pow(phi, 18) * v3 + b1 * std::pow(phi, 12) * v2 + b2 * std::pow(phi, 6) * v1 + b3 * v0) / std::pow(chi, 4);
    dval += C1 * Airy0 / std::pow(musq, 2.0 / 3.0 + 2.0);
    const Scalar D1 = (a0 * std::pow(phi, 12) * v2 + a1 * std::pow(phi, 6) * v1 + a2 * v0) / (chi * chi * chi);
    dval += D1 * Airy1 / (musq * musq);
    ders[i] = constD * dval;
  }
}

// Full asymptotic rule in physicist convention, mapped to probabilist.
// Returns false on any structural defect so the caller falls back to the
// polished eigensolver, mirroring the Laguerre/Jacobi paths.
static Bool ComputeNodesAndWeightsAsymptotic(const UnsignedInteger n,
                                             Scalar * nodes,
                                             Scalar * weights)
{
  Point x0;
  HermiteInitialGuesses(n, x0);
  const UnsignedInteger m = x0.getSize();
  Point theta(m);
  for (UnsignedInteger k = 0; k < m; ++k)
  {
    const Scalar t0 = x0[k] / std::sqrt(2.0 * n + 1.0);
    if (!std::isfinite(t0)) return false;
    theta[k] = std::acos(std::max(-1.0, std::min(1.0, t0)));
  }
  Point vals(m);
  Point ders(m);
  for (UnsignedInteger iter = 0; iter < 20; ++iter)
  {
    EvaluateAiryAsymptotics(n, theta, vals, ders);
    Scalar worst = 0.0;
    for (UnsignedInteger k = 0; k < m; ++k)
    {
      const Scalar denom = std::sqrt(2.0) * std::sqrt(2.0 * n + 1.0) * ders[k] * std::sin(theta[k]);
      if (!(std::abs(denom) > 0.0)) return false;
      const Scalar dt = -vals[k] / denom;
      if (!std::isfinite(dt)) return false;
      theta[k] -= dt;
      worst = std::max(worst, std::abs(dt));
    }
    if (worst <= std::sqrt(SpecFunc::ScalarEpsilon) / 100.0) break;
  }
  Point x(m);
  for (UnsignedInteger k = 0; k < m; ++k)
    x[k] = std::sqrt(2.0 * n + 1.0) * std::cos(theta[k]);
  EvaluateAiryAsymptotics(n, theta, vals, ders);
  Point w(m);
  for (UnsignedInteger k = 0; k < m; ++k)
  {
    const Scalar dd = x[k] * vals[k] + std::sqrt(2.0) * ders[k];
    w[k] = std::exp(-x[k] * x[k]) / (dd * dd);
  }
  // Map physicist to probabilist (z = x*sqrt(2), w/sqrt(pi)), fold, normalize
  Point z(n);
  Point ww(n);
  if (n % 2 == 1)
  {
    // Mirror of the prototype fold [-x[::-1], x[1:]]: x holds h+1 entries
    // with x[0] the center, h = (n-1)/2; center kept once as -x[0]
    const UnsignedInteger h = m - 1;
    for (UnsignedInteger k = 0; k <= h; ++k)
    {
      z[k] = -x[h - k] * std::sqrt(2.0);
      ww[k] = w[h - k];
    }
    for (UnsignedInteger k = 1; k <= h; ++k)
    {
      z[h + k] = x[k] * std::sqrt(2.0);
      ww[h + k] = w[k];
    }
  }
  else
  {
    for (UnsignedInteger k = 0; k < m; ++k)
    {
      z[m - 1 - k] = -x[k] * std::sqrt(2.0);
      ww[m - 1 - k] = w[k];
      z[m + k] = x[k] * std::sqrt(2.0);
      ww[m + k] = w[k];
    }
  }
  // Physicist to probabilist is a uniform weight scale (1/sqrt(pi)),
  // absorbed by the sum-to-one normalization below
  Scalar total = 0.0;
  for (UnsignedInteger k = 0; k < n; ++k)
    total += ww[k];
  if (!(total > 0.0)) return false;
  // Normalize to sum one (probability convention)
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    nodes[k] = z[k];
    weights[k] = ww[k] / total;
  }
  for (UnsignedInteger k = 0; k < n; ++k)
  {
    if (!std::isfinite(nodes[k])) return false;
    if (!std::isfinite(weights[k])) return false;
    if (!(weights[k] >= 0.0)) return false;
    if ((k > 0) && !(nodes[k] > nodes[k - 1])) return false;
  }
  return true;
}

/**
 * @namespace FastHermite
 *
 * Fast Gauss-Hermite quadrature for the standard probabilist's Hermite
 * polynomials, orthonormal w.r.t. N(0,1), via the polished tridiagonal
 * eigensolver for small rules and the Townsend-Trogdon-Olver Airy
 * expansion for large rules (threshold HermiteAsymptoticThreshold = 256).
 *
 * The symmetric recurrence is: x * p_j = sqrt(j+1) * p_{j+1} + 0 * p_j + sqrt(j) * p_{j-1}
 * So: gamma_j = 0, b_j = sqrt(j), b_0 = 0.
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

    // Large rules use the Townsend-Trogdon-Olver Airy path, small ones the
    // polished eigensolver; threshold from the ResourceMap.
    // Above the threshold the relative accuracy is better than 5e-13 and
    // the asymptotic path is faster. A failed asymptotic evaluation falls
    // back to the polished eigensolver.
    const UnsignedInteger asymptoticThreshold = ResourceMap::GetAsUnsignedInteger("FastHermite-AsymptoticThreshold");
    if (n >= asymptoticThreshold)
    {
      if (ComputeNodesAndWeightsAsymptotic(n, nodes, weights)) return;
      // fall through to the polished eigensolver on asymptotic failure
    }
    FastGaussQuadrature::PolishedSolve(gamma.data(), b.data(), n, nodes, weights);
  }
} // namespace FastHermite

END_NAMESPACE_OPENTURNS
