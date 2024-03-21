//                                               -*- C++ -*-
/**
 *  @brief OpenTURNS wrapper to a library of special functions
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
#ifndef OPENTURNS_SPECFUNC_HXX
#define OPENTURNS_SPECFUNC_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"
#include <limits>

BEGIN_NAMESPACE_OPENTURNS

namespace SpecFunc
{

// 0.39894228040143267 = 1 / sqrt(2.pi)
const Scalar ISQRT2PI              = 0.3989422804014326779399462;
// 2.5066282746310005024 = sqrt(2.pi)
const Scalar SQRT2PI               = 2.506628274631000502415765;
// 0.91893853320467274177 = log(sqrt(2.pi))
const Scalar LOGSQRT2PI            = 0.91893853320467274178;
// 0.57721566490153286 = Euler constant gamma
const Scalar EulerConstant         = 0.57721566490153286;
// 1.64493406684822643 = pi^2 / 6
const Scalar PI2_6                 = 1.64493406684822643;
// 1.28254983016118640 = pi / sqrt(6)
const Scalar PI_SQRT6              = 1.28254983016118640;
// 0.45005320754569466 = gamma * sqrt(6) / pi
const Scalar EULERSQRT6_PI         = 0.45005320754569466;
// 3.28986813369645287 = pi^2 / 3
const Scalar PI2_3                 = 3.28986813369645287;
// 0.55132889542179204 = sqrt(3) / pi
const Scalar SQRT3_PI              = 0.55132889542179204;
// 1.81379936423421785 = pi / sqrt(3)
const Scalar PI_SQRT3              = 1.81379936423421785;
// 6.283185307179586476925286 = 2*pi
const Scalar TWOPI                 = 6.283185307179586476925286;
// 1.20205690315959429 = Zeta(3)
const Scalar ZETA3                 = 1.20205690315959429;
// Scalar limits
const Scalar MinScalar     = std::numeric_limits<Scalar>::min();
const Scalar LogMinScalar  = std::log(MinScalar);
#ifdef OPENTURNS_MAXSCALAR_IS_INF
const Scalar MaxScalar     = std::numeric_limits<Scalar>::infinity();
#else
const Scalar MaxScalar     = std::numeric_limits<Scalar>::max();
#endif
const Scalar LogMaxScalar  = std::log(MaxScalar);
const Scalar ActualMaxScalar = std::numeric_limits<Scalar>::max();
const Scalar LowestScalar  = -MaxScalar;
const Scalar ScalarEpsilon = std::numeric_limits<Scalar>::epsilon();
// Maximum number of iterations for the algorithms
const UnsignedInteger MaximumIteration = ResourceMap::GetAsUnsignedInteger("SpecFunc-MaximumIteration");
const Scalar Precision = ResourceMap::GetAsScalar("SpecFunc-Precision");

// Some facilities for NaN and inf
OT_API Bool IsNormal(const Scalar value);

// Modified first kind Bessel function of order 0: BesselI0(x) = \sum_{m=0}\infty\frac{1}{m!^2}\left(\frac{x}{2}\right)^{2m}
OT_API Scalar BesselI0(const Scalar x);
OT_API Scalar LogBesselI0(const Scalar x);
// Modified first kind Bessel function of order 1: BesselI1(x) = \sum_{m=0}\infty\frac{1}{m!(m+1)!}\left(\frac{x}{2}\right)^{2m+1}
OT_API Scalar BesselI1(const Scalar x);
OT_API Scalar LogBesselI1(const Scalar x);
// Difference between the logarithms of BesselI1 and BesselI0:
OT_API Scalar DeltaLogBesselI10(const Scalar x);
// Modified second kind Bessel function of order nu: BesselK(nu, x)=\frac{\pi}{2}\frac{I_{-\nu}(x)-I_[\nu}(x)}{\sin{\nu\pi}}
OT_API Scalar LogBesselK(const Scalar nu,
                         const Scalar x);
OT_API Scalar BesselK(const Scalar nu,
                      const Scalar x);
OT_API Scalar BesselKDerivative(const Scalar nu,
                                const Scalar x);
// Beta function: beta(a, b) = \int_0^1 t^{a-1}.(1-t)^{b-1} dt
OT_API Scalar Beta(const Scalar a,
                   const Scalar b);
// Incomplete beta function: betaInc(a, b, x) = \int_0^x t^{a-1}.(1-t)^{b-1} dt
OT_API Scalar IncompleteBeta(const Scalar a,
                             const Scalar b,
                             const Scalar x,
                             const Bool tail = false);
// Incomplete beta function inverse with respect to x
OT_API Scalar IncompleteBetaInverse(const Scalar a,
                                    const Scalar b,
                                    const Scalar x,
                                    const Bool tail = false);
// Incomplete beta ratio function: betaRatioInc(a, b, x) = \int_0^x t^{a-1}.(1-t)^{b-1} dt / beta(a, b)
OT_API Scalar RegularizedIncompleteBeta(const Scalar a,
                                        const Scalar b,
                                        const Scalar x,
                                        const Bool tail = false);
// Incomplete beta ratio function inverse with respect to x
OT_API Scalar RegularizedIncompleteBetaInverse(const Scalar a,
    const Scalar b,
    const Scalar x,
    const Bool tail = false);
// Natural logarithm of the beta function
OT_API Scalar LogBeta(const Scalar a,
                      const Scalar b);
// Dawson function: Dawson(x) = \exp(-x^2) * \int_0^x \exp(t^2) dt
OT_API Scalar Dawson(const Scalar x);
OT_API Complex Dawson(const Complex & z);
// Debye function of order n: DebyeN(x, n) = n / x^n \int_0^x t^n/(\exp(t)-1) dt
OT_API Scalar Debye(const Scalar x,
                    const UnsignedInteger n);
// DiLog function: Dilog(x) = -\int_0^x \log(1-t)/t dt
OT_API Scalar DiLog(const Scalar x);
// Exponential integral function: Ei(x) = -\int_{-x}^{\infty}exp(-t)/t dt
OT_API Scalar Ei(const Scalar x);
// Complex exponential integral function: Ei(z) = -\int_{-z}^{\infty}exp(-t)/t dt
OT_API Complex Ei(const Complex & z);
// Complex Faddeeva function: Faddeeva(z) = exp(-z^2)\erfc(-I*z)
OT_API Complex Faddeeva(const Complex & z);
// Imaginary part of the Faddeeva function: FaddeevaIm(z) = Im(Faddeeva(x))
OT_API Scalar FaddeevaIm(const Scalar x);
// Factorial and log-Factorial functions
OT_API Scalar Factorial(UnsignedInteger n);
OT_API Scalar LogFactorial(UnsignedInteger n);
// Gamma function: gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t) dt
OT_API Scalar Gamma(const Scalar a);
// igamma1pm1(a) = 1 / gamma(1 + a) - 1
OT_API Scalar IGamma1pm1(const Scalar a);
// GammaCorrection(a) = LogGamma(a) - log(sqrt(2.Pi)) + a - (a - 1/2) log(a)
OT_API Scalar GammaCorrection(const Scalar a);
// Complex gamma function: gamma(a) = \int_0^{\infty} t^{a-1}\exp(-t) dt
OT_API Complex Gamma(const Complex & a);
// Natural logarithm of the gamma function
OT_API Scalar LogGamma(const Scalar a);
OT_API Scalar LogGamma1p(const Scalar a);
OT_API Complex LogGamma(const Complex & a);
// Incomplete gamma function: gamma(a, x) = \int_0^x t^{a-1}\exp(-t) dt
OT_API Scalar IncompleteGamma(const Scalar a,
                              const Scalar x,
                              const Bool tail = false);
// Incomplete gamma function inverse with respect to x
OT_API Scalar IncompleteGammaInverse(const Scalar a,
                                     const Scalar x,
                                     const Bool tail = false);
// Regularized incomplete gamma function: gamma(a, x) = \int_0^x t^{a-1}\exp(-t) dt / \Gamma(a)
OT_API Scalar RegularizedIncompleteGamma(const Scalar a,
    const Scalar x,
    const Bool tail = false);
// Regularized incomplete gamma function inverse with respect to x
OT_API Scalar RegularizedIncompleteGammaInverse(const Scalar a,
    const Scalar x,
    const Bool tail = false);
// Digamma function: psi(x) = ((dgamma/dx) / gamma)(x)
OT_API Scalar DiGamma(const Scalar x);
OT_API Complex DiGamma(const Complex & a);
OT_API Scalar Psi(const Scalar x);
OT_API Complex Psi(const Complex & a);
// Inverse of the DiGamma function
OT_API Scalar DiGammaInv(const Scalar a);
// Trigamma function: TriGamma(x) = ((d^2gamma/dx^2) / gamma)(x)
OT_API Scalar TriGamma(const Scalar x);
// Stirling error: Stirlerr(n) = log(n!) - log( sqrt(2*pi*n)*(n/e)^n )
OT_API Scalar Stirlerr(const UnsignedInteger n);
// Hypergeometric function of type (1,1): hyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
OT_API Scalar HyperGeom_1_1(const Scalar p1,
                            const Scalar q1,
                            const Scalar x);
// Complex hypergeometric function of type (1,1): hyperGeom_1_1(p1, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) / (q1 + k)] * x^n / n!
OT_API Complex HyperGeom_1_1(const Scalar p1,
                             const Scalar q1,
                             const Complex & x);
// Hypergeometric function of type (2,1): hyperGeom_2_1(p1, p2, q1, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k)] * x^n / n!
OT_API Scalar HyperGeom_2_1(const Scalar p1,
                            const Scalar p2,
                            const Scalar q1,
                            const Scalar x);
// Hypergeometric function of type (2,2): hyperGeom_2_1(p1, p2, q1, q2, x) = \sum_{n=0}^{\infty} [\prod_{k=0}^{n-1} (p1 + k) . (p2 + k) / (q1 + k) / (q2 + k)] * x^n / n!
OT_API Scalar HyperGeom_2_2(const Scalar p1,
                            const Scalar p2,
                            const Scalar q1,
                            const Scalar q2,
                            const Scalar x);
// Erf function erf(x) = 2 / \sqrt(\pi) . \int_0^x \exp(-t^2) dt
OT_API Scalar Erf(const Scalar x);
OT_API Complex Erf(const Complex & z);
// Erf function erfi(x) = -i.erf(iz)
OT_API Scalar ErfI(const Scalar x);
OT_API Complex ErfI(const Complex & z);
// Erf function erfc(x) = 1 - erf(x)
OT_API Scalar ErfC(const Scalar x);
OT_API Complex ErfC(const Complex & z);
// Erf function erfcx(x) = exp(x^2).erfc(x)
OT_API Scalar ErfCX(const Scalar x);
OT_API Complex ErfCX(const Complex & z);
// Inverse of the erf function
OT_API Scalar ErfInverse(const Scalar x);
// Real branch of Lambert W function (principal or secndary)
OT_API Scalar LambertW(const Scalar x,
                       const Bool principal = true);
// Accurate value of log(1+z) for |z|<<1
OT_API Complex Log1p(const Complex & z);
// Accurate value of exp(z)-1 for |z|<<1
OT_API Complex Expm1(const Complex & z);
// Accurate value of log(1-exp(-x)) for all x
OT_API Complex Log1MExp(const Scalar x);
// MarcumQ- function
//      Scalar MarcumQFunction(const Scalar a,const Scalar b);

// Next power of two
OT_API UnsignedInteger NextPowerOfTwo(const UnsignedInteger n);

// Integer power
OT_API Scalar IPow(const Scalar x, const SignedInteger n);

// Integer root
OT_API Scalar IRoot(const Scalar x, const SignedInteger n);

// Integer log2
OT_API UnsignedInteger Log2(const Unsigned64BitsInteger n);

// Compute the number of bits sets to 1 in n
// Best known algorithm for 64 bits n and fast multiply
OT_API UnsignedInteger BitCount(const Unsigned64BitsInteger n);

// binomial coefficient C(n, k)
OT_API UnsignedInteger BinomialCoefficient(const UnsignedInteger n,
    const UnsignedInteger k);

// Accurate summation
OT_API Scalar AccurateSum(const Point & v);

// clip value on [0, 1]
OT_API Scalar Clip01(const Scalar prob, const Bool tail = false);

} /* SpecFunc */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SPECFUNC_HXX */
