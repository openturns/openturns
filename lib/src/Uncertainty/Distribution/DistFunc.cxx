//                                               -*- C++ -*-
/**
 *  @brief OpenTURNS wrapper to a library of special functions
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iomanip>

#include "openturns/DistFunc.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/RandomGenerator.hxx"
#include "openturns/Log.hxx"
#include "openturns/KFactorFunctions.hxx"
#include "openturns/Normal2DCDF.hxx"
#include "openturns/Normal3DCDF.hxx"
#include "openturns/Exception.hxx"

#ifdef OPENTURNS_HAVE_BOOST

#include <boost/math/distributions/hypergeometric.hpp>

#endif

// The following implementation of the Kolmogorov CDF and tail CDF is used in a LGPL context with written permission of the author.
#include "KolmogorovSmirnovDist.h"
// The following implementation of the Poisson quantile is used in a LGPL context with written permission of the author.
#include "poissinv_cpu.h"
#include "openturns/StudentFunctions.hxx"
#include "openturns/OTconfig.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**************************************************************************************************************************/
/* Normalized Beta distribution, i.e. with a PDF equals to x ^ (p1 - 1) . (1 - x) ^ (p2 - 1) / Beta(p1, p2) . (0 < x < 1) */
/**************************************************************************************************************************/
/* CDF */
Scalar DistFunc::pBeta(const Scalar p1,
                       const Scalar p2,
                       const Scalar x,
                       const Bool tail)
{
  return SpecFunc::RegularizedIncompleteBeta(p1, p2, x, tail);
}
/* CDF Inverse */
Scalar DistFunc::qBeta(const Scalar p1,
                       const Scalar p2,
                       const Scalar p,
                       const Bool tail)
{
  return SpecFunc::RegularizedIncompleteBetaInverse(p1, p2, p, tail);
}
/* Random number generation
   We use the algorithm of Cheng (1978), Johnk, Atkinson and Whittaker (1979) 1 & 2 described in:
   Luc Devroye, "Non-Uniform RandomVariate Generation", Springer-Verlag, 1986, available online at:
   http://cg.scs.carleton.ca/~luc/nonuniformrandomvariates.zip
   and with the important errata at:
   http://cg.scs.carleton.ca/~luc/errors.pdf
*/
Scalar DistFunc::rBeta(const Scalar p1,
                       const Scalar p2)
{
  // Strategy:
  // If (a = 1 and b = 1), Beta(1,1) = Uniform(0,1)
  // If (a = 1 or b = 1), analytic cases
  // If (a + b <= 1), Johnk
  // If (a + b > 1):
  // If (a < 1 and b < 1), Atkinson and Whittaker 1
  // If (a < 1 and b > 1) or (a > 1 and b < 1), Atkinson and Whittaker 2
  // If (a > 1 and b > 1) Cheng.
  if ((p1 == 1.0) && (p2 == 1.0)) return RandomGenerator::Generate();
  // Special case for p1 = 1 or p2 = 1
  if (p1 == 1.0) return 1.0 - std::pow(RandomGenerator::Generate(), 1.0 / p2);
  if (p2 == 1.0) return std::pow(RandomGenerator::Generate(), 1.0 / p1);
  // Now, the more general cases
  const Scalar minp = std::min(p1, p2);
  const Scalar maxp = std::max(p1, p2);
  const Scalar sum = p1 + p2;
  if (sum <= 1.0) // Johnk
  {
    // Use of logarithms to avoid underflow if minp << 1 (here 1e-3, quite arbitrary). It is the
    // only case where this kind of trick is useful. It should not be much slower than the usual
    // version using std::pow()
    if (minp < 1e-3)
    {
      for (;;)
      {
        const Scalar u = RandomGenerator::Generate();
        const Scalar v = RandomGenerator::Generate();
        const Scalar logx = std::log(u) / p1;
        const Scalar logy = std::log(v) / p2;
        const Scalar logsum = (logx > logy) ? logx + log1p(std::exp(logy - logx)) : logy + log1p(std::exp(logx - logy));
        // Acceptation step
        if (logsum <= 0.0) return std::exp(logy - logsum);
      }
    }
    // Usual form of the algorithm
    for (;;)
    {
      const Scalar u = RandomGenerator::Generate();
      const Scalar v = RandomGenerator::Generate();
      const Scalar x = std::pow(u, 1.0 / p1);
      const Scalar y = std::pow(v, 1.0 / p2);
      // Acceptation step
      if (x + y <= 1.0) return x / (x + y);
    }
  } // End Johnk
  // Now, sum > 1 for all the remaining cases
  if (minp > 1.0) // Cheng
  {
    const Scalar lambda = std::sqrt((sum - 2.0) / (2.0 * p1 * p2 - sum));
    const Scalar c = minp + 1.0 / lambda;
    for (;;)
    {
      const Scalar u1 = RandomGenerator::Generate();
      const Scalar u2 = RandomGenerator::Generate();
      const Scalar v = lambda * std::log(u1 / (1.0 - u1));
      const Scalar w = minp * std::exp(v);
      const Scalar z = u1 * u1 * u2;
      // 1.386294361119890618834464 = log(4)
      const Scalar r = c * v - 1.386294361119890618834464;
      const Scalar s = minp + r - w;
      // Quick acceptance steps
      // 2.609437912434100374600759 = 1 + log(5)
      if (s + 2.609437912434100374600759 >= 5.0 * z) return (p1 == minp) ? w / (maxp + w) : maxp / (maxp + w);
      const Scalar t = std::log(z);
      if (s > t) return (p1 == minp) ? w / (maxp + w) : maxp / (maxp + w);
      // Acceptance step
      if (r + sum * std::log(sum / (maxp + w)) >= t) return (p1 == minp) ? w / (maxp + w) : maxp / (maxp + w);
    }
  } // End Cheng
  if (maxp < 1.0) // Atkinson and Whittaker 1
  {
    const Scalar t = 1.0 / (1.0 + std::sqrt(maxp * (1.0 - maxp) / (minp * (1.0 - minp))));
    const Scalar tc = 1.0 - t;
    const Scalar p = maxp * t / (maxp * t + minp * tc);
    for (;;)
    {
      const Scalar u = RandomGenerator::Generate();
      const Scalar e = -std::log(RandomGenerator::Generate());
      if (u <= p)
      {
        const Scalar x = t * std::pow(u / p, 1.0 / minp);
        // Acceptation test
        if (e >= (1.0 - maxp) * std::log((1.0 - x) / tc)) return ((p1 == minp) ? x : 1.0 - x);
      }
      else
      {
        const Scalar x = 1.0 - tc * std::exp(log1p((p - u) / (1.0 - p)) / maxp);
        // Acceptation test
        if (e >= (1.0 - minp) * std::log(x / t)) return ((p1 == minp) ? x : 1.0 - x);
      }
    }
  } // End Atkinson and Whittaker 1
  // Remaining case, Atkinson and Whittaker 2
  const Scalar t = (minp > 1) ? (1.0 - minp) / (maxp + 1.0 - minp) : 0.5;
  const Scalar tc = 1.0 - t;
  const Scalar p = maxp * t / (maxp * t + minp * std::pow(tc, maxp));
  for (;;)
  {
    const Scalar u = RandomGenerator::Generate();
    const Scalar e = -std::log(RandomGenerator::Generate());
    if (u <= p)
    {
      const Scalar x = t * std::pow(u / p, 1.0 / minp);
      if (e >= (1.0 - maxp) * log1p(-x)) return ((p1 == minp) ? x : 1.0 - x);
    }
    else
    {
      const Scalar x = 1.0 - tc * std::exp(log1p((p - u) / (1.0 - p)) / maxp);
      if (e >= (1.0 - minp) * std::log(x / t)) return ((p1 == minp) ? x : 1.0 - x);
    }
  } // End Atkinson and Whittaker 2
} // End of rBeta

Point DistFunc::rBeta(const Scalar p1,
                      const Scalar p2,
                      const UnsignedInteger size)
{
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = rBeta(p1, p2);
  return result;
}

/*******************************************************************************************************/
/* Binomial distribution, i.e. with a PDF equals to C(n, p) p^k (1 - p)^(n - k) */
/*******************************************************************************************************/
static Scalar bd0(const UnsignedInteger k,
                  const Scalar np)
{
  const Scalar kpnp = k + np;
  const Scalar kmnp = k - np;
  if (std::abs(kmnp) < 0.1 * kpnp)
  {
    const Scalar v = kmnp / kpnp;
    Scalar s = kmnp * v;
    if (std::abs(s) < SpecFunc::Precision) return s;
    Scalar ej = 2.0 * k * v;
    const Scalar v2 = v * v;
    for (UnsignedInteger j = 1; j < SpecFunc::MaximumIteration; ++j)
    {
      ej *= v2;
      const Scalar s1 = s + ej / (2 * j + 1);
      if (s1 == s) return s1;
      s = s1;
    }
  }
  return k * std::log(k / np) - kmnp;
}

/* Probability distribution function
   We use the algorithm described in:
   Catherine Loader, "Fast and Accurate Computation of Binomial Probabilities", On-
line:   https://lists.gnu.org/archive/html/octave-maintainers/2011-09/pdfK0uKOST642.pdf, accessed 22 December 2018, 2000.
   with the improvements discussed in:
   Jannis Dimitriadis, "On the Accuracy of Loader’s Algorithm for the Binomial
   Density and Algorithms for Rectangle Probabilities for Markov Increments",
   Dissertation, Universitat Trier, August 2016.
*/
Scalar DistFunc::dBinomial(const UnsignedInteger n,
                           const Scalar p,
                           const UnsignedInteger k)
{
  if (p < 0.0) throw InvalidArgumentException(HERE) << "Error: dBinomial expects p>=0";
  if (k > n) return 0.0;
  if (p == 0.0) return (k == 0 ? 1.0 : 0.0);
  if (p == 1.0) return (k == n ? 1.0 : 0.0);
  if (k == 0)
  {
    if (n == 0) return 1.0;
    const Scalar lc = (p < 0.1 ? -bd0(n, n * (1.0 - p)) - n * p : n * log1p(-p));
    return std::exp(lc);
  }
  if (k == n)
  {
    const Scalar lc = (p > 0.9 ? -bd0(n, n * p) - n * (1.0 - p) : n * std::log(p));
    return std::exp(lc);
  }
  const Scalar lc = SpecFunc::Stirlerr(n) - SpecFunc::Stirlerr(k) - SpecFunc::Stirlerr(n - k) - bd0(k, n * p) - bd0(n - k, n * (1.0 - p));
  return std::exp(lc - SpecFunc::LOGSQRT2PI - 0.5 * (std::log(1.0 * k) + log1p(-(1.0 * k) / n)));
}

Scalar DistFunc::logdBinomial(const UnsignedInteger n,
                              const Scalar p,
                              const UnsignedInteger k)
{
  if (k > n) return SpecFunc::LowestScalar;
  if (p == 0.0) return (k == 0 ? 0.0 : SpecFunc::LowestScalar);
  if (p == 1.0) return (k == n ? 0.0 : SpecFunc::LowestScalar);
  if (k == 0)
  {
    if (n == 0) return 0.0;
    return (p < 0.1 ? -bd0(n, n * (1.0 - p)) - n * p : n * log1p(-p));
  }
  if (k == n)
    return (p > 0.9 ? -bd0(n, n * p) - n * (1.0 - p) : n * std::log(p));
  const Scalar lc = SpecFunc::Stirlerr(n) - SpecFunc::Stirlerr(k) - SpecFunc::Stirlerr(n - k) - bd0(k, n * p) - bd0(n - k, n * (1.0 - p));
  return lc - SpecFunc::LOGSQRT2PI - 0.5 * (std::log(1.0 * k) + log1p(-(1.0 * k) / n));
}

/* Random number generation
   We use the rejection algorithm described in:
   Wolfgang Hormann, "The Generation of Binomial Random Variates",
   Journal of Statistical Computation and Simulation 46, pp. 101-110, 1993
   http://epub.wu.ac.at/1242/
*/
UnsignedInteger DistFunc::rBinomial(const UnsignedInteger n,
                                    const Scalar p)
{
  // Quick return for degenerate cases
  if (n == 0) return 0;
  if (p == 0.0) return 0;
  if (p == 1.0) return n;
  // Use symmetry
  const Scalar q = std::min(p, 1.0 - p);
  const Bool complementary = p > 0.5;
  // Small case, use inversion
  if (n * q <= 15)
  {
    const Scalar r = q / (1.0 - q);
    Scalar t = std::pow(1.0 - q, static_cast<int>(n));
    Scalar s = t;
    const Scalar u = RandomGenerator::Generate();
    for (UnsignedInteger k = 0; k <= n; ++k)
    {
      if (s >= u) return (complementary ? n - k : k);
      t *= r * (n - k) / (k + 1.0);
      s += t;
    }
    // Should never go there, except in case of round-off errors
    return 0;
  }
  // Large case, use the algorithm described in the reference.
  // Setup
  const Scalar m = floor((n + 1) * q);
  const Scalar r = q / (1.0 - q);
  const Scalar nr = (n + 1) * r;
  const Scalar npq = n * q * (1.0 - q);
  const Scalar npqSqrt = std::sqrt(npq);
  const Scalar b = 1.15 + 2.53 * npqSqrt;
  const Scalar a = -0.0873 + 0.0248 * b + 0.01 * q;
  const Scalar c = n * q + 0.5;
  const Scalar alpha = (2.83 + 5.1 / b) * npqSqrt;
  const Scalar vr = 0.92 - 4.2 / b;
  const Scalar urvr = 0.86 * vr;
  Scalar u = 0.0;
  Scalar k = 0.0;
  // Main loop
  for (;;)
  {
    Scalar v = RandomGenerator::Generate();
    if (v <= urvr)
    {
      u = v / vr - 0.43;
      k = floor((2 * a / (0.5 - std::abs(u)) + b) * u + c);
      return (complementary ? static_cast<UnsignedInteger>(n - k) : static_cast<UnsignedInteger>(k));
    } // v <= urvr
    if (v >= vr)
    {
      u = RandomGenerator::Generate() - 0.5;
    } // v >= vr
    else
    {
      u = v / vr - 0.93;
      u = (u < 0.0 ? -0.5 : 0.5) - u;
      v = RandomGenerator::Generate() * vr;
    } // v < vr
    const Scalar us = 0.5 - std::abs(u);
    k = floor((2.0 * a / us + b) * u + c);
    if ((k < 0.0) || (k > n)) continue;
    v = v * alpha / (a / (us * us) + b);
    const Scalar km = std::abs(k - m);
    // Recursive evaluation of f(k)
    if (km <= 15)
    {
      Scalar f = 1.0;
      if (m < k)
      {
        Scalar i = m;
        do
        {
          ++i;
          f *= (nr / i - r);
        }
        while (i < k);
      } // m < k
      else if (m > k)
      {
        Scalar i = k;
        do
        {
          ++i;
          v *= (nr / i - r);
        }
        while (i < m);
      } // m > k
      if (v <= f) return (complementary ? static_cast<UnsignedInteger>(n - k) : static_cast<UnsignedInteger>(k));
      continue;
    } // km <= 15
    // Squeeze-acceptance or rejection
    v = std::log(v);
    const Scalar rho = km / npq * (((km / 3.0 + 0.625) * km + 1.0 / 6.0) / npq + 0.5);
    const Scalar t = -km * km / (2.0 * npq);
    if (v < t - rho) return (complementary ? static_cast<UnsignedInteger>(n - k) : static_cast<UnsignedInteger>(k));
    if (v > t + rho) continue;
    const Scalar nm = n - m + 1;
    const Scalar h = (m + 0.5) * std::log((m + 1) / (r * nm)) + SpecFunc::Stirlerr(static_cast<UnsignedInteger>(m + 1)) + SpecFunc::Stirlerr(static_cast<UnsignedInteger>(nm));
    // Final acceptance-rejection
    const Scalar nk = n - k + 1;
    if (v <= h + (n + 1) * std::log(nm / nk) + (k + 0.5) * std::log(nk * r / (k + 1)) - SpecFunc::Stirlerr(static_cast<UnsignedInteger>(k + 1)) - SpecFunc::Stirlerr(static_cast<UnsignedInteger>(nk))) return (complementary ? static_cast<UnsignedInteger>(n - k) : static_cast<UnsignedInteger>(k));
  } // for(;;)
} // rBinomial

Indices DistFunc::rBinomial(const UnsignedInteger n,
                            const Scalar p,
                            const UnsignedInteger size)
{
  Indices result(size);
  //for (UnsignedInteger i = 0; i < size; ++i) result[i] = rBinomial(n, p);
  // Quick return for degenerate cases
  if (n == 0) return result;
  if (p == 0.0) return result;
  if (p == 1.0) return Indices(size, n);
  // Use symmetry
  const Scalar q = std::min(p, 1.0 - p);
  const Bool complementary = p > 0.5;
  // Small case, use inversion
  if (n * q <= 15)
  {
    const Scalar r = q / (1.0 - q);
    Scalar t = std::pow(1.0 - q, static_cast<int>(n));
    Scalar s = t;
    for (UnsignedInteger index = 0; index < size; ++index)
    {
      const Scalar u = RandomGenerator::Generate();
      for (UnsignedInteger k = 0; k <= n; ++k)
      {
        if (s >= u)
        {
          result[k] = (complementary ? n - k : k);
          break;
        }
        t *= r * (n - k) / (k + 1.0);
        s += t;
      }
      // Should never go there, except in case of round-off errors
    } // index
  } // n * q < 15
  // Large case, use the algorithm described in the reference.
  // Setup
  const Scalar m = floor((n + 1) * q);
  const Scalar r = q / (1.0 - q);
  const Scalar nr = (n + 1) * r;
  const Scalar npq = n * q * (1.0 - q);
  const Scalar npqSqrt = std::sqrt(npq);
  const Scalar b = 1.15 + 2.53 * npqSqrt;
  const Scalar a = -0.0873 + 0.0248 * b + 0.01 * q;
  const Scalar c = n * q + 0.5;
  const Scalar alpha = (2.83 + 5.1 / b) * npqSqrt;
  const Scalar vr = 0.92 - 4.2 / b;
  const Scalar urvr = 0.86 * vr;
  for (UnsignedInteger index = 0; index < size; ++index)
  {
    Scalar u = 0.0;
    Scalar k = 0.0;
    // Main loop
    for (;;)
    {
      Scalar v = RandomGenerator::Generate();
      if (v <= urvr)
      {
        u = v / vr - 0.43;
        k = floor((2 * a / (0.5 - std::abs(u)) + b) * u + c);
        result[index] = (complementary ? static_cast<UnsignedInteger>(n - k) : static_cast<UnsignedInteger>(k));
        break;
      } // v <= urvr
      if (v >= vr)
      {
        u = RandomGenerator::Generate() - 0.5;
      } // v >= vr
      else
      {
        u = v / vr - 0.93;
        u = (u < 0.0 ? -0.5 : 0.5) - u;
        v = RandomGenerator::Generate() * vr;
      } // v < vr
      const Scalar us = 0.5 - std::abs(u);
      k = floor((2.0 * a / us + b) * u + c);
      if ((k < 0.0) || (k > n)) continue;
      v = v * alpha / (a / (us * us) + b);
      const Scalar km = std::abs(k - m);
      // Recursive evaluation of f(k)
      if (km <= 15)
      {
        Scalar f = 1.0;
        if (m < k)
        {
          Scalar i = m;
          do
          {
            ++i;
            f *= (nr / i - r);
          }
          while (i < k);
        } // m < k
        else if (m > k)
        {
          Scalar i = k;
          do
          {
            ++i;
            v *= (nr / i - r);
          }
          while (i < m);
        } // m > k
        if (v <= f)
        {
          result[index] = (complementary ? static_cast<UnsignedInteger>(n - k) : static_cast<UnsignedInteger>(k));
          break;
        }
        continue;
      } // km <= 15
      // Squeeze-acceptance or rejection
      v = std::log(v);
      const Scalar rho = km / npq * (((km / 3.0 + 0.625) * km + 1.0 / 6.0) / npq + 0.5);
      const Scalar t = -km * km / (2.0 * npq);
      if (v < t - rho)
      {
        result[index] = (complementary ? static_cast<UnsignedInteger>(n - k) : static_cast<UnsignedInteger>(k));
        break;
      }
      if (v > t + rho) continue;
      const Scalar nm = n - m + 1;
      const Scalar h = (m + 0.5) * std::log((m + 1) / (r * nm)) + SpecFunc::Stirlerr(static_cast<UnsignedInteger>(m + 1)) + SpecFunc::Stirlerr(static_cast<UnsignedInteger>(nm));
      // Final acceptance-rejection
      const Scalar nk = n - k + 1;
      if (v <= h + (n + 1) * std::log(nm / nk) + (k + 0.5) * std::log(nk * r / (k + 1)) - SpecFunc::Stirlerr(static_cast<UnsignedInteger>(k + 1)) - SpecFunc::Stirlerr(static_cast<UnsignedInteger>(nk)))
      {
        result[index] = (complementary ? static_cast<UnsignedInteger>(n - k) : static_cast<UnsignedInteger>(k));
        break;
      }
    } // for(;;)
  } // index
  return result;
}

/*******************************************************************************************************/
/* Discrete distribution, ie supported by {0,...,n-1} where n = size(weights) */
/* It implements the alias method as described here: */
/* http://keithschwarz.com/darts-dice-coins/ */
/*******************************************************************************************************/
Indices DistFunc::rDiscrete(const Point & probabilities,
                            const UnsignedInteger size)
{
  Point base;
  Indices alias;
  return rDiscrete(probabilities, base, alias, size);
}

Indices DistFunc::rDiscrete(const Point & probabilities,
                            Point & base,
                            Indices & alias,
                            const UnsignedInteger size)
{
  // Setup part
  const UnsignedInteger n = probabilities.getSize();
  // Weights are normalized such that they sum to n
  Point weights(probabilities * n);
  base = Point(n);
  alias = Indices(n);
  Indices small(0);
  Indices large(0);
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    if (weights[i] < 1.0) small.add(i);
    else large.add(i);
  } // i
  UnsignedInteger smallSize = small.getSize();
  UnsignedInteger largeSize = large.getSize();
  UnsignedInteger indexSmall = 0;
  UnsignedInteger indexLarge = 0;
  while ((indexSmall < smallSize) && (indexLarge < largeSize))
  {
    const UnsignedInteger s = small[indexSmall];
    const Scalar pS = weights[s];
    ++indexSmall;
    const UnsignedInteger l = large[indexLarge];
    ++indexLarge;
    base[s] = pS;
    alias[s] = l;
    weights[l] = (pS + weights[l]) - 1.0;
    if (weights[l] < 1.0)
    {
      small.add(l);
      ++smallSize;
    }
    else
    {
      large.add(l);
      ++largeSize;
    }
  } // small and large not empty
  for (; indexLarge < largeSize; ++indexLarge)
    base[large[indexLarge]] = 1.0;
  // The next loop accurs only due to numerical instability
  for (; indexSmall < smallSize; ++indexSmall)
    base[small[indexSmall]] = 1.0;
  return rDiscrete(base, alias, size);
}

UnsignedInteger DistFunc::rDiscrete(const Point & probabilities)
{
  Point base;
  Indices alias;
  return rDiscrete(probabilities, base, alias, 1)[0];
}

UnsignedInteger DistFunc::rDiscrete(const Point & probabilities,
                                    Point & base,
                                    Indices & alias)
{
  return DistFunc::rDiscrete(probabilities, base, alias, 1)[0];
}

UnsignedInteger DistFunc::rDiscrete(const Point & base,
                                    const Indices & alias)
{
  const UnsignedInteger i = RandomGenerator::IntegerGenerate(base.getSize());
  if (RandomGenerator::Generate() < base[i]) return i;
  return alias[i];
}

Indices DistFunc::rDiscrete(const Point & base,
                            const Indices & alias,
                            const UnsignedInteger size)
{
  Indices result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = rDiscrete(base, alias);
  return result;
}

/*******************************************************************************************************/
/* Normalized Gamma distribution, i.e. with a PDF equals to x ^ (k - 1) . exp(-x) / gamma(k) . (x > 0) */
/*******************************************************************************************************/
/* CDF */
Scalar DistFunc::pGamma(const Scalar k,
                        const Scalar x,
                        const Bool tail)
{
  return SpecFunc::RegularizedIncompleteGamma(k, x, tail);
}

/* CDF Inverse */
Scalar DistFunc::qGamma(const Scalar k,
                        const Scalar p,
                        const Bool tail)
{
  const Scalar cp = std::min(std::max(p, SpecFunc::MinScalar), 0.5 + (0.5 - SpecFunc::ScalarEpsilon));
  return SpecFunc::RegularizedIncompleteGammaInverse(k, cp, tail);
}
/* Random number generation
   We use the algorithm described in:
   Variables": ACM Transactions on Mathematical Software, Vol. 26, No. 3,
   September 2000, Pages 363-372
   with a small optimization on the beta that appears in the squeezing function (1 + beta * x^4)*exp(-x^2/2).
   We also add the special treatment of the case k < 1
*/
Scalar DistFunc::rGamma(const Scalar k)
{
  // Special case k < 1.0
  Scalar correction = 1.0;
  Scalar alpha = k;
  if (alpha < 1.0)
  {
    correction = std::pow(RandomGenerator::Generate(), 1.0 / alpha);
    ++alpha;
  }
  const Scalar d = alpha - 0.3333333333333333333333333;
  const Scalar c = 1.0 / std::sqrt(9.0 * d);
  Scalar x = -1.0;
  Scalar v = -1.0;
  for (;;)
  {
    do
    {
      x = rNormal();
      v = 1.0 + c * x;
    }
    while (v <= 0.0);
    v = v * v * v;
    const Scalar u = RandomGenerator::Generate();
    const Scalar x2 = x * x;
    // Quick acceptation test
    // 0.03431688782875261396035499 is the numerical solution of the squeezing
    // problem described in
    if (u < 1.0 - 0.03431688782875261396035499 * x2 * x2) return correction * d * v;
    // Acceptation test
    if (std::log(u) < 0.5 * x2 + d * (1.0 - v + std::log(v))) return correction * d * v;
  }
} // End of rGamma

Point DistFunc::rGamma(const Scalar k,
                       const UnsignedInteger size)
{
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = rGamma(k);
  return result;
}

/********************************/
/* Hypergeometric distribution. */
/********************************/
/*
Some useful symmetry relations:
d(n,k,m,x) = d(n, n-k,   m, m-x)
           = d(n,   k, n-m, k-x)
           = d(n,   m,   k,   x)
*/
Scalar DistFunc::dHypergeometric(const UnsignedInteger n,
                                 const UnsignedInteger k,
                                 const UnsignedInteger m,
                                 const UnsignedInteger x)
{
  if (x + n < k + m) return 0.0;
  if (x > k || x > m) return 0.0;
  if ((m == 0) || (m == n)) return 1.0;
#ifdef OPENTURNS_HAVE_BOOST
  return boost::math::pdf(boost::math::hypergeometric_distribution<Scalar>(k, m, n), x);
#else
  // Check range
  return std::exp(logdHypergeometric(n, k, m, x));
#endif
}

Scalar DistFunc::logdHypergeometric(const UnsignedInteger n,
                                    const UnsignedInteger k,
                                    const UnsignedInteger m,
                                    const UnsignedInteger x)
{
  if (x + n < k + m) return SpecFunc::LowestScalar;
  if (x > k || x > m) return SpecFunc::LowestScalar;
  if ((m == 0) || (m == n)) return 0.0;
#ifdef OPENTURNS_HAVE_BOOST
  return std::log(boost::math::pdf(boost::math::hypergeometric_distribution<Scalar>(k, m, n), x));
#else
  // Check range
  Scalar p = (1.0 * m) / n;
  const Scalar nm = -SpecFunc::LOGSQRT2PI - 0.5 * std::log(m * (1.0 - p)) + SpecFunc::Stirlerr(n) - SpecFunc::Stirlerr(m) - SpecFunc::Stirlerr(n - m);
  Scalar kx = 0.0;
  if (x == 0) kx = k * log1p(-p);
  else if (x == k) kx = x * std::log(p);
  else kx = -SpecFunc::LOGSQRT2PI - 0.5 * std::log(x * (1.0 - (1.0 * x) / k)) + SpecFunc::Stirlerr(k) - SpecFunc::Stirlerr(x) - SpecFunc::Stirlerr(k - x) - bd0(x, k * p) - bd0(k - x, k * (1.0 - p));
  Scalar nkmx = 0.0;
  if (x == m) nkmx = (n - k) * log1p(-p);
  else if (m - x == n - k) nkmx = (m - x) * std::log(p);
  else
  {
    const UnsignedInteger delta = n - k + x - m;
    nkmx = -SpecFunc::LOGSQRT2PI - 0.5 * std::log((m - x) * (1.0 - (1.0 * (m - x)) / (n - k))) + SpecFunc::Stirlerr(n - k) - SpecFunc::Stirlerr(m - x) - SpecFunc::Stirlerr(delta) - bd0(m - x, (n - k) * p) - bd0(delta, (n - k) * (1.0 - p));
  }
  return kx + nkmx - nm;
#endif
}

Scalar DistFunc::pHypergeometric(const UnsignedInteger n,
                                 const UnsignedInteger k,
                                 const UnsignedInteger m,
                                 const UnsignedInteger x,
                                 const Bool tail)
{
  if (x + n < k + m) return 0.0;
  if (x > k || x > m) return 1.0;
#ifdef OPENTURNS_HAVE_BOOST
  if (tail) return boost::math::cdf(boost::math::complement(boost::math::hypergeometric_distribution<Scalar>(k, m, n), x));
  return boost::math::cdf(boost::math::hypergeometric_distribution<Scalar>(k, m, n), x);
#else
  // Compute the summation on the same side of the mode, ie
  // from 0 to x if x <= mode, or from x+1 to max(range) then complement if x > mode
  // Wikipedia H(n, k, m; x), OT H(n, k, m; x), Boost H(x; r, m, n) r = k
  const UnsignedInteger mode = (m + 1) * (k + 1);
  Bool complement = tail;
  Scalar cdf = 0.0;
  UnsignedInteger t = x;
  if ((n + 2) * x < mode)
  {
    cdf = dHypergeometric(n, k, m, x);
    Scalar delta = cdf;
    UnsignedInteger xMin = k + m > n ? k + m - n : 0;
    Scalar epsilon = complement ? SpecFunc::Precision : cdf * SpecFunc::Precision;
    while (delta > epsilon)
    {
      delta *= (t / (1.0 + k - t)) * ((n + t - m - k) / (1.0 + m - t));
      cdf += delta;
      if (t == xMin)
        break;
      --t;
    }
  } // x < mode
  else
  {
    complement = !complement;
    UnsignedInteger xMax = std::min(k, m);
    if (t < xMax)
    {
      ++t;
      cdf = dHypergeometric(n, k, m, t);
      Scalar delta = cdf;
      Scalar epsilon = complement ? SpecFunc::Precision : cdf * SpecFunc::Precision;
      while ((t <= xMax) && (delta > epsilon))
      {
        delta *= ((m - t) / (t + 1.0)) * ((k - t) / (n + t + 1.0 - m - k));
        cdf += delta;
        ++t;
      }
    } // x < xMax
  } // x >= mode
  if (complement) return 1.0 - cdf;
  return cdf;
#endif
}

UnsignedInteger DistFunc::rHypergeometric(const UnsignedInteger n,
    const UnsignedInteger k,
    const UnsignedInteger m)
{
  // Generate the probability table
  Point probabilities(n + 1);
  UnsignedInteger xMin = (k + m >= n ? k + m - n : 0);
  UnsignedInteger xMax = std::min(m, k);
  UnsignedInteger xMode = ((k + 1) * (m + 1)) / (n + 2);
  Scalar p = DistFunc::dHypergeometric(n, k, m, xMode);
  probabilities[xMode] = p;
  for (UnsignedInteger x = xMode + 1; x <= xMax; ++x)
  {
    p *= ((1.0 + k - x) / x) * ((1.0 + m - x) / (n + x - m - k));
    probabilities[k] = p;
  }
  p = probabilities[xMode];
  for (UnsignedInteger x = xMode; x > xMin; --x)
  {
    p *= (x / (k - x + 1.0)) * ((n + x - m - k) / (m - x + 1.0));
    probabilities[x - 1] = p;
  }
  return rDiscrete(probabilities);
}

Indices DistFunc::rHypergeometric(const UnsignedInteger n,
                                  const UnsignedInteger k,
                                  const UnsignedInteger m,
                                  const UnsignedInteger size)
{
  // Generate the probability table
  Point probabilities(n + 1);
  UnsignedInteger xMin = (k + m >= n ? k + m - n : 0);
  UnsignedInteger xMax = std::min(m, k);
  UnsignedInteger xMode = ((k + 1) * (m + 1)) / (n + 2);
  Scalar p = DistFunc::dHypergeometric(n, k, m, xMode);
  probabilities[xMode] = p;
  for (UnsignedInteger x = xMode + 1; x <= xMax; ++x)
  {
    p *= ((1.0 + k - x) / x) * ((1.0 + m - x) / (n + x - m - k));
    probabilities[k] = p;
  }
  p = probabilities[xMode];
  for (UnsignedInteger x = xMode; x > xMin; --x)
  {
    p *= (x / (k - x - 1.0)) * ((n + x - m - k) / (m - x - 1.0));
    probabilities[x - 1] = p;
  }
  return rDiscrete(probabilities, size);
}

/****************************/
/* Kolmogorov distribution. */
/****************************/
/* CDF
   The algorithms and the selection strategy is described in:
   Simard, R. and L'Ecuyer, P. "Computing the Two-Sided Kolmogorov-Smirnov Distribution", Journal of Statistical Software, 2010.
   The implementation is from the first author, initially published under the GPL v3 license but used here with written permission of the author.
*/
Scalar DistFunc::pKolmogorov(const UnsignedInteger n,
                             const Scalar x,
                             const Bool tail)
{
  if (tail) return KSfbar(n, x);
  else return KScdf(n, x);
}

/***************************************************************************************************************/
/* Normalized NonCentralChiSquare distribution, i.e. with a PDF equals to (eq. 31.15 p.516 of the reference):  */
/* exp(-delta^2 / 2) * (nu / (nu + x^2)) ^ ((nu + 1) / 2) / (sqrt(nu * Pi) * Gamma(nu / 2)) * SUM              */
/* where SUM = sum_0^inf Gamma((nu + k + 1) / 2) * omega^k / Gamma(k + 1)                                      */
/* and omega = x * delta * sqrt(2 / (nu + x^2))                                                                */
/* In order to derive simple update formulas for the terms in the sum, we separate the odd indices from the    */
/* even ones:                                                                                                  */
/* SUM = SUM_ODD + SUM_EVEN, where:                                                                            */
/* SUM_ODD  = sum_0^inf Gamma(nu / 2 + k + 1) * omega * z^k / Gamma(2 * k + 2)                                 */
/* SUM_EVEN = sum_O^inf Gamma((nu + 1) / 2 + k) * z^k / Gamma(2 * k + 1)                                       */
/* and z = omega^2                                                                                             */
/* The summation is done starting at the kM chosen as for the pNonCentralStudent method. See                   */
/* Denise Benton, K. Krishnamoorthy, "Computing discrete mixtures of continuous distributions: noncentral      */
/* chisquare, noncentral t and the distribution of the square of the sample multiple correlation coefficient", */
/* Computational Statistics & Data Analysis, 43 (2003) pp 249-267                                              */
/* Reference:                                                                                                  */
/* Norman L. Johnson, Samuel Kotz, N. Balakrishnan, "Continuous univariate distributions volume 2", second     */
/* edition, 1995, Wiley Inter-Science                                                                          */
/***************************************************************************************************************/
/* PDF */
Scalar DistFunc::dNonCentralChiSquare(const Scalar nu,
                                      const Scalar lambda,
                                      const Scalar x,
                                      const Scalar precision,
                                      const UnsignedInteger maximumIteration)
{
  if (!(nu >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu must be >= 0.";
  if (!(lambda >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the non-centrality parameter lambda must be >= 0.";
  if (x <= 0.0) return 0.0;
  const Scalar halfNu = 0.5 * nu;
  // Early exit for lambda == 0, central ChiSquare PDF
  if (std::abs(lambda) < precision) return std::exp((halfNu - 1.0) * std::log(x) - 0.5 * x - SpecFunc::LnGamma(halfNu) - halfNu * M_LN2);
  // Case lambda <> 0
  const Scalar halfLambda = 0.5 * lambda;
  // Starting index in the sum: integral part of halfDelta2 and insure that it is at least 1
  const UnsignedInteger k = std::max(1UL, static_cast<UnsignedInteger>(floor(halfLambda)));
  // Loop forward and backward starting from k
  // Initialization
  Scalar pForward = std::exp(-halfLambda - 0.5 * x + (halfNu + k - 1.0) * std::log(x) - SpecFunc::LogGamma(k + 1.0) - SpecFunc::LogGamma(halfNu + k) - (2.0 * k + halfNu) * M_LN2 + k * std::log(lambda));
  Scalar pBackward = pForward;
  Scalar value = pForward;
  Scalar error = SpecFunc::MaxScalar;
  UnsignedInteger kForward = k;
  UnsignedInteger kBackward = k;
#define FORWARD_ITERATION                                               \
  pForward *= halfLambda * x / (2.0 * (halfNu + kForward) * (kForward + 1)); \
  value += pForward;
#define BACKWARD_ITERATION                                              \
  pBackward *= 2.0 * (halfNu + kBackward - 1) * kBackward / (halfLambda * x); \
  value += pBackward;

  // Here, i is an UnsignedInteger as it is only a loop counter
  UnsignedInteger i = 1;
  const UnsignedInteger imax = std::min(k, maximumIteration);
  //        while((error > precision * (std::abs(value) + precision)) && (i <= imax))
  while((error > 0.0) && (i <= imax))
  {
    FORWARD_ITERATION;
    BACKWARD_ITERATION;
    error = pForward + pBackward;
    ++kForward;
    --kBackward;
    ++i;
  }
  // Do we have to perform further forward iterations?
  //        while ((error > precision * (std::abs(value) + precision)) && (i <= MaximumIteration))
  while ((error > 0.0) && (i <= maximumIteration))
  {
    FORWARD_ITERATION;
    error = pForward;
    ++kForward;
    ++i;
  }
#undef FORWARD_ITERATION
#undef BACKWARD_ITERATION
  if (error > precision * (std::abs(value) + precision)) LOGWARN(OSS() << "Warning: in DistFunc::dNonCentralChiSquare(nu, lambda, x), no convergence after " << i << " iterations. Error is " << error * value << " value is " << value << " for nu=" << nu << ", lambda=" << lambda << " and x=" << x);
  // Clip to [0,+inf[ in order to get rid of small rounding error
  return (value <= 0.0 ? 0.0 : value);
}
/* CDF
   We use the algorithm described in:
   Denise Benton, K. Krishnamoorthy, "Computing discrete mixtures of continuous distributions: noncentral chisquare, noncentral t
   and the distribution of the square of the sample multiple correlation coefficient",
   Computational Statistics & Data Analysis, 43 (2003) pp 249-267
*/
Scalar DistFunc::pNonCentralChiSquare(const Scalar nu,
                                      const Scalar lambda,
                                      const Scalar x,
                                      const Bool tail,
                                      const Scalar precision,
                                      const UnsignedInteger maximumIteration)
{
  if (!(nu >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu must be >= 0.";
  if (!(lambda >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the non-centrality parameter lambda must be >= 0.";
  if (x <= 0.0) return (tail ? 1.0 : 0.0);
  const Scalar halfNu = 0.5 * nu;
  const Scalar halfX = 0.5 * x;
  // Early exit for lambda == 0, central ChiSquare PDF
  if (std::abs(lambda) < precision) return pGamma(halfNu, halfX, tail);
  // Case lambda <> 0
  const Scalar halfLambda = 0.5 * lambda;
  // Starting index in the sum: integral part of halfDelta2 and insure that it is at least 1
  const UnsignedInteger k = std::max(1UL, static_cast<UnsignedInteger>(floor(halfLambda)));
  // Loop forward and backward starting from k
  // Initialization
  const Scalar logHalfX = std::log(halfX);
  Scalar xForward = std::exp((halfNu + k - 1) * logHalfX - halfX - SpecFunc::LogGamma(halfNu + k));
  Scalar expForward = std::exp(-halfLambda + k * std::log(halfLambda) - SpecFunc::LogGamma(k + 1.0));
  Scalar gammaForward = pGamma(halfNu + k, halfX);
  Scalar pForward = expForward * gammaForward;
  Scalar xBackward = xForward;
  Scalar expBackward = expForward;
  Scalar gammaBackward = gammaForward;
  Scalar pBackward = expBackward * gammaBackward;
  Scalar value = pForward;
  Scalar error = SpecFunc::MaxScalar;
  UnsignedInteger kForward = k;
  UnsignedInteger kBackward = k;
#define FORWARD_ITERATION                       \
  xForward *= halfX / (halfNu + kForward);      \
  expForward *= halfLambda / (kForward + 1);    \
  gammaForward -= xForward;                     \
  pForward = expForward * gammaForward;         \
  value += pForward;
#define BACKWARD_ITERATION                              \
  expBackward *= kBackward / halfLambda;                \
  gammaBackward += xBackward;                           \
  xBackward *= (halfNu + kBackward - 1) / halfX;        \
  pBackward = expBackward * gammaBackward;              \
  value += pBackward;

  // Here, i is an UnsignedInteger as it is only a loop counter
  UnsignedInteger i = 1;
  const UnsignedInteger imax = std::min(k, maximumIteration);
  //        while((error > precision * (std::abs(value) + precision)) && (i <= imax))
  while((error > 0.0) && (i <= imax))
  {
    FORWARD_ITERATION;
    BACKWARD_ITERATION;
    error = pForward + pBackward;
    ++kForward;
    --kBackward;
    ++i;
  }
  // Do we have to perform further forward iterations?
  //        while ((error > precision * (std::abs(value) + precision)) && (i <= MaximumIteration))
  while ((error > 0.0) && (i <= maximumIteration))
  {
    FORWARD_ITERATION;
    error = pForward;
    ++kForward;
    ++i;
  }
#undef FORWARD_ITERATION
#undef BACKWARD_ITERATION
  if (error > precision * (std::abs(value) + precision)) LOGWARN(OSS() << "Warning: in DistFunc::dNonCentralChiSquare(nu, lambda, x), no convergence after " << i << " iterations. Error is " << error * value << " value is " << value << " for nu=" << nu << ", lambda=" << lambda << " and x=" << x);
  // Clip to [0,1] in order to get rid of small rounding error
  value = (value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value));
  // Check if we are asked for the tail CDF
  if (tail) value = 1.0 - value;
  return value;
}
/* Random number generation
   We use a transformation method based on Gamma and Normal transformation:
   If N is Normal(delta, 1) distributed and G is Gamma(nu / 2) distributed,
   sqrt(2 * nu) * N / sqrt(G) is distributed according to NonCentralChiSquare(nu, delta)
*/
Scalar DistFunc::rNonCentralChiSquare(const Scalar nu,
                                      const Scalar lambda)
{
  if (!(nu >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the number of degrees of freedom nu must be >= 0.";
  if (!(lambda >= 0.0)) throw InvalidArgumentException(HERE) << "Error: the non-centrality parameter lambda must be >= 0.";
  // If the non-central parameter is zero return a usual chi-square realization
  if (lambda == 0.0) return 2.0 * rGamma(0.5 * nu);
  // Use the decomposition of a zero degree of freedom non-central chisquare and a nu degrees of freedom central chisquare
  const Scalar n = rPoisson(0.5 * lambda);
  return 2.0 * rGamma(0.5 * nu + n);
}

Point DistFunc::rNonCentralChiSquare(const Scalar nu,
                                     const Scalar lambda,
                                     const UnsignedInteger size)
{
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = rNonCentralChiSquare(nu, lambda);
  return result;
}

/************************************************************************************************************/
/* Normalized NonCentralStudent distribution, i.e. with a PDF equals to (eq. 31.15 p.516 of the reference): */
/* exp(-delta^2 / 2) * (nu / (nu + x^2)) ^ ((nu + 1) / 2) / (sqrt(nu * Pi) * Gamma(nu / 2)) * SUM           */
/* where SUM = sum_0^inf Gamma((nu + k + 1) / 2) * omega^k / Gamma(k + 1)                                   */
/* and omega = x * delta * sqrt(2 / (nu + x^2))                                                             */
/* Reference:                                                                                               */
/* Norman L. Johnson, Samuel Kotz, N. Balakrishnan, "Continuous univariate distributions volume 2", second  */
/* edition, 1995, Wiley Inter-Science                                                                       */
/************************************************************************************************************/
/* PDF
   We use the relation between the PDF and the CDF in order to reduce the computation of the PDF to two computations of the CDF */
Scalar DistFunc::dNonCentralStudent(const Scalar nu,
                                    const Scalar delta,
                                    const Scalar x)
{
  return StudentFunctions::NonCentralStudentPDF(nu, delta, x);
}
/************************************************************************************************************/
/* Normalized NonCentralStudent distribution, i.e. with a PDF equals to (eq. 31.15 p.516 of the reference): */
/* exp(-delta^2 / 2) * (nu / (nu + x^2)) ^ ((nu + 1) / 2) / (sqrt(nu * Pi) * Gamma(nu / 2)) * SUM           */
/* where SUM = sum_0^inf Gamma((nu + k + 1) / 2) * omega^k / Gamma(k + 1)                                   */
/* and omega = x * delta * sqrt(2 / (nu + x^2))                                                             */
/* In order to derive simple update formulas for the terms in the sum, we separate the odd indices from the */
/* even ones:                                                                                               */
/* SUM = SUM_ODD + SUM_EVEN, where:                                                                         */
/* SUM_ODD  = sum_0^inf Gamma(nu / 2 + k + 1) * omega * z^k / Gamma(2 * k + 2)                              */
/* SUM_EVEN = sum_O^inf Gamma((nu + 1) / 2 + k) * z^k / Gamma(2 * k + 1)                                    */
/* and z = omega^2                                                                                          */
/* The summation is done starting at the kM chosen as for the pNonCentralStudent method.                    */
/* Reference:                                                                                               */
/* Norman L. Johnson, Samuel Kotz, N. Balakrishnan, "Continuous univariate distributions volume 2", second  */
/* edition, 1995, Wiley Inter-Science                                                                       */
/************************************************************************************************************/
/* PDF */
Scalar DistFunc::dNonCentralStudentAlt0(const Scalar nu,
                                        const Scalar delta,
                                        const Scalar x,
                                        const Scalar precision,
                                        const UnsignedInteger maximumIteration)
{
  return StudentFunctions::NonCentralStudentPDFAlt0(nu, delta, x, precision, maximumIteration);
}
/* CDF
   We use the algorithm described in:
   Viktor Witkovsky, "A Note on Computing Extreme Tail Probabilities of the Noncentral T Distribution with Large Noncentrality Parameter"
   Computational Statistics & Data Analysis, 43 (2003) pp 249-267
*/
Scalar DistFunc::pNonCentralStudent(const Scalar nu,
                                    const Scalar delta,
                                    const Scalar x,
                                    const Bool tail)
{
  return StudentFunctions::NonCentralStudentCDF(nu, delta, x, tail);
}
/* Random number generation
   We use a transformation method based on Gamma and Normal transformation:
   If N is Normal(delta, 1) distributed and G is Gamma(nu / 2) distributed,
   sqrt(2 * nu) * N / sqrt(G) is distributed according to NonCentralStudent(nu, delta)
*/
Scalar DistFunc::rNonCentralStudent(const Scalar nu,
                                    const Scalar delta)
{
  return StudentFunctions::NonCentralStudentRealization(nu, delta);
}

Point DistFunc::rNonCentralStudent(const Scalar nu,
                                   const Scalar delta,
                                   const UnsignedInteger size)
{
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = StudentFunctions::NonCentralStudentRealization(nu, delta);
  return result;
}

/**************************************************************************************/
/* Normalized Normal distribution, i.e. with a PDF equals to exp(-x^2/2) / sqrt(2.Pi) */
/**************************************************************************************/
/* CDF */
Scalar DistFunc::pNormal(const Scalar x,
                         const Bool tail)
{
  if (tail) return pNormal(-x, false);
  const Scalar t = x * M_SQRT1_2;
  const Scalar z = std::abs(t);
  Scalar y;
  if (z < M_SQRT1_2) y = 0.5 + 0.5 * SpecFunc::Erf(t);
  else
  {
    y = 0.5 * SpecFunc::ErfC(z);
    if (t > 0.0) y = 1.0 - y;
  }
  return y;
}

Point DistFunc::pNormal(const Point & x,
                        const Bool tail)
{
  if (tail) return pNormal(x * (-1.0), false);
  const UnsignedInteger size = x.getSize();
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const Scalar t = x[i] * M_SQRT1_2;
    const Scalar z = std::abs(t);
    Scalar *y = &result[i];
    if (z < M_SQRT1_2) *y = 0.5 + 0.5 * SpecFunc::Erf(t);
    else
    {
      *y = 0.5 * SpecFunc::ErfC(z);
      if (t > 0.0) *y = 1.0 - *y;
    }
  }
  return result;
}

Scalar DistFunc::logpNormal(const Scalar x,
                            const Bool tail)
{
  if (tail) return logpNormal(-x, false);
  const Scalar t = x * M_SQRT1_2;
  if (x < -1.0) return std::log(0.5 * SpecFunc::ErfCX(-t)) - t * t;
  else return log1p(-0.5 * SpecFunc::ErfC(t));
}

Scalar DistFunc::pNormal2D(const Scalar x1,
                           const Scalar x2,
                           const Scalar rho,
                           const Bool tail)
{
  return Normal2DCDF(x1, x2, rho, tail);
}

Scalar DistFunc::pNormal3D(const Scalar x1,
                           const Scalar x2,
                           const Scalar x3,
                           const Scalar rho12,
                           const Scalar rho13,
                           const Scalar rho23,
                           const Bool tail)
{
  return Normal3DCDF(x1, x2, x3, rho12, rho13, rho23, tail);
}

/* CDF inverse
   It implements the algorithm of Peter John Acklam, see
   http://home.online.no/~pjacklam/notes/invnorm/index.html
*/
Scalar DistFunc::qNormal(const Scalar p,
                         const Bool tail)
{
  if (p <= 0.0) return (tail ?  37.5193793471445 : -37.5193793471445);
  if (p >= 1.0) return (tail ? -8.125890664701906 :  8.125890664701906);

  static const Scalar a[6] =
  {
    -3.969683028665376e+01,  2.209460984245205e+02,
      -2.759285104469687e+02,  1.383577518672690e+02,
      -3.066479806614716e+01,  2.506628277459239e+00
    };
  static const Scalar b[5] =
  {
    -5.447609879822406e+01,  1.615858368580409e+02,
      -1.556989798598866e+02,  6.680131188771972e+01,
      -1.328068155288572e+01
    };
  static const Scalar c[6] =
  {
    -7.784894002430293e-03, -3.223964580411365e-01,
      -2.400758277161838e+00, -2.549732539343734e+00,
      4.374664141464968e+00,  2.938163982698783e+00
    };
  static const Scalar d[4] =
  {
    7.784695709041462e-03,  3.224671290700398e-01,
    2.445134137142996e+00,  3.754408661907416e+00
  };
  Scalar x = -1.0;
  // Left tail
  if (p < 0.02425)
  {
    /* Rational approximation for tail region. */
    const Scalar q = std::sqrt(-2.0 * std::log(p));
    x = (((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5])
        / ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1.0);
  }
  // Central region
  else if (p <= 0.97575)
  {
    /* Rational approximation for central region. */
    const Scalar q = p - 0.5;
    const Scalar r = q * q;
    x = q * (((((a[0] * r + a[1]) * r + a[2]) * r + a[3]) * r + a[4]) * r + a[5])
        / (((((b[0] * r + b[1]) * r + b[2]) * r + b[3]) * r + b[4]) * r + 1.0);
  }
  // Right tail
  else
  {
    /* Rational approximation for tail region. */
    const Scalar q = std::sqrt(-2.0 * log1p(-p));
    x = -(((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5])
        / ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1.0);
  }
  /* The relative error of the approximation has absolute value less
     than 1.15e-9.  One iteration of Newton's rational method (second
     order) gives full machine precision... */
  // 2.50662827463100050241576528481 = sqrt(2.pi)
  const Scalar e = pNormal(x) - p;
  const Scalar u = e * 2.50662827463100050241576528481 * std::exp(0.5 * x * x);
  x -= u / (1.0 + 0.5 * x * u);
  return (tail ? -x : x);
}

Point DistFunc::qNormal(const Point & p,
                        const Bool tail)
{
  static const Scalar a[6] =
  {
    -3.969683028665376e+01,  2.209460984245205e+02,
      -2.759285104469687e+02,  1.383577518672690e+02,
      -3.066479806614716e+01,  2.506628277459239e+00
    };
  static const Scalar b[5] =
  {
    -5.447609879822406e+01,  1.615858368580409e+02,
      -1.556989798598866e+02,  6.680131188771972e+01,
      -1.328068155288572e+01
    };
  static const Scalar c[6] =
  {
    -7.784894002430293e-03, -3.223964580411365e-01,
      -2.400758277161838e+00, -2.549732539343734e+00,
      4.374664141464968e+00,  2.938163982698783e+00
    };
  static const Scalar d[4] =
  {
    7.784695709041462e-03,  3.224671290700398e-01,
    2.445134137142996e+00,  3.754408661907416e+00
  };
  const UnsignedInteger size = p.getSize();
  Point result(size);
  Scalar x = 0.0;
  Scalar q = 0.0;
  Scalar r = 0.0;
  Scalar e = 0.0;
  Scalar u = 0.0;
  Scalar prob = 0.0;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    prob = p[i];
    if (prob == 0.0)
    {
      result[i] = (tail ? 8.125890664701906 : -8.125890664701906);
      continue;
    }
    if (prob == 1.0)
    {
      result[i] = (tail ? -8.125890664701906 : 8.125890664701906);
      continue;
    }
    // Left tail
    if (prob < 0.02425)
    {
      /* Rational approximation for tail region. */
      q = std::sqrt(-2.0 * std::log(prob));
      x = (((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5])
          / ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1.0);
    }
    // Central region
    else if (prob <= 0.97575)
    {
      /* Rational approximation for central region. */
      q = prob - 0.5;
      r = q * q;
      x = q * (((((a[0] * r + a[1]) * r + a[2]) * r + a[3]) * r + a[4]) * r + a[5])
          / (((((b[0] * r + b[1]) * r + b[2]) * r + b[3]) * r + b[4]) * r + 1.0);
    }
    // Right tail
    else
    {
      /* Rational approximation for tail region. */
      q = std::sqrt(-2.0 * log1p(-prob));
      x = -(((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5])
          / ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1.0);
    }
    /* The relative error of the approximation has absolute value less
    than 1.15e-9.  One iteration of Newton's rational method (second
    order) gives full machine precision... */
    // 2.50662827463100050241576528481 = sqrt(2.pi)
    e = pNormal(x) - prob;
    u = e * 2.50662827463100050241576528481 * std::exp(0.5 * x * x);
    x -= u / (1.0 + 0.5 * x * u);
    result[i] = (tail ? -x : x);
  } // i
  return result;
}

/* Random number generation
   We use the improved ziggurat method, see:
   Doornik, J.A. (2005), "An Improved Ziggurat Method to Generate Normal
   Random Samples", mimeo, Nuffield College, University of Oxford,
   and www.doornik.com/research.
*/
Scalar DistFunc::rNormal()
{
  for (;;)
  {
    const Scalar u = 2.0 * RandomGenerator::Generate() - 1.0;
    const UnsignedInteger index = RandomGenerator::IntegerGenerate(DistFunc::NumberOfBandNormalZigurrat);
    /* Are we in a rectangular band of the ziggurat? */
    if (std::abs(u) < DistFunc::NormalZigguratRatio[index]) return u * NormalZigguratAbscissa[index + 1];
    /* No, we are either on a wedge or in the upper tail of the Normal distribution */
    /* Are we in the bottom band? Sample from the tail of the Normal distribution */
    if (index == DistFunc::NumberOfBandNormalZigurrat - 1)
    {
      Scalar x = -1.0;
      Scalar y = -1.0;
      /* Marsaglia method */
      do
      {
        x = std::log(RandomGenerator::Generate()) / DistFunc::NormalZigguratTail;
        y = std::log(RandomGenerator::Generate());
      }
      while (-(y + y) < x * x);
      return (u > 0.0) ? x - DistFunc::NormalZigguratTail : DistFunc::NormalZigguratTail - x;
    }
    /* Are we in the wedges? Basic rejection method */
    const Scalar xI = NormalZigguratAbscissa[index];
    const Scalar xIp1 = NormalZigguratAbscissa[index + 1];
    const Scalar x = u * xIp1;
    const Scalar pdfX = std::exp(-0.5 * x * x);
    const Scalar pdfI = std::exp(-0.5 * xI * xI);
    const Scalar pdfIp1 = std::exp(-0.5 * xIp1 * xIp1);
    if (RandomGenerator::Generate() * (pdfI - pdfIp1) < pdfX - pdfIp1) return x;
  }
}

Point DistFunc::rNormal(const UnsignedInteger size)
{
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = rNormal();
  return result;
}

Scalar DistFunc::dNormal(const Scalar x)
{
  return std::exp(-0.5 * x * x) * SpecFunc::ISQRT2PI;
}

Point DistFunc::dNormal(const Point &x)
{
  const UnsignedInteger size = x.getSize();
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = dNormal(x[i]);
  return result;
}

Scalar DistFunc::logdNormal(const Scalar x)
{
  return -0.5 * x * x - SpecFunc::LOGSQRT2PI;
}

/**********************************************************************************/
/* Poisson distribution, i.e. with a PDF equals to exp(-lambda) . lambda ^ k / k! */
/**********************************************************************************/
/* Probability distribution function
   We use the algorithm described in:
   Catherine Loader, "Fast and Accurate Computation of Binomial Probabilities", On-
line:   https://lists.gnu.org/archive/html/octave-maintainers/2011-09/pdfK0uKOST642.pdf, accessed 22 December 2018, 2000.
   with the improvements discussed in:
   Jannis Dimitriadis, "On the Accuracy of Loader’s Algorithm for the Binomial
   Density and Algorithms for Rectangle Probabilities for Markov Increments",
   Dissertation, Universitat Trier, August 2016.
*/
Scalar DistFunc::logdPoisson(const Scalar lambda,
                             const UnsignedInteger k)
{
  if (lambda == 0.0) return (k == 0 ? 0.0 : SpecFunc::LowestScalar);
  if (k == 0) return -lambda;
  return -SpecFunc::Stirlerr(k) - bd0(k, lambda) - 0.5 * std::log(2.0 * M_PI * k);
}

Scalar DistFunc::dPoisson(const Scalar lambda,
                          const UnsignedInteger k)
{
  if (lambda == 0.0) return (k == 0 ? 1.0 : 0.0);
  if (k == 0) return std::exp(-lambda);
  return std::exp(-SpecFunc::Stirlerr(k) - bd0(k, lambda)) / std::sqrt(2.0 * M_PI * k);
}

/* Quantile function
   We use the algorithm described in:
   Mikes Giles, "Fast evaluation of the inverse Poisson cumulative distribution function", https://people.maths.ox.ac.uk/gilesm/poissinv/paper.pdf
   It is the Author's implementation, used in OpenTURNS with his written permission, see COPYING.poissinv
*/
Scalar DistFunc::qPoisson(const Scalar lambda,
                          const Scalar p,
                          const Bool tail)
{
  Scalar r = tail ? 1.0 - p : p;
  if (r <= SpecFunc::MinScalar) r = SpecFunc::MinScalar;
  if (r >= 1.0 - SpecFunc::ScalarEpsilon) r = 1.0 - SpecFunc::ScalarEpsilon;
  // We use the scalar version
  return poissinv_scalar(r, lambda);
}
/* Random number generation
   For the small values of lambda, we use the method of inversion by sequential search described in:
   Luc Devroye, "Non-Uniform RandomVariate Generation", Springer-Verlag, 1986, available online at:
   http://cg.scs.carleton.ca/~luc/nonuniformrandomvariates.zip
   and with the important errata at:
   http://www.nrbook.com/devroye/Devroye_files/errors.pdf
   For the large values of lambda, we use the ratio of uniform method described in:
   E. Stadlober, "The ratio of uniforms approach for generating discrete random variates". Journal of Computational and Applied Mathematics, vol. 31, no. 1, 1990, pp. 181-189.
*/
UnsignedInteger DistFunc::rPoisson(const Scalar lambda)
{
  Scalar mu = floor(lambda);
  // Small case. The bound 6 is quite arbitrary, but must be < 80 to avoid overflow.
  if (mu < 6)
  {
    UnsignedInteger x = 0;
    Scalar sum = std::exp(-lambda);
    Scalar prod = sum;
    const Scalar u = RandomGenerator::Generate();
    for (;;)
    {
      if (u <= sum) return x;
      ++x;
      prod *= lambda / x;
      sum += prod;
    }
  } // Small case
  // Large case
  const Scalar hatCenter = lambda + 0.5;
  const Scalar mode = floor(lambda);
  const Scalar logLambda = std::log(lambda);
  const Scalar pdfMode = mode * logLambda - SpecFunc::LnGamma(mode + 1.0);
  // 2.943035529371538572764190 = 8 / e
  // 0.898916162058898740826254 = 3 - 2 sqr(3 / e)
  const Scalar hatWidth = std::sqrt(2.943035529371538572764190 * (lambda + 0.5)) + 0.898916162058898740826254;
  const Scalar safetyBound = hatCenter + 6.0 * hatWidth;
  for (;;)
  {
    const Scalar u = RandomGenerator::Generate();
    const Scalar x = hatCenter + hatWidth * (RandomGenerator::Generate() - 0.5) / u;
    if (x < 0 || x >= safetyBound) continue;
    const UnsignedInteger k = static_cast< UnsignedInteger >(floor(x));
    const Scalar logPdf = k * logLambda - SpecFunc::LnGamma(k + 1.0) - pdfMode;
    // Quick acceptance
    if (logPdf >= u * (4.0 - u) - 3.0) return k;
    // Quick rejection
    if (u * (u - logPdf) > 1.0) continue;
    // Acceptance
    if (2.0 * std::log(u) <= logPdf) return k;
  }
}

Indices DistFunc::rPoisson(const Scalar lambda,
                           const UnsignedInteger size)
{
  Indices result(size);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = rPoisson(lambda);
  return result;
}

/********************************************************************************************/
/* Pearson rho asymptotic distribution: let X & Y be two univariate samples of size.        */
/* This function computes the probability that the Pearson correlation between the two      */
/* samples is less or equal (tail = false) or is greater (tail = true) than the given value */
/* under the assumption that samples are linearly independent.                                    */
/********************************************************************************************/

Scalar DistFunc::pPearsonCorrelation(const UnsignedInteger size, const Scalar rho, const Bool tail)
{
  if (size < 3) throw InvalidArgumentException(HERE) << "Error: in pPearsonCorrelation, size must be > 2";
  // Border values for rho
  if (rho >=  1.0) return (tail ? 0.0 : 1.0);
  if (rho < -1.0) return (tail ? 1.0 : 0.0);
  // Use a Student asymptotic approximation
  if (rho <= -1.0 + SpecFunc::Precision) return (tail ? 1.0 : 0.0);
  if (rho >=  1.0 - SpecFunc::Precision) return (tail ? 0.0 : 1.0);
  const Scalar rhoSquared = rho * rho;
  const Scalar t = std::abs(rho) * std::sqrt((size - 2.0) / (1.0 - rhoSquared));
  return pStudent(size - 2.0, t, tail);
}


/********************************************************************************************/
/* Spearman rho distribution: let (X_i) and (Y_i) be two samples of size n and dimension 1. */
/* This function computes the probability that the Spearman correlation between the samples */
/* is less or equal (tail = false) or is greater (tail = ture) than the given value under   */
/* the assumption that the permutation that reorder the first sample in the same order as   */
/* the second sample is uniformly distributed among the n-permutations. It is the case in   */
/* particular when the two samples are independent and iid.                                 */
/********************************************************************************************/
Scalar DistFunc::pSpearmanCorrelation(const UnsignedInteger size, const Scalar rho, const Bool tail, const Bool ties)
{
  if (size < 2) throw InvalidArgumentException(HERE) << "Error: in pSpearmanCorrelation, size must be > 1";
  // Border values for rho
  if (rho >=  1.0) return (tail ? 0.0 : 1.0);
  if (rho < -1.0) return (tail ? 1.0 : 0.0);
  // If ties, the exact method or the Edgeworth expansion cannot be used.
  // Use a Student asymptotic approximation
  if (ties)
  {
    // In this case rho is seen as a continuous variable so the border cases
    // have to be a little bit fuzzier
    if (rho <= -1.0 + SpecFunc::Precision) return (tail ? 1.0 : 0.0);
    if (rho >=  1.0 - SpecFunc::Precision) return (tail ? 0.0 : 1.0);
    const Scalar t = rho * std::sqrt((size - 2.0) / (1.0 - rho * rho));
    return pStudent(size - 2.0, t, tail);
  }
  // Exact computation for small sample size thanks to the values given
  // in http://www.luke-g.com/math/spearman/index.html
  // The case n==2 has been added to the table
  if (size <= 26)
  {
    // Include the tabulated exact distribution
#include "pSpearmanCorrelation.hxx"
    const UnsignedInteger start = shiftSpearman[size - 2];
    const UnsignedInteger stop = shiftSpearman[size - 1];
    for (UnsignedInteger i = start + 1; i < stop; ++i)
      if (rho < rhoSpearman[i])
        return (tail ? 1.0 - cdfSpearman[i - 1] : cdfSpearman[i - 1]);
  } // size <= 26
  // Edgeworth expansion for the other sample sizes
  // Coefficients from AS89. More digits would be desirable...
  static const Scalar c[12] = {0.2274, 0.2531, 0.1745, 0.0758, 0.1033, 0.3932, 0.0879, 0.0151, 0.0072, 0.0831, 0.0131, 4.6e-4};
  const Scalar y = 1.0 * size;
  const Scalar b = 1.0 / y;
  const Scalar x = (rho + 6.0 / (y * (y + 1.0) * (y - 1.0))) * std::sqrt(y - 1);
  const Scalar x2 = x * x;
  const Scalar u = x * b * (c[0] + b * (c[1] + c[2] * b) + x2 * (-c[3] + b * (c[4] + c[5] * b) - x2 * b * (c[6] + c[7] * b - x2 * (c[8] - c[9] * b + x2 * b * (c[10] - c[11] * x2)))));
  const Scalar beta = u / std::exp(0.5 * x2);
  const Scalar value = (tail ? beta : -beta) + pNormal(x, tail);
  return (value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value));
}
/********************************************************************************************************************************/
/* Normalized Student distribution, i.e. with a PDF equals to (1 + x^2 / nu)^(-(1 + nu) / 2) / (sqrt(nu) . Beta(1 / 2, nu / 2)) */
/********************************************************************************************************************************/
/* CDF */
Scalar DistFunc::pStudent(const Scalar nu,
                          const Scalar x,
                          const Bool tail)
{
  return StudentFunctions::StudentCDF(nu, x, tail);
}
Point DistFunc::pStudent(const Scalar nu,
                         const Point & x,
                         const Bool tail)
{
  const UnsignedInteger size = x.getSize();
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = StudentFunctions::StudentCDF(nu, x[i], tail);
  return result;
}
/* CDF inverse */
Scalar DistFunc::qStudent(const Scalar nu,
                          const Scalar p,
                          const Bool tail)
{
  return StudentFunctions::StudentQuantile(nu, p, tail);
}
Point DistFunc::qStudent(const Scalar nu,
                         const Point & p,
                         const Bool tail)
{
  const UnsignedInteger size = p.getSize();
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i)
    result[i] = qStudent(nu, p[i], tail);
  return result;
}
/* Random number generation
   We use a transformation method based on Gamma and Normal transformation:
   If N is Normal(0, 1) distributed and G is Gamma(nu / 2) distributed,
   sqrt(2 * nu) * N / sqrt(G) is distributed according to Student(nu)
*/
Scalar DistFunc::rStudent(const Scalar nu)
{
  return StudentFunctions::StudentRealization(nu);
}

Point DistFunc::rStudent(const Scalar nu,
                         const UnsignedInteger size)
{
  Point result(size);
  for (UnsignedInteger i = 0; i < size; ++i) result[i] = StudentFunctions::StudentRealization(nu);
  return result;
}

/* Compute the expectation of the min of n independent standard normal random variables. Useful for the modified moment estimator of the LogNormal distribution. */
Scalar DistFunc::eZ1(const UnsignedInteger n)
{
  if (!(n > 0)) throw InvalidArgumentException(HERE) << "Error: n must be strictly positive.";

  static Scalar nodes[128] = {9.8079096926749782026033390e-02, 2.9424096921218469797991561e-01, 4.9041387858718514746809250e-01, 6.8660518909321215660380258e-01, 8.8282227375329345793892660e-01, 1.0790725181825577535496122e+00, 1.2753633242167377696353796e+00, 1.4717021135638638145739759e+00,
                              1.6680963314844359561817086e+00, 1.8645534505054447383309383e+00, 2.0610809741737079558017920e+00, 2.2576864408541048712667240e+00, 2.4543774275784199432866622e+00, 2.6511615539506562785567740e+00, 2.8480464861148453552909053e+00, 3.0450399407915649134290777e+00,
                              3.2421496893895821993047513e+00, 3.4393835621992660247489466e+00, 3.6367494526746595077587933e+00, 3.8342553218113771844054655e+00, 4.0319092026277868364264334e+00, 4.2297192047572594302912378e+00, 4.4276935191596217359545187e+00, 4.6258404229603273857896694e+00,
                              4.8241682844262754351411023e+00, 5.0226855680876531919920886e+00, 5.2214008400156647181628365e+00, 5.4203227732665307412164732e+00, 5.6194601535027128016005275e+00, 5.8188218848029276413744931e+00, 6.0184169956731807988206196e+00, 6.2182546452717651529731537e+00,
                              6.4183441298619452136612962e+00, 6.6186948895068861733967826e+00, 6.8193165150222935201989526e+00, 7.0202187552032102976570365e+00, 7.2214115243424814429173638e+00, 7.4229049100595452662318140e+00, 7.6247091814594590441665212e+00, 7.8268347976434177117924059e+00,
                              8.0292924165934915265672000e+00, 8.2320929044559011583234267e+00, 8.4352473452488789318099250e+00, 8.6387670510230462242060088e+00, 8.8426635725042840919313109e+00, 9.0469487102513035219338200e+00, 9.2516345263625515942763530e+00, 9.4567333567697407325779683e+00,
                              9.6622578241581829095747019e+00, 9.8682208515572746537567071e+00, 1.0074635676647940494041886e+01, 1.0281515866837634049248891e+01, 1.0488875335157653191251042e+01, 1.0696728357042088900345860e+01, 1.0905089588052741961530773e+01, 1.1113974082619858662822321e+01,
                              1.1323397313874613884362823e+01, 1.1533375194655972686401645e+01, 1.1743924099781963641404176e+01, 1.1955060889683582572281695e+01, 1.2166802935508609351446676e+01, 1.2379168145812671465813240e+01, 1.2592174994966049877127273e+01, 1.2805842553417136659329113e+01,
                              1.3020190519967281793289524e+01, 1.3235239256227193956928362e+01, 1.3451009823442300506862860e+01, 1.3667524021893770835767368e+01, 1.3884804433103548556990800e+01, 1.4102874465096049669738234e+01, 1.4321758400996546516308293e+01, 1.4541481451277113257067713e+01,
                              1.4762069809995873134112522e+01, 1.4983550715414763026069010e+01, 1.5205952515425821882539839e+01, 1.5429304738266944762150950e+01, 1.5653638169066098815394345e+01, 1.5878984932819323859862773e+01, 1.6105378584483803337592983e+01, 1.6332854206954513949199624e+01,
                              1.6561448517793378857450989e+01, 1.6791199985695774874822891e+01, 1.7022148957813457866272315e+01, 1.7254337799208822083094216e+01, 1.7487811045896949980107817e+01, 1.7722615573144060013318108e+01, 1.7958800780939722006343882e+01, 1.8196418798852933084511311e+01,
                              1.8435524712827875258644471e+01, 1.8676176816885087724358203e+01, 1.8918436893181770856317469e+01, 1.9162370524468337507669120e+01, 1.9408047443678915527648282e+01, 1.9655541926238779717773660e+01, 1.9904933231696581852007900e+01, 2.0156306102538408960103184e+01,
                              2.0409751329571602386146602e+01, 2.0665366395153397356064835e+01, 2.0923256207880053036728538e+01, 2.1183533945274238266112570e+01, 2.1446322024681900006387514e+01, 2.1711753227242152331884139e+01, 2.1979972005732402979093282e+01, 2.2251136014735937033989485e+01,
                              2.2525417911510261410205347e+01, 2.2803007488961426903566920e+01, 2.3084114219397521301316111e+01, 2.3368970310885331538480179e+01, 2.3657834409456066774035863e+01, 2.3950996123628848479052048e+01, 2.4248781608052614781871560e+01, 2.4551560528643458715087857e+01,
                              2.4859754855145844760826474e+01, 2.5173850108958237208064469e+01, 2.5494409967825708622699690e+01, 2.5822095551225733525384449e+01, 2.6157691379788251433263128e+01, 2.6502141097849996836684948e+01, 2.6856597908134813373229728e+01, 2.7222497961270095541841192e+01,
                              2.7601671075507827784986288e+01, 2.7996515289006133611177657e+01, 2.8410287565216026379953973e+01, 2.8847623300317097284912261e+01, 2.9315556495648897392258304e+01, 2.9825809458081402413142885e+01, 3.0401117779657764294819147e+01, 3.1100951037096511748801284e+01
                             };

  static Scalar weights[128] = {7.7880553112849072198766294e-02, 7.4943512919414536910806816e-02, 6.9397141749149403969696237e-02, 6.1836746879648318333389484e-02, 5.3020239572973708333178304e-02, 4.3743820721247411296800373e-02, 3.4726352391828770445416924e-02, 2.6524913163345550162895103e-02,
                                1.9493216056892157244261750e-02, 1.3782486012401120501189296e-02, 9.3748300171492664629503913e-03, 6.1343092263553692729419654e-03, 3.8610584455446167914723509e-03, 2.3375177719222513086212317e-03, 1.3610623554229154160050607e-03, 7.6214868374154758836225128e-04,
                                4.1039424914329114912560963e-04, 2.1248156236438935169486303e-04, 1.0576827089812413493907134e-04, 5.0612366034536501431668631e-05, 2.3279572286335517177399318e-05, 1.0291000375452160655354603e-05, 4.3716822258132265765633340e-06, 1.7843872447157496838756722e-06,
                                6.9970863752445993319645415e-07, 2.6355296294457982485801659e-07, 9.5339500076719088603559665e-08, 3.3117736286643680141438096e-08, 1.1044763292458927772742952e-08, 3.5357534962409985484084265e-09, 1.0863147662329805097318140e-09, 3.2025234569583239758235623e-10,
                                9.0573898119730292539244887e-11, 2.4569445682930553563482259e-11, 6.3910595568724711910845065e-12, 1.5938054964152907343039923e-12, 3.8095919232928330906238773e-13, 8.7255716548816521833151253e-14, 1.9145600624729613090422991e-14, 4.0233405909344399451473717e-15,
                                8.0951675009613752719948473e-16, 1.5590499798725125935141981e-16, 2.8731516823222887474066625e-17, 5.0650721565888260826310931e-18, 8.5388712150794321468581963e-19, 1.3761221211258663377847429e-19, 2.1193549695588593255142755e-20, 3.1180535562838082396978111e-21,
                                4.3806011311093044092563723e-22, 5.8746940037473806331850174e-23, 7.5173209054033447555417709e-24, 9.1745690698535897008652337e-25, 1.0674922835552105657158543e-25, 1.1836003268604515384321003e-26, 1.2499832905811924753397716e-27, 1.2567593992729679307393964e-28,
                                1.2023503699854981391237349e-29, 1.0939938866854915092942988e-30, 9.4616999410627741962246371e-32, 7.7741057498528684818055696e-33, 6.0646535975418060617176232e-34, 4.4892461173993180351676461e-35, 3.1512300703857921867014789e-36, 2.0962490291314412221073794e-37,
                                1.3205879942102419576039463e-38, 7.8731309724410339834852054e-40, 4.4387936139837483446171576e-41, 2.3647681386310266830599492e-42, 1.1895249269031468696051071e-43, 5.6449598528648601040867566e-45, 2.5250874009530786161544321e-46, 1.0637272217126616433515017e-47,
                                4.2161469063242967027862854e-49, 1.5707547975689386981401980e-50, 5.4949672861369168167697578e-52, 1.8031127826300551915850409e-53, 5.5436741208780826872988904e-55, 1.5950823588115267095636623e-56, 4.2899178785744838761108310e-58, 1.0770598014915502881777703e-59,
                                2.5210019210640023596252402e-61, 5.4933568192556171804844677e-63, 1.1127348357056033046953705e-64, 2.0919910037640666023889327e-66, 3.6444488891998227750903798e-68, 5.8729939242428208168494011e-70, 8.7388392883152991177346810e-72, 1.1983435235137524938450365e-73,
                                1.5113381294896973696530902e-75, 1.7492932046071090072619502e-77, 1.8539433227865165645693191e-79, 1.7947949751038629650034721e-81, 1.5830825023894390544815014e-83, 1.2687510116986886750192806e-85, 9.2123101777701364185032765e-88, 6.0412782427518176337230762e-90,
                                3.5662449072374121768733826e-92, 1.8882726818582926654010717e-94, 8.9335232334505241260528038e-97, 3.7608844095106074694647086e-99, 1.4025800177880556273945875e-101, 4.6114419052210857911207097e-104, 1.3296527917278135803395156e-106, 3.3431060612647631418663216e-109,
                                7.2838408325905858803792272e-112, 1.3658255130442080104321089e-114, 2.1876645330156469909347131e-117, 2.9682322505121909735883514e-120, 3.3800825692122852526903147e-123, 3.1972924782982543066350585e-126, 2.4832856265123329861700072e-129, 1.5630453427246388490165028e-132,
                                7.8550152086622435117816579e-136, 3.0983962164454199109222893e-139, 9.4051577447557691199185888e-143, 2.1469376259019048933868626e-146, 3.5864591066294724118391333e-150, 4.2436752264252758983575152e-154, 3.4184301509636211134786682e-158, 1.7847924896743181968557422e-162,
                                5.6754752717640951349146435e-167, 1.0136208941909344983031939e-171, 9.1144573618340727008462260e-177, 3.5359397081504360617642767e-182, 4.6913583190766486697804951e-188, 1.4509334500270392274530570e-194, 5.0208177568884358829526795e-202, 2.9540145871800834681710394e-211
                               };

  Scalar value = 0.0;
  // Least square approximation of eZ1
  const Scalar z0 = -1.5270815222604243733 - 0.25091814704012410653 * std::log(static_cast<double>(n));
  for (UnsignedInteger i = 0; i < 128; ++i)
    value += weights[i] * ((z0 + nodes[i]) * std::exp(-z0 * nodes[i]) * std::pow(pNormal(z0 + nodes[i], true), static_cast<int>(n - 1)) + (z0 - nodes[i]) * std::exp(z0 * nodes[i]) * std::pow(pNormal(z0 - nodes[i], true), static_cast<int>(n - 1)));
  return n * exp(-0.5 * z0 * z0) * value;
}


/* Dickey Fuller asymptotic table
   The use of such table allow one to make a linear approximation
*/
Scalar DistFunc::pDickeyFullerTrend(const Scalar x,
                                    const Bool tail)
{
  // Quantiles and values available
  const Scalar q001 = -3.96;
  const Scalar q005 = -3.41;
  const Scalar q010 = -3.13;
  const Scalar p001 = 0.01;
  const Scalar p005 = 0.05;
  const Scalar p010 = 0.10;
  if (x < q001)
  {
    LOGWARN(OSS() <<  "Warning! Result p-value is missing. The return result is the 0.01 quantile level ");
    return (tail ? 1.0 - p001 : p001);
  }

  if (x <= q005)
  {
    // Linear approach
    // quantileFunction(q) = a * q + b
    // We use information available : quantile values for levels 0.01 and 0.05
    const Scalar a = (p005 - p001) / (q005 - q001);
    const Scalar b = p001 - a * q001;
    return (tail ? 1.0 - (a * x + b) : a * x + b);
  }

  if (x <= q010)
  {
    // Linear approach using the same approach
    // quantileFunction(q) = a * q + b
    // We use information available : quantile values for levels 0.05 and 0.10
    const Scalar a = (p010 - p005) / (q010 - q005);
    const Scalar b = p005 - a * q005;
    return (tail ? 1.0 - (a * x + b) : a * x + b);
  }

  // Arbitrary we seek values until level 0.15
  LOGWARN(OSS() <<  "Warning! Result p-value is missing. The return result is the 0.10 quantile levels ");
  return (tail ? 1.0 - p010 : p010);
}

Scalar DistFunc::pDickeyFullerConstant(const Scalar x,
                                       const Bool tail)
{
  // Quantiles and values available
  const Scalar q001 = -3.43;
  const Scalar q005 = -2.86;
  const Scalar q010 = -2.57;
  const Scalar p001 = 0.01;
  const Scalar p005 = 0.05;
  const Scalar p010 = 0.10;
  if (x < q001)
  {
    LOGWARN(OSS() <<  "Warning! Result p-value is missing. The return result is the 0.01 quantile level ");
    return (tail ? 1.0 - p001 : p001);
  }

  if (x <= q005)
  {
    // Linear approach
    // quantileFunction(q) = a * q + b
    // We use information available : quantile values for levels 0.01 and 0.05
    const Scalar a = (p005 - p001) / (q005 - q001);
    const Scalar b = p001 - a * q001;
    return (tail ? 1.0 - (a * x + b) : a * x + b);
  }

  if (x <= q010)
  {
    // Linear approach using the same approach
    // quantileFunction(q) = a * q + b
    // We use information available : quantile values for levels 0.05 and 0.10
    const Scalar a = (p010 - p005) / (q010 - q005);
    const Scalar b = p005 - a * q005;
    return (tail ? 1.0 - (a * x + b) : a * x + b);
  }
  LOGWARN(OSS() <<  "Warning! Result p-value is missing. The return result is the 0.10 quantile levels ");
  return (tail ? 1.0 - p010 : p010);
}

Scalar DistFunc::pDickeyFullerNoConstant(const Scalar x,
    const Bool tail)
{
  // Quantiles and values available
  const Scalar q001 = -2.57;
  const Scalar q005 = -1.94;
  const Scalar q010 = -1.62;
  const Scalar p001 = 0.01;
  const Scalar p005 = 0.05;
  const Scalar p010 = 0.10;
  if (x < q001)
  {
    LOGWARN(OSS() <<  "Warning! Result p-value is missing. The return result is the 0.01 quantile level ");
    return (tail ? 1.0 - p001 : p001);
  }

  if (x <= q005)
  {
    // Linear approach
    // quantileFunction(q) = a * q + b
    // We use information available : quantile values for levels 0.01 and 0.05
    const Scalar a = (p005 - p001) / (q005 - q001);
    const Scalar b = p001 - a * q001;
    return (tail ? 1.0 - (a * x + b) : a * x + b);
  }

  if (x <= q010)
  {
    // Linear approach using the same approach
    // quantileFunction(q) = a * q + b
    // We use information available : quantile values for levels 0.05 and 0.10
    const Scalar a = (p010 - p005) / (q010 - q005);
    const Scalar b = p005 - a * q005;
    return (tail ? 1.0 - (a * x + b) : a * x + b);
  }

  LOGWARN(OSS() <<  "Warning! Result p-value is missing. The return result is the 0.10 quantile levels ");
  return (tail ? 1.0 - p010 : p010);
}

// We currently use the asymptotic distribution
// We get the values for levels 0.01, 0.05 and 0.10
// If level is lower than 0.01 we return the 0.01 quantile value and we inform user
// by giving a warning message
// if the quantile level is between 0.01 and 0.05, we use a linear interpolation
// The same result is done if the level seeked is between 0.05 and 0.10
// Finally if the leval is upper than 0.10 we return the 0.10-quantile value with a warning message
Scalar DistFunc::qDickeyFullerTrend(const Scalar p,
                                    const Bool tail)
{
  const Scalar prob = (tail ? 1.0 - p : p);
  // Asymptotic distribution
  // quantile values for levels 0.01, 0.05 and 0.10
  const Scalar x001 = -4.96;
  const Scalar x005 = -3.41;
  const Scalar x010 = -3.13;
  if (prob < 0.01)
  {
    LOGWARN(OSS() <<  "Warning! Result quantile value is missing. The return result is for level 0.01 ");
    return x001;
  }

  if (prob <= 0.05)
  {
    // Linear approach
    // quantileFunction(p) = a * p + b
    // We use information available : quantile values for levels 0.01 and 0.05
    const Scalar a = (x005 - x001) / (0.05 - 0.01);
    const Scalar b = x001 - a * 0.01;
    return (a * prob + b);
  }

  if (prob <= 0.10)
  {
    // Linear approach using the same approach
    // quantileFunction(p) = a * p + b
    // We use information available : quantile values for levels 0.05 and 0.10
    const Scalar a = (x010 - x005) / (0.10 - 0.05);
    const Scalar b = x005 - a * 0.05;
    return (a * prob + b);
  }

  // Arbitrary we seek values until level 0.15
  if (prob <= 0.15)
  {
    LOGWARN(OSS() <<  "Warning! Result quantile value is missing. The return result is for level 0.10 ");
    return x010;
  }
  throw NotYetImplementedException(HERE) << "In DistFunc::pDickeyFullerNoConstant(const Scalar x, const Bool tail): cannot give quantile value for the level " << p << ". Value is missing in table";
}

Scalar DistFunc::qDickeyFullerConstant(const Scalar p,
                                       const Bool tail)
{
  const Scalar prob = (tail ? 1.0 - p : p);
  // Asymptotic distribution
  // quantile values for levels 0.01, 0.05 and 0.10
  const Scalar x001 = -3.43;
  const Scalar x005 = -2.86;
  const Scalar x010 = -2.57;
  if (prob < 0.01)
  {
    LOGWARN(OSS() <<  "Warning! Result quantile value is missing. The return result is for level 0.01 ");
    return x001;
  }

  if (prob <= 0.05)
  {
    // Linear approach
    // quantileFunction(p) = a * p + b
    // We use information available : quantile values for levels 0.01 and 0.05
    const Scalar a = (x005 - x001) / (0.05 - 0.01);
    const Scalar b = x001 - a * 0.01;
    return (a * prob + b);
  }

  if (prob <= 0.10)
  {
    // Linear approach using the same approach
    // quantileFunction(p) = a * p + b
    // We use information available : quantile values for levels 0.05 and 0.10
    const Scalar a = (x010 - x005) / (0.10 - 0.05);
    const Scalar b = x005 - a * 0.05;
    return (a * prob + b);
  }

  // Arbitrary we seek values until level 0.15
  if (prob <= 0.15)
  {
    LOGWARN(OSS() <<  "Warning! Result quantile value is missing. The return result is for level 0.10 ");
    return x010;
  }

  throw NotYetImplementedException(HERE) << "In DistFunc::qDickeyFullerConstant(const Scalar p, const Bool tail): cannot give quantile value for the level " << p << ". Value is missing in table";
}

Scalar DistFunc::qDickeyFullerNoConstant(const Scalar p,
    const Bool tail)
{
  const Scalar prob = (tail ? 1.0 - p : p);
  // Asymptotic distribution
  // quantile values for levels 0.01, 0.05 and 0.10
  const Scalar x001 = -2.57;
  const Scalar x005 = -1.94;
  const Scalar x010 = -1.62;
  if (prob < 0.01)
  {
    LOGWARN(OSS() <<  "Warning! Result quantile value is missing. The return result is for level 0.01 ");
    return x001;
  }

  if (prob <= 0.05)
  {
    // Linear approach
    // quantileFunction(p) = a * p + b
    // We use information available : quantile values for levels 0.01 and 0.05
    const Scalar a = (x005 - x001) / (0.05 - 0.01);
    const Scalar b = x001 - a * 0.01;
    return (a * prob + b);
  }

  if (prob <= 0.10)
  {
    // Linear approach using the same approach
    // quantileFunction(p) = a * p + b
    // We use information available : quantile values for levels 0.05 and 0.10
    const Scalar a = (x010 - x005) / (0.10 - 0.05);
    const Scalar b = x005 - a * 0.05;
    return (a * prob + b);
  }

  // Arbitrary we seek values until level 0.15
  if (prob <= 0.15)
  {
    LOGWARN(OSS() <<  "Warning! Result quantile value is missing. The return result is for level 0.10 ");
    return x010;
  }
  throw NotYetImplementedException(HERE) << "In DistFunc::qDickeyFullerNoConstant(const Scalar p, const Bool tail): cannot give quantile value for the level " << p << ". Value is missing in table";
}

Point DistFunc::rUniformTriangle(const Point & a,
                                 const Point & b,
                                 const Point & c)
{
  const UnsignedInteger dimension = a.getDimension();
  if (b.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the second point has a dimension=" << b.getDimension() << ", expected dimension=" << dimension;
  if (c.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the third point has a dimension=" << c.getDimension() << ", expected dimension=" << dimension;
  const Scalar u = RandomGenerator::Generate();
  const Scalar v = RandomGenerator::Generate();
  const Scalar sqrtU = std::sqrt(u);
  Scalar x = 1.0 - sqrtU;
  Scalar y = v * sqrtU;
  Scalar z = 1.0 - x - y;
  if (z < 0.0)
  {
    x = sqrtU;
    y = 1.0 - sqrtU;
    z = -z;
  }
  Point result(dimension);
  for (UnsignedInteger i = 0; i < dimension; ++i) result[i] = x * a[i] + y * b[i] + z * c[i];
  return result;
}

Sample DistFunc::rUniformTriangle(const Point & a,
                                  const Point & b,
                                  const Point & c,
                                  const UnsignedInteger size)
{
  // Here we reproduce the algorithm in order to avoid costly data copy and tests
  const UnsignedInteger dimension = a.getDimension();
  if (b.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the second point has a dimension=" << b.getDimension() << ", expected dimension=" << dimension;
  if (c.getDimension() != dimension) throw InvalidArgumentException(HERE) << "Error: the third point has a dimension=" << c.getDimension() << ", expected dimension=" << dimension;
  Sample result(size, dimension);
  for (UnsignedInteger n = 0; n < size; ++n)
  {
    const Scalar u = RandomGenerator::Generate();
    const Scalar v = RandomGenerator::Generate();
    const Scalar sqrtU = std::sqrt(u);
    Scalar x = 1.0 - sqrtU;
    Scalar y = v * sqrtU;
    Scalar z = 1.0 - x - y;
    if (z < 0.0)
    {
      x = sqrtU;
      y = 1.0 - sqrtU;
      z = -z;
    }
    for (UnsignedInteger i = 0; i < dimension; ++i) result(n, i) = x * a[i] + y * b[i] + z * c[i];
  } // n
  return result;
}

/* K factor for exact two-sided tolerance intervals of normal pooled populations
 * see Janiga, I. Miklos, R. "Statistical Tolerance Intervals for a Normal Distribution", Measurement Science Review, 2001.
 * Here we use \alpha instead of 1-\alpha wrt the reference
 */
Scalar DistFunc::kFactorPooled(const UnsignedInteger n,
                               const UnsignedInteger m,
                               const Scalar p,
                               const Scalar alpha)
{
  if (!(n >= 2)) throw InvalidArgumentException(HERE) << "Error: the population size n must be at least 2";
  if (m == 0) throw InvalidArgumentException(HERE) << "Error: the number m of pooled populations must be positive";
  return KFactorFunctions::KFactor(n, m * (n - 1), p, 1.0 - alpha);
}

/* K factor for exact two-sided tolerance intervals of normal populations
 * see Janiga, I. Miklos, R. "Statistical Tolerance Intervals for a Normal Distribution", Measurement Science Review, 2001.
 * Here we use \alpha instead of 1-\alpha wrt the reference
 */
Scalar DistFunc::kFactor(const UnsignedInteger n,
                         const Scalar p,
                         const Scalar alpha)
{
  if (!(n >= 2)) throw InvalidArgumentException(HERE) << "Error: the population size n must be at least 2";
  return KFactorFunctions::KFactor(n, n - 1, p, 1.0 - alpha);
}

END_NAMESPACE_OPENTURNS
