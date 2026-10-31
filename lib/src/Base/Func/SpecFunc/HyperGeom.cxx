//                                               -*- C++ -*-
/**
 *  @brief Hypergeometric functions and related functions
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
#include <algorithm>
#include <cmath>
#include <limits>

#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/ResourceMap.hxx"

#ifdef OPENTURNS_HAVE_BOOST

#ifdef OPENTURNS_HAVE_MPC
#include <boost/multiprecision/mpc.hpp>
#endif

#ifdef OPENTURNS_HAVE_MPFR
#include <boost/multiprecision/mpfr.hpp>
#endif

#endif

BEGIN_NAMESPACE_OPENTURNS

// Hypergeometric functions (moved from SpecFunc.cxx)
// Hypergeometric function of type (1,1): HyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
Scalar SpecFunc::HyperGeom_1_1(const Scalar p1,
                               const Scalar q1,
                               const Scalar x)
{
  if (q1 == p1) return std::exp(x);
  if (x == 0) return 1.0;
#ifdef OPENTURNS_HAVE_MPFR
  boost::multiprecision::mpfr_float_500 pochhammerP1(p1);
  boost::multiprecision::mpfr_float_500 pochhammerQ1(q1);
  boost::multiprecision::mpfr_float_500 factorial(1.0);
  boost::multiprecision::mpfr_float_500 term(1.0);
  boost::multiprecision::mpfr_float_500 sum(term);
  boost::multiprecision::mpfr_float_500 eps(1.0);
  boost::multiprecision::mpfr_float_500 z(x);
  Bool absEps = abs(eps) > Precision;
  Bool absEpsPrec = absEps;
  do
  {
    absEpsPrec = absEps;
    term *= pochhammerP1 * z / (pochhammerQ1 * factorial);
    pochhammerP1 += 1.0;
    pochhammerQ1 += 1.0;
    factorial += 1.0;
    sum += term;
    eps = term / sum;
    absEps = abs(eps) > Precision;
  }
  while (absEps || absEpsPrec);
  return sum.convert_to<Scalar>();

#else
  Scalar term = 1.0;
  Scalar t = x;
  Scalar pochhammerP1 = p1;
  if (x < 0)
  {
    pochhammerP1 = q1 - p1;
    t = -x;
    term = std::exp(x);
  }
  Scalar pochhammerQ1 = q1;
  Scalar factorial = 1.0;
  Scalar sum = term;
  Scalar eps = 1.0;
  Bool absEps = std::abs(eps) > Precision;
  Bool absEpsPrec = absEps;
  do
  {
    absEpsPrec = absEps;
    term *= pochhammerP1 * t / (pochhammerQ1 * factorial);
    ++pochhammerP1;
    ++pochhammerQ1;
    ++factorial;
    sum += term;
    eps = term / sum;
    absEps = std::abs(eps) > Precision;
  }
  while (absEps || absEpsPrec);
  return sum;
#endif
}


// Complex hypergeometric function of type (1,1): HyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
Complex SpecFunc::HyperGeom_1_1(const Scalar p1,
                                const Scalar q1,
                                const Complex & x)
{
#ifdef OPENTURNS_HAVE_MPC
  LOGDEBUG("Use MPC implementation");
  boost::multiprecision::mpc_complex_500 pochhammerP1(p1);
  boost::multiprecision::mpc_complex_500 pochhammerQ1(q1);
  boost::multiprecision::mpc_complex_500 factorial{1.0, 0.0};
  boost::multiprecision::mpc_complex_500 term{1.0, 0.0};
  boost::multiprecision::mpc_complex_500 sum(term);
  boost::multiprecision::mpc_complex_500 eps{1.0, 0.0};
  boost::multiprecision::mpc_complex_500 z(x);
  Bool absEps = abs(eps) > Precision;
  Bool absEpsPrec = absEps;
  do
  {
    absEpsPrec = absEps;
    term *= pochhammerP1 * z / (pochhammerQ1 * factorial);
    pochhammerP1 += 1.0;
    pochhammerQ1 += 1.0;
    factorial += 1.0;
    sum += term;
    eps = term / sum;
    absEps = abs(eps) > Precision;
  }
  while (absEps || absEpsPrec);
  return sum.convert_to<Complex>();
#else
  Complex pochhammerP1(p1);
  Complex pochhammerQ1(q1);
  Scalar factorial = 1.0;
  Complex term(1.0);
  Complex sum(term);
  Complex eps(1.0);
  Bool absEps = std::abs(eps) > Precision;
  Bool absEpsPrec = absEps;
  do
  {
    absEpsPrec = absEps;
    term *= pochhammerP1 * x / (pochhammerQ1 * factorial);
    pochhammerP1 += 1.0;
    pochhammerQ1 += 1.0;
    ++factorial;
    sum += term;
    eps = term / sum;
    absEps = std::abs(eps) > Precision;
  }
  while (absEps || absEpsPrec);
  return sum;
#endif
}

// Hypergeometric function of type (2,1): HyperGeom_2_1(p1, p2, q1, x) = sum_{n=0}^{\infty} [prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k)] * x^n / n!
Scalar SpecFunc::HyperGeom_2_1(const Scalar p1,
                               const Scalar p2,
                               const Scalar q1,
                               const Scalar x)
{
#ifdef OPENTURNS_HAVE_MPFR
  boost::multiprecision::mpfr_float_500 pochhammerP1(p1);
  boost::multiprecision::mpfr_float_500 pochhammerP2(p2);
  boost::multiprecision::mpfr_float_500 pochhammerQ1(q1);
  boost::multiprecision::mpfr_float_500 factorial(1.0);
  boost::multiprecision::mpfr_float_500 z(x);
  boost::multiprecision::mpfr_float_500 term(1.0);
  boost::multiprecision::mpfr_float_500 sum(term);
  boost::multiprecision::mpfr_float_500 eps(1.0);
  Bool absEps = abs(eps) > Precision;
  Bool absEpsPrec = absEps;
  do
  {
    absEpsPrec = absEps;
    term *= pochhammerP1 * pochhammerP2 * z / (pochhammerQ1 * factorial);
    ++pochhammerP1;
    ++pochhammerP2;
    ++pochhammerQ1;
    ++factorial;
    sum += term;
    eps = abs(term / sum);
    absEps = abs(eps) > Precision;
  }
  while (absEps || absEpsPrec);
  return sum.convert_to<Scalar>();
#else
  Scalar pochhammerP1 = p1;
  Scalar pochhammerP2 = p2;
  Scalar pochhammerQ1 = q1;
  Scalar factorial = 1.0;
  Scalar term = 1.0;
  Scalar sum = term;
  Scalar eps = 1.0;
  Bool absEps = std::abs(eps) > Precision;
  Bool absEpsPrec = absEps;
  do
  {
    absEpsPrec = absEps;
    term *= pochhammerP1 * pochhammerP2 * x / (pochhammerQ1 * factorial);
    ++pochhammerP1;
    ++pochhammerP2;
    ++pochhammerQ1;
    ++factorial;
    sum += term;
    eps = std::abs(term / sum);
    absEps = std::abs(eps) > Precision;
  }
  while (absEps || absEpsPrec);
  return sum;
#endif
}

// Hypergeometric function of type (2,2): HyperGeom_2_1(p1, p2, q1, q2, x) = sum_{n=0}^{\infty} [prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k) / (q2 + k)] * x^n / n!
Scalar SpecFunc::HyperGeom_2_2(const Scalar p1,
                               const Scalar p2,
                               const Scalar q1,
                               const Scalar q2,
                               const Scalar x)
{
  if (x == 0.0) return 1.0;
#ifdef OPENTURNS_HAVE_MPFR
  boost::multiprecision::mpfr_float_500 pochhammerP1(p1);
  boost::multiprecision::mpfr_float_500 pochhammerP2(p2);
  boost::multiprecision::mpfr_float_500 pochhammerQ1(q1);
  boost::multiprecision::mpfr_float_500 pochhammerQ2(q2);
  boost::multiprecision::mpfr_float_500 factorial(1.0);
  boost::multiprecision::mpfr_float_500 z(x);
  boost::multiprecision::mpfr_float_500 term(0.0);
  boost::multiprecision::mpfr_float_500 sum(term);
  boost::multiprecision::mpfr_float_500 eps(1.0);
  Bool absEps = abs(eps) > Precision;
  Bool absEpsPrec = absEps;
  do
  {
    absEpsPrec = absEps;
    term += pochhammerP1 * pochhammerP2 * z / (pochhammerQ1 * pochhammerQ2 * factorial);
    ++pochhammerP1;
    ++pochhammerP2;
    ++pochhammerQ1;
    ++pochhammerQ2;
    ++factorial;
    sum += term;
    eps = abs(term / sum);
    absEps = abs(eps) > Precision;
  }
  while (absEps || absEpsPrec);
  return sum.convert_to<Scalar>();
#else
  Scalar pochhammerP1 = p1;
  Scalar pochhammerP2 = p2;
  Scalar pochhammerQ1 = q1;
  Scalar pochhammerQ2 = q2;
  Scalar factorial = 1.0;
  Scalar term = 0.0;
  Scalar sum = term;
  Scalar eps = 1.0;
  Bool absEps = std::abs(eps) > Precision;
  Bool absEpsPrec = absEps;
  const Scalar logX = std::log(std::abs(x));
  Scalar signX = x > 0.0 ? 1.0 : -1.0;
  Scalar signTerm = 1.0;
  do
  {
    absEpsPrec = absEps;
    term += std::log(pochhammerP1) + std::log(pochhammerP2) + logX - std::log(pochhammerQ1) - std::log(pochhammerQ2) - std::log(factorial);
    ++pochhammerP1;
    ++pochhammerP2;
    ++pochhammerQ1;
    ++pochhammerQ2;
    ++factorial;
    sum += signTerm * std::exp(term);
    signTerm *= signX;
    eps = std::abs(term / sum);
    absEps = std::abs(eps) > Precision;
  }
  while (absEps || absEpsPrec);
  return sum;
#endif
}

namespace {

/* Normalize a vector by its largest component and update a log-scale. */
Bool Normalize(Point & values,
               Scalar & logScale)
{
  Scalar maximum = 0.0;
  for (const Scalar value : values) maximum = std::max(maximum, std::abs(value));
  if (!(maximum > 0.0) || !std::isfinite(maximum)) return false;
  for (Scalar & value : values) value /= maximum;
  logScale += std::log(maximum);
  return true;
}

} /* anonymous namespace */

// Lauricella hypergeometric function F_A, evaluated through an exponential
// generating function to avoid computing large multinomial coefficients.
// Returns the natural logarithm of its absolute value together with its sign
// and an estimate of the cancellation level. Returns false when the series
// cannot be evaluated.
Bool SpecFunc::LogLauricellaF_A(const Scalar a,
                                const Point & b,
                                const Point & c,
                                const Point & x,
                                Scalar & logValue,
                                Scalar & cancellation,
                                Scalar & sign)
{
  const UnsignedInteger n = x.getSize();
  if ((b.getSize() != n) || (c.getSize() != n))
    throw InvalidArgumentException(HERE) << "Error: expected b, c and x of the same dimension, got b dimension=" << b.getSize() << ", c dimension=" << c.getSize() << " and x dimension=" << n;
  if (n == 0)
  {
    logValue = 0.0;
    cancellation = 1.0;
    sign = 1.0;
    return true;
  }

  Scalar sumAbsX = 0.0;
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    // Also rejects NaN.
    if (!(std::abs(x[i]) < SpecFunc::Infinity)) return false;
    if (!(c[i] > 0.0)) return false;
    sumAbsX += std::abs(x[i]);
  }
  // The series converges for sum_i |x_i| < 1.
  if (!(sumAbsX < 1.0)) return false;

  if (a == 0.0)
  {
    logValue = 0.0;
    cancellation = 1.0;
    sign = 1.0;
    return true;
  }

  const Scalar tolerance = ResourceMap::GetAsScalar("SpecFunc-LauricellaTolerance");
  // Also rejects NaN.
  if (!((tolerance > 0.0) && (tolerance < 1.0)))
    throw InvalidArgumentException(HERE) << "Error: SpecFunc-LauricellaTolerance must be in (0, 1), here tolerance=" << tolerance;
  /* The convolution is done in O(n degree) operations, but the number of
     non-zero coefficients still grows with the dimension. From eight
     variables, halve the maximum degree to keep the cost bounded. */
  UnsignedInteger maxDegree = ResourceMap::GetAsUnsignedInteger("SpecFunc-LauricellaMaximumDegree");
  if (n >= 8) maxDegree = std::max<UnsignedInteger>(64, maxDegree / 2);
  /* The series terms decay like |x|^q for a > 0, requiring about
     -log(tolerance) / (1 - sum|x|) terms; the factor 4 and the minimum of
     64 provide a safety margin. For a <= 0 the binomial factor (a)_q / q!
     grows like q^(-a-1), so the required degree increases with |a|. */
  /* Floor the convergence gap to avoid an unbounded degree estimate when
     sum|x| is within roundoff of the convergence radius. */
  const Scalar gap = std::max(1.0 - sumAbsX, 1.0e-16);
  const Scalar maximumEstimate = std::min(static_cast<Scalar>(maxDegree), static_cast<Scalar>(std::numeric_limits<UnsignedInteger>::max() / 2));
  Scalar estimate = 64.0;
  if (a > 0.0)
    estimate = std::max(64.0, 4.0 * (-std::log(tolerance)) / gap);
  else
    estimate = std::max(64.0, 2.0 * (std::abs(a) + 10.0) / gap);
  /* Clamp before the cast: a tiny tolerance or a tiny gap can push the
     estimate beyond the UnsignedInteger range. */
  UnsignedInteger degree = std::min(maxDegree, static_cast<UnsignedInteger>(std::min(maximumEstimate, estimate)));

  while (true)
  {
    Point d(degree + 1, 0.0);
    Point factor(degree + 1, 0.0);
    Point next(degree + 1, 0.0);
    d[0] = 1.0;
    Scalar logScale = 0.0;
    Bool underflow = false;
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      factor[0] = 1.0;
      for (UnsignedInteger k = 1; k <= degree; ++k)
        factor[k] = factor[k - 1] * x[i] * (b[i] + k - 1.0) / (c[i] + k - 1.0);
      std::fill(next.begin(), next.end(), 0.0);
      for (UnsignedInteger q = 0; q <= degree; ++q)
      {
        Scalar sum = 0.0;
        Scalar binomial = 1.0;
        for (UnsignedInteger j = 0; j <= q; ++j)
        {
          sum += binomial * d[j] * factor[q - j];
          if (j < q) binomial *= static_cast<Scalar>(q - j) / static_cast<Scalar>(j + 1);
        }
        next[q] = sum;
      }
      if (!Normalize(next, logScale))
      {
        underflow = true;
        break;
      }
      std::swap(d, next);
    }
    if (underflow) return false;

    Scalar total = 0.0;
    Scalar maxAbs = 0.0;
    Scalar ratio = 1.0;
    Bool converged = false;
    for (UnsignedInteger q = 0; q <= degree; ++q)
    {
      if (q > 0) ratio *= (a + static_cast<Scalar>(q) - 1.0) / static_cast<Scalar>(q);
      const Scalar term = ratio * d[q];
      total += term;
      maxAbs = std::max(maxAbs, std::abs(term));
      /* Do not test convergence before 20 terms, where the alternating
         part of the series may still produce an accidentally small term. */
      if ((q > 20) && (std::abs(term) <= tolerance * std::max(std::abs(total), SpecFunc::MinScalar)))
      {
        converged = true;
        break;
      }
    }
    if (!std::isfinite(total) || (total == 0.0))
    {
      if (degree >= maxDegree) return false;
      degree = std::min(maxDegree, 2 * degree);
      continue;
    }
    cancellation = maxAbs / std::abs(total);
    logValue = logScale + std::log(std::abs(total));
    sign = (total < 0.0 ? -1.0 : 1.0);
    /* A series stopped at its degree limit without meeting the tolerance
       has not produced a reliable value: report the failure so that the
       caller can use its fallback instead of accepting a biased result. */
    if (converged) return true;
    if (degree >= maxDegree) return false;
    degree = std::min(maxDegree, 2 * degree);
  }
}

// Lauricella hypergeometric function F_A:
// F_A(a; b_1, ..., b_n; c_1, ..., c_n; x_1, ..., x_n) =
// sum_{m_1, ..., m_n >= 0} (a)_{m_1 + ... + m_n}
//   prod_i (b_i)_{m_i} / (c_i)_{m_i} x_i^{m_i} / m_i!
Scalar SpecFunc::LauricellaF_A(const Scalar a,
                               const Point & b,
                               const Point & c,
                               const Point & x)
{
  Scalar logValue = 0.0;
  Scalar cancellation = 1.0;
  Scalar sign = 1.0;
  if (!LogLauricellaF_A(a, b, c, x, logValue, cancellation, sign)) return std::numeric_limits<Scalar>::quiet_NaN();
  if (logValue > SpecFunc::LogMaxScalar) return sign * SpecFunc::MaxScalar;
  if (logValue < SpecFunc::LogMinScalar) return 0.0;
  return sign * std::exp(logValue);
}

namespace {

// Optimally-truncated asymptotic expansion sum_{k>=0} c_k with c_0 = 1 and
// c_k = c_{k-1} (rising1 + k - 1) (rising2 + k - 1) / (k t). The loop stops
// when the terms start growing (optimal truncation) or when they become
// negligible with a fast-decreasing tail. minModulus returns the smallest
// term encountered, which estimates the formal truncation error; it does
// not account for exponentially-small Stokes contributions, checked by
// the caller.
Scalar AsymptoticExpansion(const Scalar rising1,
                           const Scalar rising2,
                           const Scalar t,
                           Scalar & minModulus)
{
  Scalar term = 1.0;
  Scalar sum = term;
  Scalar previousModulus = 1.0;
  minModulus = 1.0;
  for (UnsignedInteger k = 1; ; ++k)
  {
    term *= (rising1 + static_cast<Scalar>(k) - 1.0) * (rising2 + static_cast<Scalar>(k) - 1.0) / (static_cast<Scalar>(k) * t);
    const Scalar modulus = std::abs(term);
    if (!std::isfinite(modulus))
    {
      // Overflow of the expansion coefficients: not usable
      break;
    }
    if (modulus == 0.0)
    {
      // Exactly vanishing term: the formal series terminates here
      minModulus = 0.0;
      break;
    }
    if (modulus > previousModulus)
    {
      // Optimal truncation reached
      break;
    }
    const Scalar decreaseRatio = modulus / previousModulus;
    previousModulus = modulus;
    minModulus = std::min(minModulus, modulus);
    sum += term;
    if ((modulus <= SpecFunc::Precision) && (decreaseRatio <= 0.5))
    {
      // Negligible term with a fast-decreasing tail
      break;
    }
  }
  return sum;
}

} // anonymous namespace

// Natural logarithm of the hypergeometric function of type (1,1). For large
// |x| the DLMF 13.7.1 asymptotic expansions are used, optimally truncated:
// 1F1(p;q;x) ~ Gamma(q)/Gamma(p) exp(x) x^{p-q} sum_{k>=0} (q-p)_k (1-p)_k / (k! x^k)
// for x -> +infinity and
// 1F1(p;q;x) ~ Gamma(q)/Gamma(q-p) (-x)^{-p} sum_{k>=0} (p)_k (p-q+1)_k / (k! x^k)
// for x -> -infinity. Otherwise the value falls back to the logarithm of the
// direct evaluation, which is overflow-free in the remaining range.
Scalar SpecFunc::LogHyperGeom_1_1(const Scalar p1,
                                  const Scalar q1,
                                  const Scalar x)
{
  if (x == 0.0) return 0.0;
  if (q1 == p1) return x;
  if (x > 0.0)
  {
    const Scalar logPrefactor = LogGamma(q1) - LogGamma(p1);
    // lgamma returns log|Gamma|: the asymptotic form is only valid in
    // log-space when Gamma(q1) / Gamma(p1) > 0
    const Bool positivePrefactor = (std::tgamma(q1) > 0.0) == (std::tgamma(p1) > 0.0);
    Scalar minModulus = 1.0;
    const Scalar expansion = AsymptoticExpansion(q1 - p1, 1.0 - p1, x, minModulus);
    // Beyond-all-orders Stokes contribution, of relative order
    // exp(-x) x^{2p-q} Gamma(p)/Gamma(q-p): the expansion is only accepted
    // when both the formal error and this contribution are negligible
    const Scalar logStokes = -x + (2.0 * p1 - q1) * std::log(x) + LogGamma(p1) - LogGamma(q1 - p1);
    if (positivePrefactor && std::isfinite(logPrefactor) && (expansion > 0.0) && (minModulus <= Precision) && (logStokes <= std::log(Precision)))
      return x + (p1 - q1) * std::log(x) + logPrefactor + std::log(expansion);
    const Scalar value = HyperGeom_1_1(p1, q1, x);
    if (!(value > 0.0)) throw InvalidArgumentException(HERE) << "Error: LogHyperGeom_1_1 requires a positive 1F1 value, got 1F1(" << p1 << ";" << q1 << ";" << x << ")=" << value;
    return std::log(value);
  }
  // Kummer transform, exact: 1F1(p;q;x) = exp(x) 1F1(q-p;q;-x), evaluated in
  // log-space through the positive-argument path above, free of overflow
  return x + LogHyperGeom_1_1(q1 - p1, q1, -x);
}

END_NAMESPACE_OPENTURNS
