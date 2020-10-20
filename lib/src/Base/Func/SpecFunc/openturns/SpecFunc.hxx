//                                               -*- C++ -*-
/**
 *  @brief OpenTURNS wrapper to a library of special functions
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SPECFUNC_HXX
#define OPENTURNS_SPECFUNC_HXX

#include "openturns/OTprivate.hxx"

/* Many mathematical functions lack on Windows when using
   Microsoft or Intel compilers.  We use Boost to define
   them here, so that these definitions are not duplicated
   across many files.  */
#ifdef _MSC_VER
#include <boost/math/special_functions.hpp>
#include <boost/numeric/conversion/converter_policies.hpp>

using boost::math::asinh;
using boost::math::acosh;
using boost::math::atanh;
using boost::math::cbrt;
using boost::math::erf;
using boost::math::erfc;
using boost::math::lgamma;
using boost::math::tgamma;
using boost::math::log1p;
using boost::math::expm1;

using boost::math::trunc;
using boost::math::round;

/* log2 is not defined */
static inline double log2(double x)
{
  return log(x) / log(2.);
}
/* rint is not defined */
static inline double rint(double x)
{
  return boost::numeric::RoundEven<double>::nearbyint(x);
}
/* nearbyint is not defined */
static inline double nearbyint(double x)
{
  return boost::numeric::RoundEven<double>::nearbyint(x);
}

#endif /* _MSC_VER */

BEGIN_NAMESPACE_OPENTURNS

class OT_API SpecFunc
{

public:

  // 0.39894228040143267 = 1 / sqrt(2.pi)
  static const Scalar ISQRT2PI;
  // 2.5066282746310005024 = sqrt(2.pi)
  static const Scalar SQRT2PI;
  // 0.91893853320467274177 = log(sqrt(2.pi))
  static const Scalar LOGSQRT2PI;
  // 0.57721566490153286 = Euler constant gamma
  static const Scalar EulerConstant;
  // 1.64493406684822643 = pi^2 / 6
  static const Scalar PI2_6;
  // 1.28254983016118640 = pi / sqrt(6)
  static const Scalar PI_SQRT6;
  // 0.45005320754569466 = gamma * sqrt(6) / pi
  static const Scalar EULERSQRT6_PI;
  // 3.28986813369645287 = pi^2 / 3
  static const Scalar PI2_3;
  // 0.55132889542179204 = sqrt(3) / pi
  static const Scalar SQRT3_PI;
  // 1.81379936423421785 = pi / sqrt(3)
  static const Scalar PI_SQRT3;
  // 6.283185307179586476925286 = 2*pi
  static const Scalar TWOPI;
  // 1.20205690315959429 = Zeta(3)
  static const Scalar ZETA3;
  // Maximum number of iterations for algorithms
  static const UnsignedInteger MaximumIteration;
  // Maximum precision for algorithms
  static const Scalar Precision;
  // Minimum positive real number
  static const Scalar MinScalar;
  static const Scalar LogMinScalar;
  // Maximum positive real number
  static const Scalar MaxScalar;
  static const Scalar LogMaxScalar;
  // Minimum negative real number;
  static const Scalar LowestScalar;
  // Real number accuracy
  static const Scalar ScalarEpsilon;

  // Some facilities for NaN and inf
  static Bool IsNaN(const Scalar value);
  static Bool IsInf(const Scalar value);
  static Bool IsNormal(const Scalar value);

  // Modified first kind Bessel function of order 0: BesselI0(x) = \sum_{m=0}\infty\frac{1}{m!^2}\left(\frac{x}{2}\right)^{2m}
private:
  static Scalar SmallCaseBesselI0(const Scalar x);
  static Scalar LargeCaseLogBesselI0(const Scalar x);
public:
  static Scalar BesselI0(const Scalar x);
  static Scalar LogBesselI0(const Scalar x);
  // Modified first kind Bessel function of order 1: BesselI1(x) = \sum_{m=0}\infty\frac{1}{m!(m+1)!}\left(\frac{x}{2}\right)^{2m+1}
private:
  static Scalar SmallCaseBesselI1(const Scalar x);
  static Scalar LargeCaseLogBesselI1(const Scalar x);
public:
  static Scalar BesselI1(const Scalar x);
  static Scalar LogBesselI1(const Scalar x);
  // Difference between the logarithms of BesselI1 and BesselI0:
  // DeltaLogBesselI10(x) = log(BesselI1(x)) - log(BesselI0(x))
private:
  static Scalar LargeCaseDeltaLogBesselI10(const Scalar x);
public:
  static Scalar DeltaLogBesselI10(const Scalar x);
  // Modified second kind Bessel function of order nu: BesselK(nu, x)=\frac{\pi}{2}\frac{I_{-\nu}(x)-I_[\nu}(x)}{\sin{\nu\pi}}
  static Scalar LogBesselK(const Scalar nu,
                           const Scalar x);
  static Scalar BesselK(const Scalar nu,
                        const Scalar x);
  static Scalar BesselKDerivative(const Scalar nu,
                                  const Scalar x);
  // Beta function: beta(a, b) = \int_0^1 t^{a-1}.(1-t)^{b-1} dt
  static Scalar Beta(const Scalar a,
                     const Scalar b);
  // Incomplete beta function: betaInc(a, b, x) = \int_0^x t^{a-1}.(1-t)^{b-1} dt
  static Scalar IncompleteBeta(const Scalar a,
                               const Scalar b,
                               const Scalar x,
                               const Bool tail = false);
  // Incomplete beta function inverse with respect to x
  static Scalar IncompleteBetaInverse(const Scalar a,
                                      const Scalar b,
                                      const Scalar x,
                                      const Bool tail = false);
  // Incomplete beta ratio function: betaRatioInc(a, b, x) = \int_0^x t^{a-1}.(1-t)^{b-1} dt / beta(a, b)
  static Scalar RegularizedIncompleteBeta(const Scalar a,
                                          const Scalar b,
                                          const Scalar x,
                                          const Bool tail = false);
  // Incomplete beta ratio function inverse with respect to x
  static Scalar RegularizedIncompleteBetaInverse(const Scalar a,
      const Scalar b,
      const Scalar x,
      const Bool tail = false);
  // Natural logarithm of the beta function
  static Scalar LnBeta(const Scalar a,
                       const Scalar b);
  static Scalar LogBeta(const Scalar a,
                        const Scalar b);
  // Dawson function: Dawson(x) = \exp(-x^2) * \int_0^x \exp(t^2) dt
  static Scalar Dawson(const Scalar x);
  static Complex Dawson(const Complex & z);
  // Debye function of order n: DebyeN(x, n) = n / x^n \int_0^x t^n/(\exp(t)-1) dt
  static Scalar Debye(const Scalar x,
                      const UnsignedInteger n);
  // DiLog function: Dilog(x) = -\int_0^x \log(1-t)/t dt
  static Scalar DiLog(const Scalar x);
  // Exponential integral function: Ei(x) = -\int_{-x}^{\infty}exp(-t)/t dt
  static Scalar Ei(const Scalar x);
  // Complex exponential integral function: Ei(z) = -\int_{-z}^{\infty}exp(-t)/t dt
  static Complex Ei(const Complex & z);
  // Complex Faddeeva function: Faddeeva(z) = exp(-z^2)\erfc(-I*z)
  static Complex Faddeeva(const Complex & z);
  // Imaginary part of the Faddeeva function: FaddeevaIm(z) = Im(Faddeeva(x))
  static Scalar FaddeevaIm(const Scalar x);
  // Factorial and log-Factorial functions
  static Scalar Factorial(UnsignedInteger n);
  static Scalar LogFactorial(UnsignedInteger n);
  // Gamma function: gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t) dt
  static Scalar Gamma(const Scalar a);
  // igamma1pm1(a) = 1 / gamma(1 + a) - 1
  static Scalar IGamma1pm1(const Scalar a);
  // GammaCorrection(a) = LogGamma(a) - log(sqrt(2.Pi)) + a - (a - 1/2) log(a)
  static Scalar GammaCorrection(const Scalar a);
  // Complex gamma function: gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t) dt
  static Complex Gamma(const Complex & a);
  // Natural logarithm of the gamma function
  static Scalar LnGamma(const Scalar a);
  static Scalar LogGamma(const Scalar a);
  static Scalar LogGamma1p(const Scalar a);
  static Complex LogGamma(const Complex & a);
  // Incomplete gamma function: gamma(a, x) = \int_0^x t^{a-1}\exp(-t) dt
  static Scalar IncompleteGamma(const Scalar a,
                                const Scalar x,
                                const Bool tail = false);
  // Incomplete gamma function inverse with respect to x
  static Scalar IncompleteGammaInverse(const Scalar a,
                                       const Scalar x,
                                       const Bool tail = false);
  // Regularized incomplete gamma function: gamma(a, x) = \int_0^x t^{a-1}\exp(-t) dt / \Gamma(a)
  static Scalar RegularizedIncompleteGamma(const Scalar a,
      const Scalar x,
      const Bool tail = false);
  // Regularized incomplete gamma function inverse with respect to x
  static Scalar RegularizedIncompleteGammaInverse(const Scalar a,
      const Scalar x,
      const Bool tail = false);
  // Digamma function: psi(x) = ((dgamma/dx) / gamma)(x)
  static Scalar DiGamma(const Scalar x);
  static Scalar Psi(const Scalar x);
  // Inverse of the DiGamma function
  static Scalar DiGammaInv(const Scalar a);
  // Trigamma function: TriGamma(x) = ((d^2gamma/dx^2) / gamma)(x)
  static Scalar TriGamma(const Scalar x);
  // Stirling error: Stirlerr(n) = log(n!) - log( sqrt(2*pi*n)*(n/e)^n )
  static Scalar Stirlerr(const UnsignedInteger n);
  // Hypergeometric function of type (1,1): hyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
  static Scalar HyperGeom_1_1(const Scalar p1,
                              const Scalar q1,
                              const Scalar x);
  // Complex hypergeometric function of type (1,1): hyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
  static Complex HyperGeom_1_1(const Scalar p1,
                               const Scalar q1,
                               const Complex & x);
  // Hypergeometric function of type (2,1): hyperGeom_2_1(p1, p2, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k)] * x^n / n!
  static Scalar HyperGeom_2_1(const Scalar p1,
                              const Scalar p2,
                              const Scalar q1,
                              const Scalar x);
  // Hypergeometric function of type (2,2): hyperGeom_2_1(p1, p2, q1, q2, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k) / (q2 + k)] * x^n / n!
  static Scalar HyperGeom_2_2(const Scalar p1,
                              const Scalar p2,
                              const Scalar q1,
                              const Scalar q2,
                              const Scalar x);
  // Erf function erf(x) = 2 / \sqrt(\pi) . \int_0^x \exp(-t^2) dt
  static Scalar Erf(const Scalar x);
  static Complex Erf(const Complex & z);
  // Erf function erfi(x) = -i.erf(iz)
  static Scalar ErfI(const Scalar x);
  static Complex ErfI(const Complex & z);
  // Erf function erfc(x) = 1 - erf(x)
  static Scalar ErfC(const Scalar x);
  static Complex ErfC(const Complex & z);
  // Erf function erfcx(x) = exp(x^2).erfc(x)
  static Scalar ErfCX(const Scalar x);
  static Complex ErfCX(const Complex & z);
  // Inverse of the erf function
  static Scalar ErfInverse(const Scalar x);
  // Real branch of Lambert W function (principal or secndary)
  static Scalar LambertW(const Scalar x,
                         const Bool principal = true);
  // Accurate value of log(1+z) for |z|<<1
  static Complex Log1p(const Complex & z);
  // Accurate value of exp(z)-1 for |z|<<1
  static Complex Expm1(const Complex & z);
  // Accurate value of log(1-exp(-x)) for all x
  static Complex Log1MExp(const Scalar x);
  // MarcumQ- function
  //      static Scalar MarcumQFunction(const Scalar a,const Scalar b);

  // Next power of two
  static UnsignedInteger NextPowerOfTwo(const UnsignedInteger n);

  // Integer power
  static Scalar IPow(const Scalar x, const SignedInteger n);

  // Integer root
  static Scalar IRoot(const Scalar x, const SignedInteger n);

  // Integer log2
  static UnsignedInteger Log2(const Unsigned64BitsInteger n);

  // Compute the number of bits sets to 1 in n
  // Best known algorithm for 64 bits n and fast multiply
  static UnsignedInteger BitCount(const Unsigned64BitsInteger n);

  // Missing functions in cmath wrt math.h as of C++98
  static Scalar Acosh(const Scalar x);
  static Scalar Asinh(const Scalar x);
  static Scalar Atanh(const Scalar x);
  static Scalar Cbrt(const Scalar x);

  // binomial coefficient C(n, k)
  static UnsignedInteger BinomialCoefficient(const UnsignedInteger n,
      const UnsignedInteger k);
}; /* class SpecFunc */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECFUNC_HXX */
