//                                               -*- C++ -*-
/**
 *  @brief OpenTURNS wrapper to a library of special functions
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_DISTFUNC_HXX
#define OPENTURNS_DISTFUNC_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Sample.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API DistFunc
{
public:
  static const UnsignedInteger NumberOfBandNormalZigurrat;
  static const NumericalScalar NormalZigguratTail;
  // The array NormalZigguratAbscissa stores the abscissas of the bands:
  // the ith band is [NormalZigguratAbscissa[0], NormalZigguratAbscissa[i+1]] with
  // NormalZigguratAbscissa[0] = 0.0 and
  // NormalZigguratAbscissa[NumberOfBandNormalZigurrat] = NormalZigguratTail
  static const NumericalScalar NormalZigguratAbscissa[];
  // The array NormalZigguratRatio stores the ratio between a band and the next one,
  // starting from the smallest one:
  // NormalZigguratRatio[i] = NormalZigguratAbscissa[i] / NormalZigguratAbscissa[i+1]
  static const NumericalScalar NormalZigguratRatio[];

  // For Beta distribution. WARNING: here we adopt the classical definition
  // If tail=false, compute P(X<=x), else P(X>x)
  static NumericalScalar pBeta(const NumericalScalar p1,
                               const NumericalScalar p2,
                               const NumericalScalar x,
                               const Bool tail = false);
  // If tail=false, compute x such that P(X<=x)=q, else x such that P(X>x)=q
  static NumericalScalar qBeta(const NumericalScalar p1,
                               const NumericalScalar p2,
                               const NumericalScalar p,
                               const Bool tail = false);
  static NumericalScalar rBeta(const NumericalScalar p1,
                               const NumericalScalar p2);
  static NumericalPoint rBeta(const NumericalScalar p1,
                              const NumericalScalar p2,
                              const UnsignedInteger size);
  // For the Binomial distribution
private:
  static NumericalScalar fcBinomial(const UnsignedInteger k);

public:
  static UnsignedInteger rBinomial(const UnsignedInteger n,
                                   const NumericalScalar p);
  static Indices rBinomial(const UnsignedInteger n,
                           const NumericalScalar p,
                           const UnsignedInteger size);
  // For Gamma distribution
  static NumericalScalar pGamma(const NumericalScalar k,
                                const NumericalScalar x,
                                const Bool tail = false);
  static NumericalScalar qGamma(const NumericalScalar k,
                                const NumericalScalar p,
                                const Bool tail = false);
  static NumericalScalar rGamma(const NumericalScalar k);
  static NumericalPoint rGamma(const NumericalScalar k,
                               const UnsignedInteger size);
  // For Kolmogorov distribution
  static NumericalScalar pKolmogorov(const UnsignedInteger n,
                                     const NumericalScalar x,
                                     const Bool tail = false);

  // For NonCentralChiSquare distribution
  static NumericalScalar dNonCentralChiSquare(const NumericalScalar nu,
      const NumericalScalar lambda,
      const NumericalScalar x,
      const NumericalScalar precision = ResourceMap::GetAsNumericalScalar("DistFunc-Precision"),
      const UnsignedInteger maximumIteration = ResourceMap::GetAsNumericalScalar("DistFunc-MaximumIteration"));
  static NumericalScalar pNonCentralChiSquare(const NumericalScalar nu,
      const NumericalScalar lambda,
      const NumericalScalar x,
      const Bool tail = false,
      const NumericalScalar precision = ResourceMap::GetAsNumericalScalar("DistFunc-Precision"),
      const UnsignedInteger maximumIteration = ResourceMap::GetAsNumericalScalar("DistFunc-MaximumIteration"));
  static NumericalScalar rNonCentralChiSquare(const NumericalScalar nu,
      const NumericalScalar lambda);
  static NumericalPoint rNonCentralChiSquare(const NumericalScalar nu,
      const NumericalScalar lambda,
      const UnsignedInteger size);
  // For NonCentralStudent distribution
  static NumericalScalar dNonCentralStudent(const NumericalScalar nu,
      const NumericalScalar delta,
      const NumericalScalar x);
  static NumericalScalar dNonCentralStudentAlt0(const NumericalScalar nu,
      const NumericalScalar delta,
      const NumericalScalar x,
      const NumericalScalar precision = ResourceMap::GetAsNumericalScalar("DistFunc-Precision"),
      const UnsignedInteger maximumIteration = ResourceMap::GetAsNumericalScalar("DistFunc-MaximumIteration"));
  static NumericalScalar pNonCentralStudent(const NumericalScalar nu,
      const NumericalScalar delta,
      const NumericalScalar x,
      const Bool tail = false);
  static NumericalScalar rNonCentralStudent(const NumericalScalar nu,
      const NumericalScalar delta);
  static NumericalPoint rNonCentralStudent(const NumericalScalar nu,
      const NumericalScalar delta,
      const UnsignedInteger size);
  // For Normal distribution
  static NumericalScalar pNormal(const NumericalScalar x,
                                 const Bool tail = false);
  static NumericalScalar pNormal2D(const NumericalScalar x1,
                                   const NumericalScalar x2,
                                   const NumericalScalar rho,
                                   const Bool tail = false);
  static NumericalScalar pNormal3D(const NumericalScalar x1,
                                   const NumericalScalar x2,
                                   const NumericalScalar x3,
                                   const NumericalScalar rho12,
                                   const NumericalScalar rho13,
                                   const NumericalScalar rho23,
                                   const Bool tail = false);
  static NumericalScalar qNormal(const NumericalScalar p,
                                 const Bool tail = false);
  static NumericalScalar rNormal();
  static NumericalPoint rNormal(const UnsignedInteger size);
  // For Poisson distribution
  static NumericalScalar qPoisson(const NumericalScalar lambda,
                                  const NumericalScalar p,
                                  const Bool tail = false);
  static UnsignedInteger rPoisson(const NumericalScalar lambda);
  static Indices rPoisson(const NumericalScalar lambda,
                          const UnsignedInteger size);
  // For Student distribution
  static NumericalScalar pStudent(const NumericalScalar nu,
                                  const NumericalScalar x,
                                  const Bool tail = false);
  static NumericalScalar qStudent(const NumericalScalar nu,
                                  const NumericalScalar p,
                                  const Bool tail = false);
  static NumericalScalar rStudent(const NumericalScalar nu);
  static NumericalPoint rStudent(const NumericalScalar nu,
                                 const UnsignedInteger size);
  static NumericalPoint rUniformTriangle(const NumericalPoint & a,
                                         const NumericalPoint & b,
                                         const NumericalPoint & c);
  static Sample rUniformTriangle(const NumericalPoint & a,
                                          const NumericalPoint & b,
                                          const NumericalPoint & c,
                                          const UnsignedInteger size);

  // For TruncatedNormal distribution
  //      static NumericalScalar rTruncatedNormal(const NumericalScalar a, const NumericalScalar b);
  // Compute the expectation of the min of n independent standard normal random variables
  static NumericalScalar eZ1(const UnsignedInteger n);

  // K factor for exact two-sided tolerance intervals of normal pooled populations
  static NumericalScalar kFactorPooled(const UnsignedInteger n,
                                       const UnsignedInteger m,
                                       const NumericalScalar p,
                                       const NumericalScalar alpha);

  // K factor for exact two-sided tolerance intervals of a normal population
  static NumericalScalar kFactor(const UnsignedInteger n,
                                 const NumericalScalar p,
                                 const NumericalScalar alpha);

  // Asymptotic distribution of the DickeyFuller distribution
  static NumericalScalar pDickeyFullerTrend(const NumericalScalar x,
      const Bool tail = false);

  static NumericalScalar pDickeyFullerConstant(const NumericalScalar x,
      const Bool tail = false);

  static NumericalScalar pDickeyFullerNoConstant(const NumericalScalar x,
      const Bool tail = false);

  static NumericalScalar qDickeyFullerTrend(const NumericalScalar p,
      const Bool tail = false);

  static NumericalScalar qDickeyFullerConstant(const NumericalScalar p,
      const Bool tail = false);

  static NumericalScalar qDickeyFullerNoConstant(const NumericalScalar p,
      const Bool tail = false);

}; /* class DistFunc */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISTFUNC_HXX */
