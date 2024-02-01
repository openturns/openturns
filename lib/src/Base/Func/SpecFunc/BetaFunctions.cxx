//                                               -*- C++ -*-
/**
 *  @brief Efficient implementation of the computation of the incomplete
 *         regularized beta function and related functions
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
//#define USE_NEW_ALGO
#ifdef USE_NEW_ALGO
// Computational subroutines
// FPSER
// APSER
// BPSER
// BFRAC
// BUP
// BGRAT
// BASYM
// Border cases
// a < 0.0 || b < 0.0
// a == 0.0 && b == 0.0
// x < 0.0 || x > 1.0
// y < 0.0 || y > 1.0
// x + y != 1
// x == 0.0 && a == 0.0
// y == 0.0 && b == 0.0
// Sub-cases
// a == 0.0 cdf = 1.0, ccdf = 0.0
// b == 0.0 cdf = 0.0, ccdf = 1.0
// max(a, b) < eps cdf = b / (a + b), ccdf = a / (a + b)
// a, b > 1:
//   a > b:
//     lambda = (a + b)y - b
//   a <= b:
//     lambda = a - (a + b)x
//   lambda >= 0.0:
//     b < 40.0:
//       bx <= 0.7 cdf = BPSER(a, b, x), ccdf = 1.0 - cdf
//       bx > 0.7:
//         frac(b) != 0:
//           x > 0.7:
//             a > 15:
//
Scalar BRATIO(const Scalar a,
              const Scalar b,
              const Scalar x,
              const Bool tail)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: a must be positive, here a=" << a;
  if (!(b > 0.0)) throw InvalidArgumentException(HERE) << "Error: b must be positive, here b=" << b;
  if (x <= 0.0) return (tail ? 1.0 : 0.0);
  if (x >= 1.0) return (tail ? 0.0 : 1.0);
  // For very small a, b
  if (std::max(a, b) < SpecFunc::ScalarEpsilon) return (tail ? a / (a + b) : b / (a + b));
  // Here we implement the decision tree as described in Section 3 of the reference
  Scalar newA = a;
  Scalar newB = b;
  Scalar newX = x;
  Scalar newY = 1.0 - x;
  const Bool minABGreater1 = std::min(a, b) > 1.0;
  if (minABGreater1)
  {
    const Scalar p = a / (a + b);
    const Bool swappedABXY = x > p;
    if (swappedABXY)
    {
      std::swap(newA, newB);
      std::swap(newX, newY);
    }
    if (newB < 40.0)
    {
      if (newB * newX <= 0.7)
      {
        const Scalar value = BPSER(newA, newB, newX);
        return (tail == swappedABXY ? value : value);
      }
      else if (x <= 0.7)
      {
        const UnsignedInteger n = static_cast<UnsignedInteger>(floor(newB));
        const Scalar value = BPSER(newA, newB - n, x) + BUP(newB - n, newA, newY, newX, n);
        return (tail == swappedABXY ? value : 1.0 - value);
      } // (newB * newX > 0.7) && (x <= 0.7)
      // Here we can have newB * newX <= 0.7
      if (newX > 0.7)
      {
        const UnsignedInteger n = static_cast<UnsignedInteger>(floor(newB));
        const Scalar bBar = newB - n;
        if (newA > 15.0)
        {
          const Scalar w0 = BUP(bBar, newA, newY, newX, n);
          const Scalar value = BGRAT(newA, bBar, newX, newY, w0);
          return (tail == swappedABXY ? value : 1.0 - value);
        } // newA > 15.0
        // Here, newA <= 15.0
        const Scalar w0 = BUP(bBar, newA, newY, newX, n) + BUP(newA, bBar, newX, newY, 20.0);
        const Scalar value = BGRAT(newA + 20.0, bBar, newX, newY, w0);
        return (tail == swappedABXY ? value : 1.0 - value);
      } // newX > 0.7
    } // newB < 40.0
    // Here, newB >= 40.0
    const Scalar q = b / (a + b);
    const Scalar lambda = newA - (newA + newB) * newX;
    if ( ((newA <= newB) && (newA <= 100.0)) ||
         ((100.0 < newA) && (newA <= newB) && (newX < 0.97 * p)) ||
         ((newA > newB) && (newB <= 100.0)) ||
         ((100.0 < newB) && (newB < newA) && (newY > 1.03 * q)) )
    {
      const Scalar value = BFRAC(newA, newB, newX, newY, lambda);
      return (tail == swappedABXY ? value : 1.0 - value);
    }
    // Here we are in the remaining cases of newB >= 40.0
    const Scalar value = BASYM(newA, newB, newX, newY, lambda);
    return (tail == swappedABXY ? value : 1.0 - value);
  } // minABGreater1
  // Here, min(a, b) <= 1.0
  const Bool swappedABXY = x > 0.5;
  if (swappedABXY)
  {
    std::swap(newA, newB);
    std::swap(newX, newY);
  }
  const Bool maxABGreater1 = std::max(newA, newB) > 1;
  if ((maxABGreater1 &&
       ((newB <= 1.0) || ((newX <= 0.1) && (pow(newB * newX, newA) <= 0.7)))) ||
      (!maxABGreater1 &&
       ((newA >= std::min(0.2, newB)) || (pow(newX, newA) <= 0.9))))
  {
    const Scalar value = BPSER(newA, newB, newX);
    return (tail == swappedABXY ? value : 1.0 - value);
  }
  if ((maxABGreater1 && (newB > 1.0) && (newX >= 0.3)) ||
      (!maxABGreater1 && (newA < std::min(0.2, newB)) && (newX >= 0.3) && (pow(newX, newA) > 0.9)))
  {
    const Scalar value = BPSER(newB, newA, newY);
    return (tail != swappedABXY ? value : 1.0 - value);
  }
  if (maxABGreater1 && (newB > 15.0) && (((0.1 <= newX) && (newX < 0.3)) || ((newX < 0.1) && (pow(newB * newX, newA) > 0.7))))
  {
    const Scalar value = BGRAT(newB, newA, newY, newX, 0.0);
    return (tail != swappedABXY ? value : 1.0 - value);
  }
  // The remaining cases of min(a, b) <= 1
  const Scalar w0 = BUP(newB, newA, newY, newX, 20);
  const Scalar value = BGRAT(newB + 20, newA, newY, newX, w0);
  return (tail != swappedABXY ? value : 1.0 - value);
}

// Function BPSER in:
// Armido R. Didonato a,d Alfred H. Morris, "Algorithm 708: Significant Digit Computation of the Incomplete Beta Function Ratios"
Scalar BPSER(const Scalar a,
             const Scalar b,
             const Scalar x)
{
  if (x == 0.0) return 0.0;
  const Scalar pMin = std::min(a, b);
  Scalar value = 0.0;
  if (pMin >= 1.0) value = exp(a * log(a) - SpecFunc::LnBeta(a, b)) / a;
  else
  {
    const Scalar pMax = std::max(a, b);
    value = (pMax >= 8.0 ? pMin * exp(a * log(x) - SpecFunc::LogGamma1p(pMin) - ALGDIV(pMin, pMax)) / a : pow(x, a) * (b / (a + b)) * (1.0 + SpecFunc::IGamma1pm1(a)) * (1.0 + SpecFunc::IGamma1pm1(b)) / (1.0 + SpecFunc::IGamma1pm1(a + b)));
  } // pMin < 1.0
  const Scalar epsilon = SpecFunc::ScalarEpsilon / a;
  Scalar sum = 0.0;
  Scalar term = 1.0;
  for (UnsignedInteger j = 1; j < SpecFunc::MaximumIteration; ++n)
  {
    term *= x * (0.5 + (0.5 - b / j));
    sum += term / (a + j);
    if (std::abs(term) < epsilon) break;
  }
  return value * (1.0 + a * sum);
}
// Function ALGDIV
Scalar ALGDIV(const Scalar a,
              const Scalar b)
{
  Scalar x = -1.0;
  Scalar xM1 = -1.0;
  Scalar d = -1.0;
  if (a > b)
  {
    const Scalar h = b / a;
    xM1 = 1.0 / (1.0 + h);
    x = h / (1.0 + h);
    d = a + (b - 0.5);
  } // a > b
  else
  {
    // a <= b
    const Scalar h = a / b;
    xM1 = h / (1.0 + h);
    x = 1.0 / (1.0 + h);
    d = b + (a - 0.5);
  } // a <= b
  const Scalar x2 = x * x;
  const Scalar s3 = 1.0 + (x + x2);
  const Scalar s5 = 1.0 + (x + x2 * s3);
  const Scalar s7 = 1.0 + (x + x2 * s5);
  const Scalar s9 = 1.0 + (x + x2 * s7);
  const Scalar s11 = 1.0 + (x + x2 * s9);
  const Scalar t = 1.0 / (b * b);
  const Scalar w = xM1 * (0.833333333333333e-01 + t * (-0.277777777760991e-02 * s3 + t * (0.793650666825390e-03 * s5 + t * (-0.595202931351870e-03 * s7 + t * (0.837308034031215e-03 * s9 - 0.165322962780713e-02 * s11 * t))))) / b;
  const Scalar u = d * log1p(a / b);
  const Scalar v = a * (log(b) - 1.0);
  return (u > v ? (w - v) - u : (w - u) - v);
}
// Function BUP in:
// Armido R. Didonato a,d Alfred H. Morris, "Algorithm 708: Significant Digit Computation of the Incomplete Beta Function Ratios"
Scalar BUP(const Scalar a,
           const Scalar b,
           const Scalar x,
           const Scalar y,
           const UnsignedInteger n)
{
  Scalar value = 1.0;
  Scalar term = a * x * (1.0 - b) / (1.0 + a);
  for (UnsignedInteger k = 1; k <= SpecFunc::MaximumIteration; ++k)
  {
    value += term;
    if (std::abs(term) <= value * SpecFunc::Precision) break;
    term *= x * (k - b) / (k * (k + a));
  }
  return exp(a * log(x) - log(a) - SpecFunc::LogBeta(a, b)) * value;
}
Scalar BGRAT(const Scalar a,
             const Scalar b,
             const Scalar x,
             const Scalar y,
             const Scalar w)
{

}
#endif
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
