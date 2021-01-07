//                                               -*- C++ -*-
/**
 *  @brief OpenTURNS wrapper to a library of special functions
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <limits>
#include <cstdlib>
#include <iostream>

#include "openturns/SpecFunc.hxx"
#include "openturns/BetaFunctions.hxx"
#include "openturns/ExponentialIntegralFunctions.hxx"
#include "openturns/Debye.hxx"
#include "Faddeeva.hh"
#include "openturns/GammaFunctions.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Function.hxx"
#include "openturns/GaussKronrod.hxx"
#include "openturns/PlatformInfo.hxx"

#ifdef OPENTURNS_HAVE_BOOST

#include <boost/math/special_functions/bessel.hpp>
#include <boost/version.hpp>

#if (BOOST_VERSION >= 105600)

#include <boost/math/special_functions/bessel_prime.hpp>

#endif

#else

#include "openturns/GaussKronrod.hxx"
#include "openturns/SymbolicFunction.hxx"
#include "openturns/Interval.hxx"

#endif

BEGIN_NAMESPACE_OPENTURNS

// 0.39894228040143267 = 1 / sqrt(2.pi)
const Scalar SpecFunc::ISQRT2PI              = 0.3989422804014326779399462;
// 2.5066282746310005024 = sqrt(2.pi)
const Scalar SpecFunc::SQRT2PI = 2.506628274631000502415765;
// 0.91893853320467274177 = log(sqrt(2.pi))
const Scalar SpecFunc::LOGSQRT2PI            = 0.91893853320467274178;
// 0.57721566490153286 = Euler constant gamma
const Scalar SpecFunc::EulerConstant         = 0.57721566490153286;
// 1.64493406684822643 = pi^2 / 6
const Scalar SpecFunc::PI2_6                 = 1.64493406684822643;
// 1.28254983016118640 = pi / sqrt(6)
const Scalar SpecFunc::PI_SQRT6              = 1.28254983016118640;
// 0.45005320754569466 = gamma * sqrt(6) / pi
const Scalar SpecFunc::EULERSQRT6_PI         = 0.45005320754569466;
// 3.28986813369645287 = pi^2 / 3
const Scalar SpecFunc::PI2_3                 = 3.28986813369645287;
// 0.55132889542179204 = sqrt(3) / pi
const Scalar SpecFunc::SQRT3_PI              = 0.55132889542179204;
// 1.81379936423421785 = pi / sqrt(3)
const Scalar SpecFunc::PI_SQRT3              = 1.81379936423421785;
// 6.283185307179586476925286 = 2*pi
const Scalar SpecFunc::TWOPI = 6.283185307179586476925286;
// 1.20205690315959429 = Zeta(3)
const Scalar SpecFunc::ZETA3 = 1.20205690315959429;
// Scalar limits
const Scalar SpecFunc::MinScalar    = std::numeric_limits<Scalar>::min();
const Scalar SpecFunc::LogMinScalar = log(MinScalar);
const Scalar SpecFunc::MaxScalar    = std::numeric_limits<Scalar>::max();
const Scalar SpecFunc::LogMaxScalar = log(MaxScalar);
const Scalar SpecFunc::LowestScalar = -MaxScalar;
const Scalar SpecFunc::ScalarEpsilon = std::numeric_limits<Scalar>::epsilon();
// Maximum number of iterations for the algorithms
const UnsignedInteger SpecFunc::MaximumIteration = ResourceMap::GetAsUnsignedInteger("SpecFunc-MaximumIteration");
const Scalar SpecFunc::Precision = ResourceMap::GetAsScalar("SpecFunc-Precision");

// Some facilities for NaN and inf
Bool SpecFunc::IsNaN(const Scalar value)
{
  return value != value;
}

Bool SpecFunc::IsInf(const Scalar value)
{
  return (value == value) && IsNaN(value - value);
}

Bool SpecFunc::IsNormal(const Scalar value)
{
  return value - value == 0.0;
}

// Modified first kind Bessel function of order 0: BesselI0(x) = \sum_{m=0}\infty\frac{1}{m!^2}\left(\frac{x}{2}\right)^{2m}
Scalar SpecFunc::SmallCaseBesselI0(const Scalar x)
{
  const Scalar x2 = x * x;
  Scalar value = 1.0;
  Scalar r = 1.0;
  UnsignedInteger k = 1;
  while ((std::abs(r / value) > 0.0) && (k < SpecFunc::MaximumIteration))
  {
    r *= 0.25 * x2 / (k * k);
    value += r;
    ++k;
  }
  return value;
}

Scalar SpecFunc::LargeCaseLogBesselI0(const Scalar x)
{
  static Scalar A[12] = {0.125, 7.03125e-02,
                         7.32421875e-02, 1.1215209960938e-01,
                         2.2710800170898e-01, 5.7250142097473e-01,
                         1.7277275025845, 6.0740420012735,
                         2.4380529699556e+01, 1.1001714026925e+02,
                         5.5133589612202e+02, 3.0380905109224e+03
                        };
  const Scalar ax = std::abs(x);
  UnsignedInteger k0 = 12;
  if (ax >= 35.0) k0 = 9;
  if (ax >= 50.0) k0 = 7;
  Scalar value = 1.0;
  const Scalar xR = 1.0 / ax;
  Scalar xRPow = xR;
  for (UnsignedInteger k = 0; k < k0; ++k)
  {
    value += A[k] * xRPow;
    xRPow *= xR;
  }
  value = log(value) + ax - 0.5 * log(2.0 * M_PI * ax);
  return value;
}

Scalar SpecFunc::BesselI0(const Scalar x)
{
  if (x == 0.0) return 1.0;
  // Small argument
  if (std::abs(x) <= 23.5) return SmallCaseBesselI0(x);
  // Large argument
  else return exp(LargeCaseLogBesselI0(x));
}

Scalar SpecFunc::LogBesselI0(const Scalar x)
{
  if (x == 0.0) return 0.0;
  // Small argument
  if (std::abs(x) <= 23.5) return log(SmallCaseBesselI0(x));
  // Large argument
  else return LargeCaseLogBesselI0(x);
}

// Modified first kind Bessel function of order 1: BesselI1(x) = \sum_{m=0}\infty\frac{1}{m!(m+1)!}\left(\frac{x}{2}\right)^{2m+1}
Scalar SpecFunc::SmallCaseBesselI1(const Scalar x)
{
  const Scalar x2 = x * x;
  Scalar value = 1.0;
  Scalar r = 1.0;
  UnsignedInteger k = 1;
  while ((std::abs(r / value) > 0.0) && (k < SpecFunc::MaximumIteration))
  {
    r *= 0.25 * x2 / (k * (k + 1));
    value += r;
    ++k;
  }
  value *= 0.5 * x;
  return value;
}

Scalar SpecFunc::LargeCaseLogBesselI1(const Scalar x)
{
  static Scalar B[12] = { -0.375, -1.171875e-01,
                          -1.025390625e-01, -1.4419555664063e-01,
                          -2.7757644653320e-01, -6.7659258842468e-01,
                          -1.9935317337513, -6.8839142681099,
                          -2.7248827311269e+01, -1.2159789187654e+02,
                          -6.0384407670507e+02, -3.3022722944809e+03
                        };
  const Scalar ax = std::abs(x);
  UnsignedInteger k0 = 12;
  if (ax >= 35.0) k0 = 9;
  if (ax >= 50.0) k0 = 7;
  Scalar value = 1.0;
  const Scalar xR = 1.0 / ax;
  Scalar xRPow = xR;
  for (UnsignedInteger k = 0; k < k0; ++k)
  {
    value += B[k] * xRPow;
    xRPow *= xR;
  }
  value = log(value) + ax - 0.5 * log(2.0 * M_PI * ax);
  return value;
}

Scalar SpecFunc::BesselI1(const Scalar x)
{
  if (x == 0.0) return 0.0;
  // Small argument
  if (std::abs(x) <= 22.0) return SmallCaseBesselI1(x);
  else
  {
    const Scalar signX = x <= 0.0 ? -1.0 : 1.0;
    const Scalar value = signX * exp(LargeCaseLogBesselI1(x));
    return value;
  }
}

Scalar SpecFunc::LogBesselI1(const Scalar x)
{
  if (x <= 0.0) return LowestScalar;
  // Small argument
  if (std::abs(x) <= 22.0) return log(SmallCaseBesselI1(x));
  else return LargeCaseLogBesselI1(x);
}

// Difference between the logarithms of BesselI1 and BesselI0:
// DeltaLogBesselI10(x) = log(BesselI1(x)) - log(BesselI0(x))
Scalar SpecFunc::LargeCaseDeltaLogBesselI10(const Scalar x)
{
  static Scalar A[12] = {0.125, 7.03125e-02,
                         7.32421875e-02, 1.1215209960938e-01,
                         2.2710800170898e-01, 5.7250142097473e-01,
                         1.7277275025845, 6.0740420012735,
                         2.4380529699556e+01, 1.1001714026925e+02,
                         5.5133589612202e+02, 3.0380905109224e+03
                        };
  static Scalar B[12] = { -0.375, -1.171875e-01,
                          -1.025390625e-01, -1.4419555664063e-01,
                          -2.7757644653320e-01, -6.7659258842468e-01,
                          -1.9935317337513, -6.8839142681099,
                          -2.7248827311269e+01, -1.2159789187654e+02,
                          -6.0384407670507e+02, -3.3022722944809e+03
                        };
  const Scalar ax = std::abs(x);
  UnsignedInteger k0 = 12;
  if (ax >= 35.0) k0 = 9;
  if (ax >= 50.0) k0 = 7;
  Scalar valueI0 = 1.0;
  Scalar valueI1 = 1.0;
  const Scalar xR = 1.0 / ax;
  Scalar xRPow = xR;
  for (UnsignedInteger k = 0; k < k0; ++k)
  {
    valueI0 += A[k] * xRPow;
    valueI1 += B[k] * xRPow;
    xRPow *= xR;
  }
  return log(valueI1) - log(valueI0);
}

Scalar SpecFunc::DeltaLogBesselI10(const Scalar x)
{
  if (x <= 0.0) return LowestScalar;
  // Small argument
  if (std::abs(x) <= 22.0) return log(SmallCaseBesselI1(x) / SmallCaseBesselI0(x));
  else return LargeCaseDeltaLogBesselI10(x);
}

// Logarithm of the modified second kind Bessel function of order nu: LogBesselK(nu, x)=log(\frac{\pi}{2}\frac{I_{-\nu}(x)-I_[\nu}(x)}{\sin{\nu\pi}})
Scalar SpecFunc::LogBesselK(const Scalar nu,
                            const Scalar x)
{
  if (!(x > 0.0)) throw InvalidArgumentException(HERE) << "Error: x must be positive, here x=" << x;
  // Reflection formula
  if (nu < 0.0) return LogBesselK(-nu, x);
  // Special cases
  if (nu == 0.5) return 0.5 * std::log(M_PI / (2.0 * x)) - x;
  if (nu == 1.5)
  {
    const Scalar num = 1.0 + 1.0 / x;
    return 0.5 * std::log(M_PI * num * num / (2.0 * x)) - x;
  }
  if (nu == 2.5)
  {
    const Scalar num = 1.0 + (3.0 / x) * (1.0 + 1.0 / x);
    return 0.5 * std::log(M_PI * num * num / (2.0 * x)) - x;
  }
#ifdef OPENTURNS_HAVE_BOOST
  return std::log(boost::math::cyl_bessel_k(nu, x));
#else
  Scalar logFactor = 0.0;
  Function integrand;
  UnsignedInteger precision = PlatformInfo::GetNumericalPrecision();
  PlatformInfo::SetNumericalPrecision(16);
  Scalar upper = -1.0;
  if (nu == 0.0)
  {
    integrand = SymbolicFunction("t", String(OSS() << "exp(-" << x << "*cosh(t))"));
    upper = std::log(-2.0 * std::log(ScalarEpsilon) / x);
  }
  else
  {
    logFactor = nu * std::log(0.5 * x) - LogGamma(0.5 + nu) + 0.5 * std::log(M_PI);
    integrand = SymbolicFunction("t", String(OSS() << "exp(-" << x << "*cosh(t))*(sinh(t))^" << 2.0 * nu));
    upper = std::log(ScalarEpsilon) / (2.0 * nu) - LambertW(-0.25 * x * std::exp(0.5 * std::log(ScalarEpsilon) / nu) / nu, false);
  }
  Scalar epsilon = -1.0;
  const Scalar integral = GaussKronrod().integrate(integrand, Interval(ScalarEpsilon, upper), epsilon)[0];
  PlatformInfo::SetNumericalPrecision(precision);
  if (!IsNormal(integral) || (integral == 0.0)) return LowestScalar;
  return logFactor + std::log(integral);
#endif
}

// Modified second kind Bessel function of order nu: BesselK(nu, x)=\frac{\pi}{2}\frac{I_{-\nu}(x)-I_[\nu}(x)}{\sin{\nu\pi}}
Scalar SpecFunc::BesselK(const Scalar nu,
                         const Scalar x)
{
#ifdef OPENTURNS_HAVE_BOOST
  return boost::math::cyl_bessel_k(nu, x);
#else
  if (!(x > 0.0)) throw InvalidArgumentException(HERE) << "Error: x must be positive, here x=" << x;
  // Reflection formula
  if (nu < 0.0) return BesselK(-nu, x);
  // First the limit cases
  if ((std::abs(x) < 0.0056) && (nu == 0.0))
  {
    const Scalar logX = log(x);
    const Scalar x2 = 0.25 * x * x;
    return M_LN2 - logX - EulerConstant + x2 * (M_LN2 - logX + 1.0 - EulerConstant + 0.25 * x2 * (M_LN2 - logX + 1.5 - EulerConstant));
  }
  if (std::abs(x) < 1e-8) return 0.5 * exp(LogGamma(nu) - nu * std::log(0.5 * x));
  if ((std::abs(x) > 1e4) && (x > nu)) return std::sqrt(M_PI / (2.0 * x)) * exp(-x);
  const Scalar logK = LogBesselK(nu, x);
  if (logK >= LogMaxScalar) return MaxScalar;
  return std::exp(logK);
#endif
}

// Modified second kind Bessel derivative function of order nu: BesselKDerivative(nu, x)=dBesselK(nu, x) / dx
Scalar SpecFunc::BesselKDerivative(const Scalar nu,
                                   const Scalar x)
{
#if defined(OPENTURNS_HAVE_BOOST) && (BOOST_VERSION >= 105600)
  return boost::math::cyl_bessel_k_prime(nu, x);
#else
  if (x == 0.0) return LowestScalar;
  return -0.5 * (BesselK(nu - 1, x) + BesselK(nu + 1.0, x));
#endif
}

// LnBeta function: LnBeta(a, b) = \log(Beta(a, b))
Scalar SpecFunc::LnBeta(const Scalar a,
                        const Scalar b)
{
  const Scalar first = std::min(a, b);
  if (!(first > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute the LogBeta function when a or b is nonpositive, a=" << a << ", b=" << b;
  const Scalar second = std::max(a, b);
  const Scalar sum = a + b;
  // Common case: a and b small
  if (second < 7.75) return lgamma(first) - lgamma(sum) + lgamma(second);
  const Scalar correctionSecond = GammaCorrection(second);
  const Scalar correctionSum = GammaCorrection(sum);
  // Case a and b large
  if (first >= 7.75)
  {
    // b not very large wrt a
    if (second < 103.25 * first) return correctionSecond - correctionSum + GammaCorrection(first) - 0.5 * log(second) + second * log(second / first) + (sum - 0.5) * log1p(-second / sum) + LOGSQRT2PI;
    else
    {
      const Scalar epsilon = 1.0 / second;
      Scalar value = log(epsilon);
      // Here we use an expansion of (log(Gamma(b)) - log(Gamma(a+b))) / a - log(b) pour a << b
      // The expansion is in double Padé form wrt a and b
      const Scalar c1 = -1.0 + first;
      const Scalar c2 = c1 * (2 * first - 1);
      const Scalar c3 = c1 * c1;
      const Scalar c4 = -6.0 + 3.0 * first;
      const Scalar c5 = first * first;
      value += epsilon * (-0.5 * c1 + epsilon * (c2 / 12.0 + epsilon * (-(c3 * first) / 12.0 + epsilon * (((-1.0 + 3.0 * first * c1) * c2) / 120.0 + epsilon * (-(c3 * (-1.0 + 2.0 * first * c1) * first) / 60.0 + epsilon * (((1.0 + first * (3.0 + c5 * c4)) * c2) / 252.0 + epsilon * (-(c3 * (2.0 + first * (4.0 + first * (-1.0 + first * c4))) * first) / 168.0 + ((-3.0 + (-9.0 + (-1.0 + (15.0 + (5.0 + (-15.0 + 5.0 * first) * first) * first) * first) * first) * first) * epsilon * c2) / 720.0)))))));
      value *= first;
      value += LogGamma(first);
      return value;
    }
  }
  // Case one of a,b large, the other small
  return LogGamma(first) + correctionSecond - correctionSum + first * (1.0 - log(sum)) + (second - 0.5) * log1p(-first / sum);
}

// LogBeta = LnBeta
Scalar SpecFunc::LogBeta(const Scalar a,
                         const Scalar b)
{
  return LnBeta(a, b);
}

// Beta function: Beta(a, b) = \int_0^1 t^{a-1}(1-t)^{b-1} dt
Scalar SpecFunc::Beta(const Scalar a,
                      const Scalar b)
{
  return exp(LnBeta(a, b));
}

// Incomplete Beta function: BetaInc(a, b, x) = \int_0^x t^{a-1}(1-t)^{b-1} dt
Scalar SpecFunc::IncompleteBeta(const Scalar a,
                                const Scalar b,
                                const Scalar x,
                                const Bool tail)
{
  return RegularizedIncompleteBeta(a, b, x, tail) * Beta(a, b);
}

// Incomplete Beta function inverse
Scalar SpecFunc::IncompleteBetaInverse(const Scalar a,
                                       const Scalar b,
                                       const Scalar x,
                                       const Bool tail)
{
  return RegularizedIncompleteBetaInverse(a, b, x / Beta(a, b), tail);
}

// Regularized Incomplete Beta function: RegularizedIncompleteBeta(a, b, x) = 1/beta(a, b) * \int_0^x t^{a-1}(1-t)^{b-1} dt
Scalar SpecFunc::RegularizedIncompleteBeta(const Scalar a,
    const Scalar b,
    const Scalar x,
    const Bool tail)
{
  return BetaFunctions::RegularizedIncompleteBeta(a, b, x, tail);
}

// Regularized incomplete Beta Function inverse
Scalar SpecFunc::RegularizedIncompleteBetaInverse(const Scalar a,
    const Scalar b,
    const Scalar x,
    const Bool tail)
{
  return BetaFunctions::RegularizedIncompleteBetaInverse(a, b, x, tail);
}

// Dawson function: Dawson(x) = \exp(-x^2) * \int_0^x \exp(t^2) dt
Scalar SpecFunc::Dawson(const Scalar x)
{
  return Faddeeva::Dawson(x);
}

Complex SpecFunc::Dawson(const Complex & z)
{
  return Faddeeva::Dawson(z);
}

// Debye function of order n: DebyeN(x, n) = n / x^n \int_0^x t^n/(\exp(t)-1) dt
Scalar SpecFunc::Debye(const Scalar x,
                       const UnsignedInteger n)
{
  if ((n == 0) || (n > 20)) throw InvalidArgumentException(HERE) << "Error: cannot compute Debye function of order outside of {1,...,20}";
  if (x < 0.0) return Debye(-x, n) - n * x / (n + 1.0);
  // The threshold is such that the overall error is less than 1.0e-16
  if (x < 1.0e-8) return 1.0 - n * x / (2.0 * (n + 1.0));
  return debyen(x, static_cast<int>(n)) * n / pow(x, static_cast<int>(n));
}

// DiLog function: DiLog(x) = -\int_0^x \log(1-t)/t dt
Scalar SpecFunc::DiLog(const Scalar x)
{
  // Special case for 0
  if (x == 0.0) return 0.0;
  // Special case for 1
  if (x == 1.0) return PI2_6;
  // No real value on (1, \infty)
  if (!(x <= 1.0)) throw InvalidArgumentException(HERE) << "Error: the DiLog function does not take real values for arguments greater than 1.";
  // Special case for x close to 1
  if (x >= 0.999997)
  {
    const Scalar z = 1.0 - x;
    const Scalar logZ = std::log(z);
    return PI2_6 + z * (logZ - 1.0 + z * (logZ - 0.5) * 0.5);
  }
  // Use DiLog(x) = -DiLog(1 / x) - \pi^2 / 6 - \log^2(-x) / 2
  // to map (-\infty, -1) into (-1, 0) for the argument
  if (x < -1.0) return -DiLog(1.0 / x) - PI2_6  - 0.5 * pow(log(-x), 2);
  // Use DiLog(x) = DiLog(x^2) / 2 - DiLog(-x)
  // to map [-1, 0) into (0, 1]
  if (x < 0.0) return 0.5 * DiLog(x * x) - DiLog(-x);
  // Use DiLog(x) = \pi^2 / 6 - DiLog(1 - x) - \log(x)\log(1-x)
  // to map (1/2, 1] into [0, 1/2)
  if (x > 0.5) return PI2_6 - DiLog(1.0 - x) - log(x) * log1p(-x);
  // Use the definition of DiLog in terms of series
  // DiLog(x)=\sum_{k=1}^{\infty} x^k/k^2
  // for (0, 1/2)
  // This upper bound is an easy-to compute tight upper bound of the number of iterations
  const UnsignedInteger nMax = static_cast<UnsignedInteger>(round(8 + 68 * x));
  Scalar value = 0.0;
  Scalar powerX = 1.0;
  for (UnsignedInteger n = 1; n <= nMax; ++n)
  {
    powerX *= x;
    value += powerX / (n * n);
  }
  return value;
}

// Exponential integral function: Ei(x) = -\int_{-x}^{\infty}exp(-t)/t dt
Scalar SpecFunc::Ei(const Scalar x)
{
  return ExponentialIntegralFunctions::Ei(x);
}

// Complex exponential integral function: Ei(z) = -\int_{-z}^{\infty}exp(-t)/t dt
Complex SpecFunc::Ei(const Complex & z)
{
  return ExponentialIntegralFunctions::Ei(z);
}

// Complex Faddeeva function: faddeeva(z) = \exp(-z^2)\erfc(-I*z)
Complex SpecFunc::Faddeeva(const Complex & z)
{
  return Faddeeva::w(z);
}

Scalar SpecFunc::FaddeevaIm(const Scalar x)
{
  return Faddeeva::w_im(x);
}

// Factorial function
Scalar SpecFunc::LogFactorial(const UnsignedInteger n)
{
  static Scalar A[128] = {0.0, 0.0, 0.69314718055994531, 1.7917594692280550, 3.1780538303479456, 4.7874917427820460, 6.5792512120101010, 8.5251613610654143, 10.604602902745250, 12.801827480081470, 15.104412573075515, 17.502307845873886, 19.987214495661886, 22.552163853123423, 25.191221182738682, 27.899271383840892, 30.671860106080673, 33.505073450136889, 36.395445208033054, 39.339884187199494, 42.335616460753485, 45.380138898476908, 48.471181351835224, 51.606675567764374, 54.784729398112319, 58.003605222980520, 61.261701761002002, 64.557538627006331, 67.889743137181535, 71.257038967168009, 74.658236348830164, 78.092223553315311, 81.557959456115037, 85.054467017581517, 88.580827542197679, 92.136175603687092, 95.719694542143202, 99.330612454787427, 102.96819861451381, 106.63176026064346, 110.32063971475740, 114.03421178146170, 117.77188139974507, 121.53308151543863, 125.31727114935690, 129.12393363912721, 132.95257503561631, 136.80272263732637, 140.67392364823426, 144.56574394634489, 148.47776695177303, 152.40959258449736, 156.36083630307879, 160.33112821663091, 164.32011226319518, 168.32744544842765, 172.35279713916280, 176.39584840699735, 180.45629141754377, 184.53382886144949, 188.62817342367159, 192.73904728784490, 196.86618167288999, 201.00931639928153, 205.16819948264120, 209.34258675253684, 213.53224149456326, 217.73693411395423, 221.95644181913033, 226.19054832372759, 230.43904356577695, 234.70172344281827, 238.97838956183432, 243.26884900298271, 247.57291409618688, 251.89040220972319, 256.22113555000953, 260.56494097186321, 264.92164979855280, 269.29109765101982, 273.67312428569370, 278.06757344036614, 282.47429268763040, 286.89313329542699, 291.32395009427031, 295.76660135076062, 300.22094864701413, 304.68685676566872, 309.16419358014692, 313.65282994987906, 318.15263962020933, 322.66349912672618, 327.18528770377522, 331.71788719692847, 336.26118197919848, 340.81505887079902, 345.37940706226685, 349.95411804077024, 354.53908551944081, 359.13420536957540, 363.73937555556349, 368.35449607240475, 372.97946888568902, 377.61419787391866, 382.25858877306003, 386.91254912321755, 391.57598821732962, 396.24881705179153, 400.93094827891575, 405.62229616114489, 410.32277652693731, 415.03230672824964, 419.75080559954473, 424.47819341825707, 429.21439186665157, 433.95932399501482, 438.71291418612118, 443.47508812091894, 448.24577274538461, 453.02489623849614, 457.81238798127818, 462.60817852687492, 467.41219957160818, 472.22438392698060, 477.04466549258563, 481.87297922988793, 486.70926113683941, 491.55344822329800};
  if (n < 128) return A[n];
  /* Stieltjes approximation, see http://www.luschny.de/math/factorial/approx/SimpleCases.html */
  static Scalar a0 = 1.0 / 12.0;
  static Scalar a1 = 1.0 / 30.0;
  static Scalar a2 = 53.0 / 210.0;
  static Scalar a3 = 195.0 / 371.0;
  static Scalar a4 = 22999.0 / 22737.0;
  static Scalar a5 = 29944523.0 / 19733142.0;
  static Scalar a6 = 109535241009.0 / 48264275462.0;
  const Scalar z = n + 1.0;
  return LOGSQRT2PI + (z - 0.5) * std::log(z) - z + a0 / (z + a1 / (z + a2 / (z + a3 / (z + a4 / (z + a5 / (z + a6 / z))))));
}

Scalar SpecFunc::Factorial(const UnsignedInteger n)
{
  if (n < LogMaxScalar) return std::exp(LogFactorial(n));
  throw InternalException(HERE) << "Error: n=" << n << " is too large for n! to fit into the return type";
  return MaxScalar;
}

// Gamma function: Gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t) dt
Scalar SpecFunc::Gamma(const Scalar a)
{
  return tgamma(a);
}

// igamma1pm1(a) = 1 / Gamma(1 + a) - 1
Scalar SpecFunc::IGamma1pm1(const Scalar a)
{
  if (a < -0.5) return a + IGamma1pm1(a + 1.0);
  if (a > 0.5) return (IGamma1pm1(a - 1.0) - a) / (1.0 + a);
  return a * (0.55840397973848040460 +    (-0.28693519326375203685 +
              (-0.28661639813928077048 +     (0.56469202267873782108e-2 +
                  (0.12419086997568707715e-1 + (-0.28455238049924868708e-2 +
                      (-0.11632361891621759586e-3 +   0.43452535150473018757e-4 * a) * a) * a) * a) * a) * a) * a) /
         (0.967409607349687017271 +    (0.602145048219053169110 +
                                        (0.258051013832915714221 +    (0.677000815494264186575e-1 +
                                            (0.142113296913915958898e-1 + (0.183821175266489590252e-2 +
                                                0.192078142776918599109e-3 * a) * a) * a) * a) * a) * a);
}

// GammaCorrection(a) = LogGamma(a) - log(sqrt(2.Pi)) + a - (a - 1/2) log(a)
Scalar SpecFunc::GammaCorrection(const Scalar a)
{
  if (!(a > 0.0)) throw InvalidArgumentException(HERE) << "Error: cannot compute GammaCorrection for nonpositive a, here a=" << a;
  // Use an asymptotic series for large argument
  if (a > 7.7490453948312251620)
  {
    const Scalar ia2 = 1.0 / (a * a);
    return (0.83333333333333333333e-1 + (-0.27777777777777777778e-2 +
                                         (0.79365079365079365079e-3 + (-0.59523809523809523810e-3 +
                                             (0.84175084175084175084e-3 + (-0.19175269175269175269e-2 +
                                                 (0.64102564102564102564e-2 + (-0.29550653594771241830e-1 +
                                                     (0.17964437236883057316e0 - 0.13924322169059011164e1 * ia2) * ia2) * ia2) * ia2) * ia2) * ia2) * ia2) * ia2) * ia2) / a;
  }
  return LogGamma(a) + a - (a - 0.5) * log(a) - LOGSQRT2PI;
}

// Complex gamma function: Gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t) dt,
// Computed using Lanczos approximation, using a C++ translation of
// Paul Godfrey's matlab implementation
// http://home.att.net/~numericana/answer/info/godfrey.htm#matlabgamma
Complex SpecFunc::Gamma(const Complex & a)
{
  if (a.imag() == 0.0) return Gamma(a.real());
  return exp(LogGamma(a));
}

Complex SpecFunc::LogGamma(const Complex & a)
{
  if (a.imag() == 0.0) return LogGamma(a.real());
  const Scalar sqrt2Pi = sqrt(2.0 * M_PI);
  Complex z(a);
  Bool flip = false;
  if (z.real() < 0.0)
  {
    z = -z;
    flip = true;
  }
  const UnsignedInteger coefficientsSize = 11;
  static const Scalar coefficients[coefficientsSize] =
  {
    1.000000000000000174663,      5716.400188274341379136,
    -14815.30426768413909044,     14291.49277657478554025,
    -6348.160217641458813289,     1301.608286058321874105,
    -108.1767053514369634679,     2.605696505611755827729,
    -0.7423452510201416151527e-2, 0.5384136432509564062961e-7,
    -0.4023533141268236372067e-8
  };
  const Scalar g = coefficientsSize - 2.0;
  Complex t(z + g);
  Complex s(0.0);
  Complex ss(t - 0.5);
  for (UnsignedInteger k = coefficientsSize - 1; k > 0; --k)
  {
    s += coefficients[k] / t;
    t -= 1.0;
  }
  s += coefficients[0];
  s = log(s * sqrt2Pi) + (z - 0.5) * log(ss) - ss;
  Complex f(s);
  if (flip) f = f + Log1p(-M_PI * exp(-f) / (a * f * sin(M_PI * a)));
  return f;
}

// Natural logarithm of the Gamma function
Scalar SpecFunc::LnGamma(const Scalar a)
{
  return LogGamma(a);
}

// LogGamma = LnGamma
Scalar SpecFunc::LogGamma(const Scalar a)
{
  return lgamma(a);
}

Scalar SpecFunc::LogGamma1p(const Scalar a)
{
  return (std::abs(a) < 0.5 ? -a * (0.34229051727072805652 +
                                    (0.75305954018877769214 +
                                     (0.25594427350421023219 +
                                      (-0.54867134418632830931 +
                                       (-0.57006260085649768851 +
                                        (-0.20361938002564003637 +
                                         (-0.27922966566918143201e-1 -
                                          0.10180389882069314488e-2 * a) * a) * a) * a) * a) * a) * a) /
          (0.59300282040876235168 +
           (0.21496034951064079616e1 +
            (0.30947091018029240660e1 +
             (0.22448538584537209829e1 +
              (0.85741167089803858333 +
               (0.16321946228463159159 +
                (0.12893353820029086191e-1 +
                 0.24787923059095734273e-3 * a) * a) * a) * a) * a) * a) * a) : LogGamma(a - 1.0));
}

// Incomplete Gamma function: IncompleteGamma(a, x) = \int_0^x t^{a-1}\exp(-t) dt
Scalar SpecFunc::IncompleteGamma(const Scalar a,
                                 const Scalar x,
                                 const Bool tail)
{
  return GammaFunctions::IncompleteGamma(a, x, tail);
}

// Incomplete Gamma function inverse with respect to x
Scalar SpecFunc::IncompleteGammaInverse(const Scalar a,
                                        const Scalar x,
                                        const Bool tail)
{
  return GammaFunctions::IncompleteGammaInverse(a, x, tail);
}

// Regularized incomplete Gamma function: RegularizedIncompleteGamma(a, x) = \int_0^x t^{a-1}\exp(-t) dt / \Gamma(a)
Scalar SpecFunc::RegularizedIncompleteGamma(const Scalar a,
    const Scalar x,
    const Bool tail)
{
  return GammaFunctions::RegularizedIncompleteGamma(a, x, tail);
}

// Regularized incomplete Gamma function inverse with respect to x
Scalar SpecFunc::RegularizedIncompleteGammaInverse(const Scalar a,
    const Scalar x,
    const Bool tail)
{
  return GammaFunctions::RegularizedIncompleteGammaInverse(a, x, tail);
}

// Digamma function: Psi(x) = ((dgamma/dx) / gamma)(x)
// Derivative of a Lanczos approximation of log(Gamma)
Scalar SpecFunc::DiGamma(const Scalar x)
{
  // Check that the argument is not non positive, i.e. not in {0, -1, -2, ...}
  if ((x <= 0.0) && (x == round(x))) throw InvalidArgumentException(HERE) << "Error: the argument of the DiGamma function cannot be a non positive integer.";
  // Approximation for small arguments
  // Here, 0.025 is a bound that insure Scalar precision approximation
  if ( std::abs(x) <= 0.025 ) return -1.0 / x - 0.57721566490153286 + (1.6449340668482264 + (-1.2020569031595943 + (1.0823232337111381 + (-1.0369277551433699 + (1.0173430619844491 + (-1.0083492773819228 + (1.0040773561979442 + (-1.0020083928260822 + 1.0009945751278180 * x) * x) * x) * x) * x) * x) * x) * x) * x;
  // If the argument is negative, use the reflexion formula
  if (x < 0.0) return -M_PI / tan(M_PI * x) + DiGamma(1.0 - x);
  // Shift the argument until it reaches the asymptotic expansion region
  // Here, 7.69 is a bound that insure Scalar precision of the approximation
  Scalar z = x;
  Scalar value = 0.0;
  while ( z < 7.33 )
  {
    value -= 1.0 / z;
    z += 1.0;
  }
  // Use the asymptotic expansion in Horner form
  const Scalar y = 1.0 / (z * z);
  return value + log(z) - 0.5 / z + (-0.83333333333333333e-1 + (0.83333333333333333e-2 + (-0.39682539682539683e-2 + (0.41666666666666667e-2 + (-0.75757575757575758e-2 + (0.21092796092796093e-1 + (-0.83333333333333333e-1 + (.44325980392156863 - 3.0539543302701197 * y) * y) * y) * y) * y) * y) * y) * y) * y;
}

Scalar SpecFunc::Psi(const Scalar x)
{
  return DiGamma(x);
}

// Inverse of the DiGamma function
Scalar SpecFunc::DiGammaInv(const Scalar a)
{
  // Initialization using an asymptotic approximation of the DiGamma function
  Scalar x = a < -2.22 ? -1.0 / (a - EulerConstant) : exp(a) + 0.5;
  // Use a Newton scheme
  Scalar d = 0.0;
  for (UnsignedInteger k = 0; k < 6; ++k)
  {
    d = (DiGamma(x) - a) / TriGamma(x);
    if (d == 0.0) break;
    x -= d;
  }
  return x;
}

// Trigamma function: TriGamma(x) = ((d^2gamma/dx^2) / gamma)(x)
Scalar SpecFunc::TriGamma(const Scalar x)
{
  // Check that the argument is not non positive, i.e. not in {0, -1, -2, ...}
  if ((x <= 0.0) && (x == round(x))) throw InvalidArgumentException(HERE) << "Error: the argument of the TriGamma function cannot be a non positive integer.";
  // Approximation for small arguments
  // Here, 0.02 is a bound that insure Scalar precision approximation
  if ( std::abs(x) <= 0.02 ) return 1.0 / (x * x) + 1.6449340668482264 + (-2.4041138063191886 + (3.2469697011334144 + (-4.1477110205734796 + (5.0867153099222453 + (-6.0500956642915368 + (7.0285414933856097 + (-8.0160671426086576 + (9.0089511761503616 - 10.004941886041195 * x) * x) * x) * x) * x) * x) * x) * x) * x;
  // If the argument is negative, use the reflexion formula
  if (x < 0.0) return pow(M_PI / sin(M_PI * x), 2.0) - TriGamma(1.0 - x);
  // Shift the argument until it reaches the asymptotic expansion region
  // Here, 7.69 is a bound that insure Scalar precision of the approximation
  Scalar z = x;
  Scalar value = 0.0;
  while ( z < 7.69 )
  {
    value += 1.0 / (z * z);
    z += 1.0;
  }
  // Use the asymptotic expansion in Horner form
  const Scalar y = 1.0 / (z * z);
  return value + 0.5 * y + (1. + (.16666666666666667 + (-0.33333333333333333e-1 + (0.23809523809523810e-1 + (-0.33333333333333333e-1 + (0.75757575757575758e-1 + (-.25311355311355311 + (1.1666666666666667 + (-7.0921568627450980 + 54.971177944862155 * y) * y) * y) * y) * y) * y) * y) * y) * y) / z;
}

/* Stirlerr(n) = log(n!) - log( sqrt(2*pi*n)*(n/e)^n )
*     = log Gamma(n+1) - 1/2 * [log(2*pi) + log(n)] - n*[log(n) - 1]
*     = log Gamma(n+1) - (n + 1/2) * log(n) + n - log(2*pi)/2 */
Scalar SpecFunc::Stirlerr(const UnsignedInteger n)
{
  static const Scalar stirlerrTable[26] =
  {
    0.000000000000000000000, 8.10614667953272582e-02, 4.13406959554092941e-02,
    2.76779256849983391e-02, 2.07906721037650931e-02, 1.66446911898211922e-02,
    1.38761288230707480e-02, 1.18967099458917701e-02, 1.04112652619720965e-02,
    9.25546218271273292e-03, 8.33056343336287126e-03, 7.57367548795184079e-03,
    6.94284010720952987e-03, 6.40899418800420707e-03, 5.95137011275884774e-03,
    5.55473355196280137e-03, 5.20765591960964044e-03, 4.90139594843473786e-03,
    4.62915374933402859e-03, 4.38556024923232427e-03, 4.16631969199692246e-03,
    3.96795421864085962e-03, 3.78761806844443458e-03, 3.62296022468309471e-03,
    3.47202138297876696e-03, 3.33315563672809288e-03
  };
  if (n < 26) return stirlerrTable[n];
  static Scalar S0 = 8.33333333333333333e-02;
  static Scalar S1 = 2.77777777777777778e-03;
  static Scalar S2 = 7.93650793650793651e-04;
  static Scalar S3 = 5.95238095238095238e-04;
  static Scalar S4 = 8.41750841750841751e-04;
  const Scalar nn = (1.0 * n) * n;
  if (n > 2559) return (S0 - S1 / nn) / n;
  if (n > 82)  return (S0 - (S1 - S2 / nn) / nn) / n;
  if (n > 50)  return (S0 - (S1 - (S2 - S3 / nn) / nn) / nn) / n;
  return (S0 - (S1 - (S2 - (S3 - S4 / nn) / nn) / nn) / nn) / n;
}

// Hypergeometric function of type (1,1): HyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
Scalar SpecFunc::HyperGeom_1_1(const Scalar p1,
                               const Scalar q1,
                               const Scalar x)
{
  if (q1 == p1) return exp(x);
  if (x == 0) return 1.0;
  Scalar term = 1.0;
  Scalar t = x;
  Scalar pochhammerP1 = p1;
  if (x < 0)
  {
    pochhammerP1 = q1 - p1;
    t = -x;
    term = exp(x);
  }
  Scalar pochhammerQ1 = q1;
  Scalar factorial = 1.0;
  Scalar sum = term;
  Scalar eps = -1.0;
  UnsignedInteger k = 0;
  do
  {
    term *= pochhammerP1 * t / (pochhammerQ1 * factorial);
    ++pochhammerP1;
    ++pochhammerQ1;
    ++factorial;
    sum += term;
    eps = std::abs(term / sum);
    ++k;
  }
  while ((eps > 0.0) && (k < SpecFunc::MaximumIteration));
  return sum;
}

// Complex hypergeometric function of type (1,1): HyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
Complex SpecFunc::HyperGeom_1_1(const Scalar p1,
                                const Scalar q1,
                                const Complex & x)
{
  Complex pochhammerP1(p1);
  Complex pochhammerQ1(q1);
  Scalar factorial = 1.0;
  Complex term(1.0);
  Complex sum(term);
  Complex eps(0.0);
  UnsignedInteger k = 0;
  do
  {
    term *= pochhammerP1 * x / (pochhammerQ1 * factorial);
    pochhammerP1 += 1.0;
    pochhammerQ1 += 1.0;
    ++factorial;
    sum += term;
    eps = term / sum;
    ++k;
  }
  // std::abs() for complex argument
  while ((std::abs(eps) > 0.0) && (k < SpecFunc::MaximumIteration));

  return sum;
}

// Hypergeometric function of type (2,1): HyperGeom_2_1(p1, p2, q1, x) = sum_{n=0}^{\infty} [prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k)] * x^n / n!
Scalar SpecFunc::HyperGeom_2_1(const Scalar p1,
                               const Scalar p2,
                               const Scalar q1,
                               const Scalar x)
{
  Scalar pochhammerP1 = p1;
  Scalar pochhammerP2 = p2;
  Scalar pochhammerQ1 = q1;
  Scalar factorial = 1.0;
  Scalar term = 1.0;
  Scalar sum = term;
  Scalar eps = 0.0;
  UnsignedInteger k = 0;
  do
  {
    term *= pochhammerP1 * pochhammerP2 * x / (pochhammerQ1 * factorial);
    ++pochhammerP1;
    ++pochhammerP2;
    ++pochhammerQ1;
    ++factorial;
    sum += term;
    eps = std::abs(term / sum);
    ++k;
  }
  while ((eps > 0.0) && (k < SpecFunc::MaximumIteration));
  return sum;
}

// Hypergeometric function of type (2,2): HyperGeom_2_1(p1, p2, q1, q2, x) = sum_{n=0}^{\infty} [prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k) / (q2 + k)] * x^n / n!
Scalar SpecFunc::HyperGeom_2_2(const Scalar p1,
                               const Scalar p2,
                               const Scalar q1,
                               const Scalar q2,
                               const Scalar x)
{
  if (x == 0.0) return 1.0;
  Scalar pochhammerP1 = p1;
  Scalar pochhammerP2 = p2;
  Scalar pochhammerQ1 = q1;
  Scalar pochhammerQ2 = q2;
  Scalar factorial = 1.0;
  Scalar term = 0.0;
  Scalar sum = term;
  Scalar eps = 0.0;
  const Scalar logX = log(std::abs(x));
  Scalar signX = x > 0.0 ? 1.0 : -1.0;
  Scalar signTerm = 1.0;
  UnsignedInteger k = 0;
  do
  {
    term += log(pochhammerP1) + log(pochhammerP2) + logX - log(pochhammerQ1) - log(pochhammerQ2) - log(factorial);
    ++pochhammerP1;
    ++pochhammerP2;
    ++pochhammerQ1;
    ++pochhammerQ2;
    ++factorial;
    sum += signTerm * exp(term);
    signTerm *= signX;
    eps = std::abs(term / sum);
    ++k;
  }
  while ((eps > 0.0) && (k < SpecFunc::MaximumIteration));
  return sum;
}

// Erf function Erf(x) = 2 / sqrt(Pi) . \int_0^x \exp(-t^2) dt
Scalar SpecFunc::Erf(const Scalar x)
{
  return Faddeeva::erf(x);
}

Complex SpecFunc::Erf(const Complex & z)
{
  return Faddeeva::erf(z);
}

// Erf function ErfI(x) = -i.erf(ix)
Scalar SpecFunc::ErfI(const Scalar x)
{
  return Faddeeva::erfi(x);
}

Complex SpecFunc::ErfI(const Complex & z)
{
  return Faddeeva::erfi(z);
}

// Erf function ErfC(x) = 1 - Erf(x)
Scalar SpecFunc::ErfC(const Scalar x)
{
  return Faddeeva::erfc(x);
}

Complex SpecFunc::ErfC(const Complex & z)
{
  return Faddeeva::erfc(z);
}

// Erf function ErfCX(x) = exp(x^2).erfC(x)
Scalar SpecFunc::ErfCX(const Scalar x)
{
  return Faddeeva::erfcx(x);
}

Complex SpecFunc::ErfCX(const Complex & z)
{
  return Faddeeva::erfcx(z);
}

// Inverse of the Erf function
// We use a rational approximation followed by one Halley's iteration (higher order Newton iteration)
Scalar SpecFunc::ErfInverse(const Scalar x)
{
  Scalar p = 0.5 * (x + 1.0);
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
  Scalar q = -1.0;
  Scalar t = -1.0;
  Scalar u = -1.0;
  q = std::min(p, 1.0 - p);
  if (q > 0.02425)
  {
    /* Rational approximation for central region. */
    u = q - 0.5;
    t = u * u;
    u = u * (((((a[0] * t + a[1]) * t + a[2]) * t + a[3]) * t + a[4]) * t + a[5])
        / (((((b[0] * t + b[1]) * t + b[2]) * t + b[3]) * t + b[4]) * t + 1.0);
  }
  else
  {
    /* Rational approximation for tail region. */
    t = sqrt(-2.0 * log(q));
    u = (((((c[0] * t + c[1]) * t + c[2]) * t + c[3]) * t + c[4]) * t + c[5])
        / ((((d[0] * t + d[1]) * t + d[2]) * t + d[3]) * t + 1.0);
  }
  /* The relative error of the approximation has absolute value less
     than 1.15e-9.  One iteration of Halley's rational method (third
     order) gives full machine precision... */
  t = 0.5 + 0.5 * Erf(u * M_SQRT1_2) - q;    /* f(u) = error */
  // 2.50662827463100050241576528481 = sqrt(2.pi)
  t = t * 2.50662827463100050241576528481 * exp(0.5 * u * u);   /* f(u)/df(u) */
  u = u - t / (1.0 + 0.5 * u * t);     /* Halley's method */
  return (p > 0.5 ? -M_SQRT1_2 * u : M_SQRT1_2 * u);
}

/* Evaluation of the principal branch of Lambert W function.
   Based on formulas exposed in:
   Robert M. Corless, G. H. Gonnet, D. E. G. Hare, D. J. Jeffrey, and D. E. Knuth, "On the Lambert W Function", Advances in Computational Mathematics, volume 5, 1996, pp. 329--359
*/
Scalar SpecFunc::LambertW(const Scalar x,
                          const Bool principal)
{
  Scalar w = -1.0;
  // -0.36787944117144232159552377016146086 = -1 / exp(1)
  if (x <= -0.3678794411714423215955238) return w;
  // Principal branch, defined over [-1/e,+inf], LambertW >= -1
  if (principal)
  {
    if (x == 0) return 0.0;
    if(x < 6.46) w = x * (3.0 + 4.0 * x) / (3.0 + x * (7.0 + 2.5 * x));
    // Large argument, use asymptotic expansion, formula 4.18
    else
    {
      const Scalar t1 = log(x);
      w = t1 - log(t1);
    }
  }
  // Second real branch, defined over [-1/e, 0[, LambertW <= -1
  else
  {
    if (x >= 0.0) return - std::numeric_limits<Scalar>::infinity();
    if (x < -0.1) w = -2.0;
    else
    {
      const Scalar t1 = log(-x);
      w = t1 - log(-t1);
    }
  }
  // Halley's iteration
  for (UnsignedInteger i = 0; i < 3; ++i)
  {
    const Scalar expW = exp(w);
    const Scalar numerator = w * expW - x;
    const Scalar dw = numerator / (expW * (w + 1.0) - 0.5 * (w + 2.0) * numerator / (w + 1.0));
    w -= dw;
  }
  return w;
}

// Accurate evaluation of log(1+z) for |z|<<1
Complex SpecFunc::Log1p(const Complex & z)
{
  if (std::norm(z) < 1e-5) return z * (1.0 + z * (-0.5 + z / 3.0));
  return log(1.0 + z);
}

// Accurate evaluation of exp(z)-1 for |z|<<1
Complex SpecFunc::Expm1(const Complex & z)
{
  if (std::norm(z) < 1e-5) return z * (1.0 + 0.5 * z * (1.0 + z / 3.0));
  return exp(z) - 1.0;
}

// Accurate evaluation of log(1-exp(-x)) for all x > 0
Complex SpecFunc::Log1MExp(const Scalar x)
{
  if (!(x > 0.0)) throw InvalidArgumentException(HERE) << "Error: x must be positive";
  if (x <= M_LN2) return log(-expm1(-x));
  return log1p(-exp(-x));
}

// Integer log2
UnsignedInteger SpecFunc::Log2(const Unsigned64BitsInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Error: n must be positive";

  // De Bruijn sequence
  const UnsignedInteger tab64[64] =
  {
    63,  0, 58,  1, 59, 47, 53,  2,
    60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12,
    44, 24, 15,  8, 23,  7,  6,  5
  };

  // http://www.pearsonhighered.com/samplechapter/0201914654.pdf
  Unsigned64BitsInteger value = n;
  value |= value >> 1;
  value |= value >> 2;
  value |= value >> 4;
  value |= value >> 8;
  value |= value >> 16;
  value |= value >> 32;

  return tab64[((Unsigned64BitsInteger)((value - (value >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}

// Compute the smallest power of two greater or equal to the given n
UnsignedInteger SpecFunc::NextPowerOfTwo(const UnsignedInteger n)
{
  UnsignedInteger powerOfTwo = 1;
  while (powerOfTwo < n) powerOfTwo <<= 1;
  return powerOfTwo;
}

// Integer power
Scalar SpecFunc::IPow(const Scalar x, const SignedInteger n)
{
  if (n == 0) return 1.0;
  if (x == 0) return 0.0;
  if (x < 0.0)
  {
    if (n % 2) return -std::pow(-x, 1.0 * n);
    return std::pow(-x, 1.0 * n);
  }
  return std::pow(x, 1.0 * n);
}

// Integer root
Scalar SpecFunc::IRoot(const Scalar x, const SignedInteger n)
{
  if (n == 0) throw InvalidArgumentException(HERE) << "Cannot take the zeroth root of anything!";
  if (x == 0) return 0.0;
  if (x < 0.0)
  {
    if (n % 2 == 0) throw InvalidArgumentException(HERE) << "Cannot take an even root of a negative number";
    return -std::pow(-x, 1.0 / n);
  }
  return std::pow(x, 1.0 / n);
}


// Compute the number of bits sets to 1 in n
// Best known algorithm for 64 bits n and fast multiply
UnsignedInteger SpecFunc::BitCount(const Unsigned64BitsInteger n)
{
  // types and constants used in the functions below

  const Unsigned64BitsInteger m1 = 0x5555555555555555; //binary: 0101...
  const Unsigned64BitsInteger m2 = 0x3333333333333333; //binary: 00110011..
  const Unsigned64BitsInteger m4 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
  const Unsigned64BitsInteger h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...

  // This uses fewer arithmetic operations than any other known
  // implementation on machines with fast multiplication.
  // It uses 12 arithmetic operations, one of which is a multiply.
  Unsigned64BitsInteger x = n;
  x -= (x >> 1) & m1;             // put count of each 2 bits into those 2 bits
  x = (x & m2) + ((x >> 2) & m2); // put count of each 4 bits into those 4 bits
  x = (x + (x >> 4)) & m4;        // put count of each 8 bits into those 8 bits
  return (x * h01) >> 56;         // returns left 8 bits of x + (x << 8) + (x << 16) + (x << 24) + ...
}

// Missing functions in cmath wrt math.h as of C++98
Scalar SpecFunc::Acosh(const Scalar x)
{
  if (!(x >= 1.0)) throw InvalidArgumentException(HERE) << "Error: acosh is not defined for x<1, here x=" << x;
  return 2.0 * log(sqrt(0.5 * (x + 1.0)) + sqrt(0.5 * (x - 1.0)));
}

Scalar SpecFunc::Asinh(const Scalar x)
{
  if (std::abs(x) < 0.0081972522783123062436) return x * (1.0 + x * x * (-1.0 / 6.0 + 3.0 * x * x / 40.0));
  return log(x + sqrt(1.0 + x * x));
}

Scalar SpecFunc::Atanh(const Scalar x)
{
  if (std::abs(x) < 0.0069422277258991260322) return x * (1.0 + x * x * (1.0 / 3.0 + x * x / 5.0));
  if (x > 0.0) return 0.5 * log1p(2.0 * x / (1.0 - x));
  return -0.5 * log1p(-2.0 * x / (1.0 + x));
}

Scalar SpecFunc::Cbrt(const Scalar x)
{
  if (x == 0.0) return 0.0;
  return (x < 0.0 ? -exp(log(-x) / 3.0) : exp(log(x) / 3.0));
}

UnsignedInteger SpecFunc::BinomialCoefficient(const UnsignedInteger n,
    const UnsignedInteger k)
{
  if (k > n) return 0; // by convention
  UnsignedInteger value = 1;
  for (UnsignedInteger i = 0; i < std::min(k, n - k); ++ i)
  {
    value *= n - i;
    value /= i + 1;
  }
  return value;
}


END_NAMESPACE_OPENTURNS
