//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the incomplete
 *         regularized beta function and related functions
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

#include "openturns/BetaFunctions.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/SpecFunc.hxx"
#ifdef OPENTURNS_HAVE_BOOST
// boost::math::ibeta_inv can error in extended precision causing t_DistFunc_beta failure
#define BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
#include <boost/math/special_functions/beta.hpp>
#endif

BEGIN_NAMESPACE_OPENTURNS

namespace BetaFunctions
{

Scalar IncompleteBeta(const Scalar a,
                      const Scalar b,
                      const Scalar x,
                      const Bool tail)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be positive, here a=" << a;
  if (!(b > 0.0)) throw InvalidArgumentException(HERE) << "Error: b must be positive, here b=" << b;
  return RegularizedIncompleteBeta(a, b, x, tail) * SpecFunc::Beta(a, b);
}

Scalar IncompleteBetaInverse(const Scalar a,
                             const Scalar b,
                             const Scalar x,
                             const Bool tail)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be positive, here a=" << a;
  if (!(b > 0.0)) throw InvalidArgumentException(HERE) << "Error: b must be positive, here b=" << b;
  return (tail ? RegularizedIncompleteBetaInverse(a, b, 1.0 - x * SpecFunc::Beta(a, b)) : RegularizedIncompleteBetaInverse(a, b, x * SpecFunc::Beta(a, b)));
}

Scalar RegularizedIncompleteBeta(const Scalar a,
                                 const Scalar b,
                                 const Scalar x,
                                 const Bool tail)
{
  if (!(a >= 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be nonnegative, here a=" << a;
  if (!(b >= 0.0)) throw InvalidArgumentException(HERE) << "Error: b must be nonnegative, here b=" << b;
  if ((a <= 0.0) && (b <= 0.0)) throw InvalidArgumentException(HERE) << "Error: a and b cannot be null at the same time";
  if (x <= 0.0) return (tail ? 1.0 : 0.0);
  if (x >= 1.0) return (tail ? 0.0 : 1.0);
#ifdef OPENTURNS_HAVE_BOOST
  return (tail ? boost::math::ibetac(a, b, x) : boost::math::ibeta(a, b, x));
#else
  if (tail) return RegularizedIncompleteBetaQ(a, b, x);
  return RegularizedIncompleteBetaP(a, b, x);
#endif
}

Scalar RegularizedIncompleteBetaSlatec(const Scalar a,
                                       const Scalar b,
                                       const Scalar x,
                                       const Bool tail)
{
  Scalar p = a;
  Scalar q = b;
  Scalar y = x;
  Bool isSwapped = false;
  if (((q <= p) && (x >= 0.8)) || ((q > p) && (x >= 0.2)))
  {
    p = b;
    q = a;
    y = 0.5 + (0.5 - x);
    isSwapped = true;
  }
  if ((p + q) * y < SpecFunc::ScalarEpsilon * (p + 1.0)) return (tail == isSwapped ? 1.0 : 0.0);
  const Scalar qFrac = std::max(1.0, q - floor(q));
  Scalar xBeta = p * log(y) - SpecFunc::LogBeta(qFrac, p) - log(p);
  Scalar value = exp(xBeta);
  Scalar term = p * value;
  if (qFrac < 1.0)
  {
    const UnsignedInteger n = static_cast<UnsignedInteger>(std::max(4.0, round(log(SpecFunc::ScalarEpsilon) / log(y))));
    for (UnsignedInteger i = 1; i <= n; ++i)
    {
      term *= (i - qFrac) * y / i;
      value += term / (p + i);
    }
  } // bFrac < 1.0
  if (q <= 1.0) return (tail == isSwapped ? value : 0.5 + (0.5 - value));
  xBeta = p * log(y) + q * log1p(-y) - SpecFunc::LogBeta(p, q) - log(q);
  UnsignedInteger iBeta = static_cast<UnsignedInteger>(std::max(0.0, xBeta / SpecFunc::LogMinScalar));
  term = exp(xBeta - iBeta * SpecFunc::LogMinScalar);
  const Scalar c = 1.0 / (1.0 - y);
  const Bool smallCase = q * c <= (p + q - 1.0);
  Scalar finiteSum = 0.0;
  const UnsignedInteger n = static_cast<UnsignedInteger>(ceil(q));
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    if (smallCase && (term <= SpecFunc::ScalarEpsilon * finiteSum)) break;
    term *= (q - i) * c / (p + q - i - 1.0);
    if (term > 1.0)
    {
      --iBeta;
      term *= SpecFunc::MinScalar;
    } // term > 1.0
    if (iBeta == 0)
    {
      finiteSum += term;
      break;
    } // ib == 0
  } // for i
  value += finiteSum;
  return (tail == isSwapped ? value : 0.5 + (0.5 - value));
}

Scalar RegularizedIncompleteBetaInverse(const Scalar a,
                                        const Scalar b,
                                        const Scalar x,
                                        const Bool tail)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be positive, here a=" << a;
  if (!(b > 0.0)) throw InvalidArgumentException(HERE) << "Error: b must be positive, here b=" << b;
  if (x <= 0.0) return (tail ? 1.0 : 0.0);
  if (x >= 1.0) return (tail ? 0.0 : 1.0);
#ifdef OPENTURNS_HAVE_BOOST
  return (tail ? boost::math::ibetac_inv(a, b, x) : boost::math::ibeta_inv(a, b, x));
#else
  Scalar lower = 0.0;
  Scalar betaLower = tail ? 1.0 : 0.0;
  Scalar upper = 1.0;
  const Bool lowerLessThanX = betaLower < x;
  for (UnsignedInteger k = 0; k < SpecFunc::MaximumIteration; ++k)
  {
    if (std::abs(upper - lower) <= SpecFunc::Precision) break;
    const Scalar middle = 0.5 * (lower + upper);
    const Scalar betaMiddle = RegularizedIncompleteBeta(a, b, middle, tail);
    const Bool middleLessThanX = betaMiddle < x;
    if (middleLessThanX == lowerLessThanX) lower = middle;
    else upper = middle;
  } // Bisection loop
  return 0.5 * (upper + lower);
#endif
}

Scalar RegularizedIncompleteBetaP(const Scalar a,
                                  const Scalar b,
                                  const Scalar x)
{
  if (!(a >= 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be nonnegative, here a=" << a;
  if (!(b >= 0.0)) throw InvalidArgumentException(HERE) << "Error: b must be nonnegative, here b=" << b;
  if ((a <= 0.0) && (b <= 0.0)) throw InvalidArgumentException(HERE) << "Error: a and b cannot be null at the same time";
  if (x <= 0.0) return 0.0;
  if (x >= 1.0) return 1.0;
  if (a == 0.0) return 1.0;
  if (b == 0.0) return 0.0;
  if (x * (a + b + 2) < a + 1.0) return RegularizedIncompleteBetaContinuedFraction(a, b, x) * exp(lgamma(a + b) - lgamma(a) - lgamma(b) + a * log(x) + b * log1p(-x)) / a;
  return 1.0 - RegularizedIncompleteBetaContinuedFraction(b, a, 1.0 - x) * exp(lgamma(a + b) - lgamma(a) - lgamma(b) + a * log(x) + b * log1p(-x)) / b;
}

Scalar RegularizedIncompleteBetaQ(const Scalar a,
                                  const Scalar b,
                                  const Scalar x)
{
  if (!(a >= 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be nonnegative, here a=" << a;
  if (!(b >= 0.0)) throw InvalidArgumentException(HERE) << "Error: b must be nonnegative, here b=" << b;
  if ((a <= 0.0) && (b <= 0.0)) throw InvalidArgumentException(HERE) << "Error: a and b cannot be null at the same time";
  if (x >= 1.0) return 0.0;
  if (x <= 0.0) return 1.0;
  if (a == 0.0) return 0.0;
  if (b == 0.0) return 1.0;
  if (x * (a + b + 2) < a + 1.0) return 1.0 - RegularizedIncompleteBetaContinuedFraction(a, b, x) * exp(lgamma(a + b) - lgamma(a) - lgamma(b) + a * log(x) + b * log1p(-x)) / a;
  return RegularizedIncompleteBetaContinuedFraction(b, a, 1.0 - x) * exp(lgamma(a + b) - lgamma(a) - lgamma(b) + a * log(x) + b * log1p(-x)) / b;
}

// Based on [8.17(v)] of the NIST Digital Library of Mathematical Functions
// Implementation using the modified Lentz method as in Boost and the Numerical Recipes Ed.3
Scalar RegularizedIncompleteBetaContinuedFraction(const Scalar a,
    const Scalar b,
    const Scalar x)
{
  if (!(a >= 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be nonnegative, here a=" << a;
  if (!(b >= 0.0)) throw InvalidArgumentException(HERE) << "Error: b must be nonnegative, here b=" << b;
  if ((a <= 0.0) && (b <= 0.0)) throw InvalidArgumentException(HERE) << "Error: a and b cannot be null at the same time";
  if (x <= 0.0) return 1.0;
  const Scalar epsilon = SpecFunc::Precision * SpecFunc::Precision;
  const Scalar aPb = a + b;
  const Scalar aP1 = a + 1.0;
  const Scalar aM1 = a - 1.0;
  Scalar c = 1.0;
  Scalar d = 1.0 - aPb * x / aP1;
  if (std::abs(d) < epsilon) d = epsilon;
  Scalar value = c / d;
  for (UnsignedInteger k = 1; k <= SpecFunc::MaximumIteration; ++k)
  {
    const UnsignedInteger k2 = 2 * k;
    Scalar aK = k * (k - b) * x / ((aM1 + k2) * (a + k2));
    d = 1.0 - aK / d;
    if (std::abs(d) < epsilon) d = epsilon;
    c = 1.0 - aK / c;
    if (std::abs(c) < epsilon) c = epsilon;
    Scalar delta = c / d;
    value *= delta;
    if (std::abs(delta - 1) <= epsilon) break;
    aK = (a + k) * (aPb + k) * x / ((a + k2) * (aP1 + k2));
    d = 1.0 - aK / d;
    if (std::abs(d) < epsilon) d = epsilon;
    c = 1.0 - aK / c;
    if (std::abs(c) < epsilon) c = epsilon;
    delta = c / d;
    value *= delta;
    if (std::abs(delta - 1) <= epsilon) break;
  }
  return value;
}

} // BetaFunctions

END_NAMESPACE_OPENTURNS
