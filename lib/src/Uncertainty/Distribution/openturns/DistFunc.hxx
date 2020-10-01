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
#ifndef OPENTURNS_DISTFUNC_HXX
#define OPENTURNS_DISTFUNC_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Point.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API DistFunc
{
public:
  static const UnsignedInteger NumberOfBandNormalZigurrat;
  static const Scalar NormalZigguratTail;
  // The array NormalZigguratAbscissa stores the abscissas of the bands:
  // the ith band is [NormalZigguratAbscissa[0], NormalZigguratAbscissa[i+1]] with
  // NormalZigguratAbscissa[0] = 0.0 and
  // NormalZigguratAbscissa[NumberOfBandNormalZigurrat] = NormalZigguratTail
  static const Scalar NormalZigguratAbscissa[];
  // The array NormalZigguratRatio stores the ratio between a band and the next one,
  // starting from the smallest one:
  // NormalZigguratRatio[i] = NormalZigguratAbscissa[i] / NormalZigguratAbscissa[i+1]
  static const Scalar NormalZigguratRatio[];

  // For Beta distribution. WARNING: here we adopt the classical definition
  // If tail=false, compute P(X<=x), else P(X>x)
  static Scalar pBeta(const Scalar p1,
                      const Scalar p2,
                      const Scalar x,
                      const Bool tail = false);
  // If tail=false, compute x such that P(X<=x)=q, else x such that P(X>x)=q
  static Scalar qBeta(const Scalar p1,
                      const Scalar p2,
                      const Scalar p,
                      const Bool tail = false);
  static Scalar rBeta(const Scalar p1,
                      const Scalar p2);
  static Point rBeta(const Scalar p1,
                     const Scalar p2,
                     const UnsignedInteger size);
  // For the Binomial distribution
private:
  static Scalar bd0(const UnsignedInteger k,
                    const Scalar np);

public:
  static Scalar logdBinomial(const UnsignedInteger n,
                             const Scalar p,
                             const UnsignedInteger k);
  static Scalar dBinomial(const UnsignedInteger n,
                          const Scalar p,
                          const UnsignedInteger k);
  static UnsignedInteger rBinomial(const UnsignedInteger n,
                                   const Scalar p);
  static Indices rBinomial(const UnsignedInteger n,
                           const Scalar p,
                           const UnsignedInteger size);
  // For discrete distribution
#ifndef SWIG
  static UnsignedInteger rDiscrete(const Point & probabilities,
                                   Point & base,
                                   Indices & alias);
  static Indices rDiscrete(const Point & probabilities,
                           Point & base,
                           Indices & alias,
                           const UnsignedInteger size);
  static UnsignedInteger rDiscrete(const Point & base,
                                   const Indices & alias);
  static Indices rDiscrete(const Point & base,
                           const Indices & alias,
                           const UnsignedInteger size);
#endif
  static UnsignedInteger rDiscrete(const Point & probabilities);
  static Indices rDiscrete(const Point & probabilities,
                           const UnsignedInteger size);
  // For Gamma distribution
  static Scalar pGamma(const Scalar k,
                       const Scalar x,
                       const Bool tail = false);
  static Scalar qGamma(const Scalar k,
                       const Scalar p,
                       const Bool tail = false);
  static Scalar rGamma(const Scalar k);
  static Point rGamma(const Scalar k,
                      const UnsignedInteger size);

  // For Hypergeometric distribution
  static Scalar dHypergeometric(const UnsignedInteger n,
                                const UnsignedInteger k,
                                const UnsignedInteger m,
                                const UnsignedInteger x);
  static Scalar logdHypergeometric(const UnsignedInteger n,
                                   const UnsignedInteger k,
                                   const UnsignedInteger m,
                                   const UnsignedInteger x);
  static Scalar pHypergeometric(const UnsignedInteger n,
                                const UnsignedInteger k,
                                const UnsignedInteger m,
                                const UnsignedInteger x,
                                const Bool tail = false);
  static UnsignedInteger rHypergeometric(const UnsignedInteger n,
                                         const UnsignedInteger k,
                                         const UnsignedInteger m);
  static Indices rHypergeometric(const UnsignedInteger n,
                                 const UnsignedInteger k,
                                 const UnsignedInteger m,
                                 const UnsignedInteger size);
  // For Kolmogorov distribution
  static Scalar pKolmogorov(const UnsignedInteger n,
                            const Scalar x,
                            const Bool tail = false);

  // For NonCentralChiSquare distribution
  static Scalar dNonCentralChiSquare(const Scalar nu,
                                     const Scalar lambda,
                                     const Scalar x,
                                     const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
                                     const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"));
  static Scalar pNonCentralChiSquare(const Scalar nu,
                                     const Scalar lambda,
                                     const Scalar x,
                                     const Bool tail = false,
                                     const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
                                     const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"));
  static Scalar rNonCentralChiSquare(const Scalar nu,
                                     const Scalar lambda);
  static Point rNonCentralChiSquare(const Scalar nu,
                                    const Scalar lambda,
                                    const UnsignedInteger size);
  // For NonCentralStudent distribution
  static Scalar dNonCentralStudent(const Scalar nu,
                                   const Scalar delta,
                                   const Scalar x);
  static Scalar dNonCentralStudentAlt0(const Scalar nu,
                                       const Scalar delta,
                                       const Scalar x,
                                       const Scalar precision = ResourceMap::GetAsScalar("DistFunc-Precision"),
                                       const UnsignedInteger maximumIteration = ResourceMap::GetAsUnsignedInteger("DistFunc-MaximumIteration"));
  static Scalar pNonCentralStudent(const Scalar nu,
                                   const Scalar delta,
                                   const Scalar x,
                                   const Bool tail = false);
  static Scalar rNonCentralStudent(const Scalar nu,
                                   const Scalar delta);
  static Point rNonCentralStudent(const Scalar nu,
                                  const Scalar delta,
                                  const UnsignedInteger size);
  // For Normal distribution
  static Scalar pNormal(const Scalar x,
                        const Bool tail = false);
  static Point pNormal(const Point & x,
                       const Bool tail = false);
  static Scalar pNormal2D(const Scalar x1,
                          const Scalar x2,
                          const Scalar rho,
                          const Bool tail = false);
  static Scalar pNormal3D(const Scalar x1,
                          const Scalar x2,
                          const Scalar x3,
                          const Scalar rho12,
                          const Scalar rho13,
                          const Scalar rho23,
                          const Bool tail = false);
  static Scalar qNormal(const Scalar p,
                        const Bool tail = false);
  static Point qNormal(const Point & p,
                       const Bool tail = false);
  static Scalar rNormal();
  static Point rNormal(const UnsignedInteger size);

  static Scalar dNormal(const Scalar x);
  static Point dNormal(const Point &x);

  // For Poisson distribution
  static Scalar logdPoisson(const Scalar lambda,
                            const UnsignedInteger k );
  static Scalar dPoisson(const Scalar lambda,
                         const UnsignedInteger k);
  static Scalar qPoisson(const Scalar lambda,
                         const Scalar p,
                         const Bool tail = false);
  static UnsignedInteger rPoisson(const Scalar lambda);
  static Indices rPoisson(const Scalar lambda,
                          const UnsignedInteger size);

  // For Pearson test
  static Scalar pPearsonCorrelation(const UnsignedInteger size, const Scalar rho, const Bool tail = false);

  // For Spearman test
  static Scalar pSpearmanCorrelation(const UnsignedInteger size, const Scalar rho, const Bool tail = false, const Bool ties = false);
  // For Student distribution
  static Scalar pStudent(const Scalar nu,
                         const Scalar x,
                         const Bool tail = false);
  static Point pStudent(const Scalar nu,
                        const Point & x,
                        const Bool tail = false);
  static Scalar qStudent(const Scalar nu,
                         const Scalar p,
                         const Bool tail = false);
  static Point qStudent(const Scalar nu,
                        const Point & p,
                        const Bool tail = false);
  static Scalar rStudent(const Scalar nu);
  static Point rStudent(const Scalar nu,
                        const UnsignedInteger size);
  // For uniform distribution over a triangle
  static Point rUniformTriangle(const Point & a,
                                const Point & b,
                                const Point & c);
  static Sample rUniformTriangle(const Point & a,
                                 const Point & b,
                                 const Point & c,
                                 const UnsignedInteger size);
  // For TruncatedNormal distribution
  //      static Scalar rTruncatedNormal(const Scalar a, const Scalar b);
  // Compute the expectation of the min of n independent standard normal random variables
  static Scalar eZ1(const UnsignedInteger n);

  // K factor for exact two-sided tolerance intervals of normal pooled populations
  static Scalar kFactorPooled(const UnsignedInteger n,
                              const UnsignedInteger m,
                              const Scalar p,
                              const Scalar alpha);

  // K factor for exact two-sided tolerance intervals of a normal population
  static Scalar kFactor(const UnsignedInteger n,
                        const Scalar p,
                        const Scalar alpha);

  // Asymptotic distribution of the DickeyFuller distribution
  static Scalar pDickeyFullerTrend(const Scalar x,
                                   const Bool tail = false);

  static Scalar pDickeyFullerConstant(const Scalar x,
                                      const Bool tail = false);

  static Scalar pDickeyFullerNoConstant(const Scalar x,
                                        const Bool tail = false);

  static Scalar qDickeyFullerTrend(const Scalar p,
                                   const Bool tail = false);

  static Scalar qDickeyFullerConstant(const Scalar p,
                                      const Bool tail = false);

  static Scalar qDickeyFullerNoConstant(const Scalar p,
                                        const Bool tail = false);

}; /* class DistFunc */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTFUNC_HXX */
