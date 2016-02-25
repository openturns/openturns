//                                               -*- C++ -*-
/**
 *  @brief OpenTURNS wrapper to a library of special functions
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
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
  static const NumericalScalar ISQRT2PI;
  // 2.5066282746310005024 = sqrt(2.pi)
  static const NumericalScalar SQRT2PI;
  // 0.91893853320467274177 = log(sqrt(2.pi))
  static const NumericalScalar LOGSQRT2PI;
  // 0.57721566490153286 = Euler constant gamma
  static const NumericalScalar EulerConstant;
  // 1.64493406684822643 = pi^2 / 6
  static const NumericalScalar PI2_6;
  // 1.28254983016118640 = pi / sqrt(6)
  static const NumericalScalar PI_SQRT6;
  // 0.45005320754569466 = gamma * sqrt(6) / pi
  static const NumericalScalar EULERSQRT6_PI;
  // 3.28986813369645287 = pi^2 / 3
  static const NumericalScalar PI2_3;
  // 0.55132889542179204 = sqrt(3) / pi
  static const NumericalScalar SQRT3_PI;
  // 1.81379936423421785 = pi / sqrt(3)
  static const NumericalScalar PI_SQRT3;
  // 1.20205690315959429 = Zeta(3)
  static const NumericalScalar ZETA3;
  // Maximum number of iterations for algorithms
  static const UnsignedInteger MaximumIteration;
  // Maximum precision for algorithms
  static const NumericalScalar Precision;
  // Minimum positive real number
  static const NumericalScalar MinNumericalScalar;
  static const NumericalScalar LogMinNumericalScalar;
  // Maximum positive real number
  static const NumericalScalar MaxNumericalScalar;
  static const NumericalScalar LogMaxNumericalScalar;
  // Real number accuracy
  static const NumericalScalar NumericalScalarEpsilon;

  // Some facilities for NaN and inf
  static Bool IsNaN(const NumericalScalar value);
  static Bool IsInf(const NumericalScalar value);
  static Bool IsNormal(const NumericalScalar value);

  // Modified first kind Bessel function of order 0: BesselI0(x) = \sum_{m=0}\infty\frac{1}{m!^2}\left(\frac{x}{2}\right)^{2m}
private:
  static NumericalScalar SmallCaseBesselI0(const NumericalScalar x);
  static NumericalScalar LargeCaseLogBesselI0(const NumericalScalar x);
public:
  static NumericalScalar BesselI0(const NumericalScalar x);
  static NumericalScalar LogBesselI0(const NumericalScalar x);
  // Modified first kind Bessel function of order 1: BesselI1(x) = \sum_{m=0}\infty\frac{1}{m!(m+1)!}\left(\frac{x}{2}\right)^{2m+1}
private:
  static NumericalScalar SmallCaseBesselI1(const NumericalScalar x);
  static NumericalScalar LargeCaseLogBesselI1(const NumericalScalar x);
public:
  static NumericalScalar BesselI1(const NumericalScalar x);
  static NumericalScalar LogBesselI1(const NumericalScalar x);
  // Difference between the logarithms of BesselI1 and BesselI0:
  // DeltaLogBesselI10(x) = log(BesselI1(x)) - log(BesselI0(x))
private:
  static NumericalScalar LargeCaseDeltaLogBesselI10(const NumericalScalar x);
public:
  static NumericalScalar DeltaLogBesselI10(const NumericalScalar x);
  // Modified second kind Bessel function of order nu: BesselK(nu, x)=\frac{\pi}{2}\frac{I_{-\nu}(x)-I_[\nu}(x)}{\sin{\nu\pi}}
  static NumericalScalar LogBesselK(const NumericalScalar nu,
                                    const NumericalScalar x);
  static NumericalScalar BesselK(const NumericalScalar nu,
                                 const NumericalScalar x);
  static NumericalScalar BesselKDerivative(const NumericalScalar nu,
      const NumericalScalar x);
  // Beta function: beta(a, b) = \int_0^1 t^{a-1}.(1-t)^{b-1} dt
  static NumericalScalar Beta(const NumericalScalar a,
                              const NumericalScalar b);
  // Incomplete beta function: betaInc(a, b, x) = \int_0^x t^{a-1}.(1-t)^{b-1} dt
  static NumericalScalar IncompleteBeta(const NumericalScalar a,
                                        const NumericalScalar b,
                                        const NumericalScalar x,
                                        const Bool tail = false);
  // Incomplete beta function inverse with respect to x
  static NumericalScalar IncompleteBetaInverse(const NumericalScalar a,
      const NumericalScalar b,
      const NumericalScalar x,
      const Bool tail = false);
  // Incomplete beta ratio function: betaRatioInc(a, b, x) = \int_0^x t^{a-1}.(1-t)^{b-1} dt / beta(a, b)
  static NumericalScalar RegularizedIncompleteBeta(const NumericalScalar a,
      const NumericalScalar b,
      const NumericalScalar x,
      const Bool tail = false);
  // Incomplete beta ratio function inverse with respect to x
  static NumericalScalar RegularizedIncompleteBetaInverse(const NumericalScalar a,
      const NumericalScalar b,
      const NumericalScalar x,
      const Bool tail = false);
  // Natural logarithm of the beta function
  static NumericalScalar LnBeta(const NumericalScalar a,
                                const NumericalScalar b);
  static NumericalScalar LogBeta(const NumericalScalar a,
                                 const NumericalScalar b);
  // Dawson function: Dawson(x) = \exp(-x^2) * \int_0^x \exp(t^2) dt
  static NumericalScalar Dawson(const NumericalScalar x);
  static NumericalComplex Dawson(const NumericalComplex & z);
  // Debye function of order n: DebyeN(x, n) = n / x^n \int_0^x t^n/(\exp(t)-1) dt
  static NumericalScalar Debye(const NumericalScalar x,
                               const UnsignedInteger n);
  // DiLog function: Dilog(x) = -\int_0^x \log(1-t)/t dt
  static NumericalScalar DiLog(const NumericalScalar x);
  // Exponential integral function: Ei(x) = -\int_{-x}^{\infty}exp(-t)/t dt
  static NumericalScalar Ei(const NumericalScalar x);
  // Complex exponential integral function: Ei(z) = -\int_{-z}^{\infty}exp(-t)/t dt
  static NumericalComplex Ei(const NumericalComplex & z);
  // Complex Faddeeva function: Faddeeva(z) = exp(-z^2)\erfc(-I*z)
  static NumericalComplex Faddeeva(const NumericalComplex & z);
  // Imaginary part of the Faddeeva function: FaddeevaIm(z) = Im(Faddeeva(x))
  static NumericalScalar FaddeevaIm(const NumericalScalar x);
  // Gamma function: gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t) dt
  static NumericalScalar Gamma(const NumericalScalar a);
  // igamma1pm1(a) = 1 / gamma(1 + a) - 1
  static NumericalScalar IGamma1pm1(const NumericalScalar a);
  // GammaCorrection(a) = LogGamma(a) - log(sqrt(2.Pi)) + a - (a - 1/2) log(a)
  static NumericalScalar GammaCorrection(const NumericalScalar a);
  // Complex gamma function: gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t) dt
  static NumericalComplex Gamma(const NumericalComplex & a);
  // Natural logarithm of the gamma function
  static NumericalScalar LnGamma(const NumericalScalar a);
  static NumericalScalar LogGamma(const NumericalScalar a);
  static NumericalScalar LogGamma1p(const NumericalScalar a);
  static NumericalComplex LogGamma(const NumericalComplex & a);
  // Incomplete gamma function: gamma(a, x) = \int_0^x t^{a-1}\exp(-t) dt
  static NumericalScalar IncompleteGamma(const NumericalScalar a,
                                         const NumericalScalar x,
                                         const Bool tail = false);
  // Incomplete gamma function inverse with respect to x
  static NumericalScalar IncompleteGammaInverse(const NumericalScalar a,
      const NumericalScalar x,
      const Bool tail = false);
  // Regularized incomplete gamma function: gamma(a, x) = \int_0^x t^{a-1}\exp(-t) dt / \Gamma(a)
  static NumericalScalar RegularizedIncompleteGamma(const NumericalScalar a,
      const NumericalScalar x,
      const Bool tail = false);
  // Regularized incomplete gamma function inverse with respect to x
  static NumericalScalar RegularizedIncompleteGammaInverse(const NumericalScalar a,
      const NumericalScalar x,
      const Bool tail = false);
  // Digamma function: psi(x) = ((dgamma/dx) / gamma)(x)
  static NumericalScalar DiGamma(const NumericalScalar x);
  static NumericalScalar Psi(const NumericalScalar x);
  // Inverse of the DiGamma function
  static NumericalScalar DiGammaInv(const NumericalScalar a);
  // Trigamma function: TriGamma(x) = ((d^2gamma/dx^2) / gamma)(x)
  static NumericalScalar TriGamma(const NumericalScalar x);
  // Hypergeometric function of type (1,1): hyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
  static NumericalScalar HyperGeom_1_1(const NumericalScalar p1,
                                       const NumericalScalar q1,
                                       const NumericalScalar x);
  // Complex hypergeometric function of type (1,1): hyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
  static NumericalComplex HyperGeom_1_1(const NumericalScalar p1,
                                        const NumericalScalar q1,
                                        const NumericalComplex & x);
  // Hypergeometric function of type (2,1): hyperGeom_2_1(p1, p2, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k)] * x^n / n!
  static NumericalScalar HyperGeom_2_1(const NumericalScalar p1,
                                       const NumericalScalar p2,
                                       const NumericalScalar q1,
                                       const NumericalScalar x);
  // Hypergeometric function of type (2,2): hyperGeom_2_1(p1, p2, q1, q2, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k) / (q2 + k)] * x^n / n!
  static NumericalScalar HyperGeom_2_2(const NumericalScalar p1,
                                       const NumericalScalar p2,
                                       const NumericalScalar q1,
                                       const NumericalScalar q2,
                                       const NumericalScalar x);
  // Erf function erf(x) = 2 / \sqrt(\pi) . \int_0^x \exp(-t^2) dt
  static NumericalScalar Erf(const NumericalScalar x);
  static NumericalComplex Erf(const NumericalComplex & z);
  // Erf function erfi(x) = -i.erf(iz)
  static NumericalScalar ErfI(const NumericalScalar x);
  static NumericalComplex ErfI(const NumericalComplex & z);
  // Erf function erfc(x) = 1 - erf(x)
  static NumericalScalar ErfC(const NumericalScalar x);
  static NumericalComplex ErfC(const NumericalComplex & z);
  // Erf function erfcx(x) = exp(x^2).erfc(x)
  static NumericalScalar ErfCX(const NumericalScalar x);
  static NumericalComplex ErfCX(const NumericalComplex & z);
  // Inverse of the erf function
  static NumericalScalar ErfInverse(const NumericalScalar x);
  // Real branch of Lambert W function (principal or secndary)
  static NumericalScalar LambertW(const NumericalScalar x,
                                  const Bool principal = true);
  // Accurate value of log(1+z) for |z|<<1
  static NumericalComplex Log1p(const NumericalComplex & z);
  // Accurate value of exp(z)-1 for |z|<<1
  static NumericalComplex Expm1(const NumericalComplex & z);
  // Accurate value of log(1-exp(-x)) for all x
  static NumericalComplex Log1MExp(const NumericalScalar x);
  // MarcumQ- function
  //      static NumericalScalar MarcumQFunction(const NumericalScalar a,const NumericalScalar b);

  // Next power of two
  static UnsignedInteger NextPowerOfTwo(const UnsignedInteger n);

  // Compute the number of bits sets to 1 in n
  // Best known algorithm for 64 bits n and fast multiply
  static UnsignedInteger BitCount(const Unsigned64BitsInteger n);

  // Missing functions in cmath wrt math.h as of C++98
  static NumericalScalar Acosh(const NumericalScalar x);
  static NumericalScalar Asinh(const NumericalScalar x);
  static NumericalScalar Atanh(const NumericalScalar x);
  static NumericalScalar Cbrt(const NumericalScalar x);

}; /* class SpecFunc */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECFUNC_HXX */
