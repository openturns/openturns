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
#include <cmath>

#include "openturns/SpecFunc.hxx"
#include "openturns/Log.hxx"

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
    Scalar minModulus = 1.0;
    const Scalar expansion = AsymptoticExpansion(q1 - p1, 1.0 - p1, x, minModulus);
    // Beyond-all-orders Stokes contribution, of relative order
    // exp(-x) x^{2p-q} Gamma(p)/Gamma(q-p): the expansion is only accepted
    // when both the formal error and this contribution are negligible
    const Scalar logStokes = -x + (2.0 * p1 - q1) * std::log(x) + LogGamma(p1) - LogGamma(q1 - p1);
    if (std::isfinite(logPrefactor) && (expansion > 0.0) && (minModulus <= Precision) && (logStokes <= std::log(Precision)))
      return x + (p1 - q1) * std::log(x) + logPrefactor + std::log(expansion);
    return std::log(HyperGeom_1_1(p1, q1, x));
  }
  // Kummer transform, exact: 1F1(p;q;x) = exp(x) 1F1(q-p;q;-x), evaluated in
  // log-space through the positive-argument path above, free of overflow
  return x + LogHyperGeom_1_1(q1 - p1, q1, -x);
}

END_NAMESPACE_OPENTURNS
