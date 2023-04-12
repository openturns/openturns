//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for non elliptical distributions
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
#ifndef OPENTURNS_ARCHIMEDEANCOPULA_HXX
#define OPENTURNS_ARCHIMEDEANCOPULA_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class ArchimedeanCopula
 *
 * All traditional distribution inherits from this class.
 * Classes derived from ArchimedeanCopula are known by themselves,
 * without being reconstructed or built in any way.
 */
class OT_API ArchimedeanCopula
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ArchimedeanCopula();

  /** Virtual constructor */
  ArchimedeanCopula * clone() const override;

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const ArchimedeanCopula & other) const;

  /** String converter */
  String __repr__() const override;

  /** Get the PDF of the archimedean copula */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the archimedean copula */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;
  using DistributionImplementation::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  Scalar computeConditionalPDF(const Scalar x,
                               const Point & y) const override;

  /** Compute the archimedean generator of the archimedean copula, i.e.
   *  the function phi such that the CDF of the copula can
   *  be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
   */
  virtual Scalar computeArchimedeanGenerator(const Scalar t) const;

  /** Compute the inverse of the archimedean generator */
  virtual Scalar computeInverseArchimedeanGenerator(const Scalar t) const;

  /** Compute the derivative of the archimedean generator */
  virtual Scalar computeArchimedeanGeneratorDerivative(const Scalar t) const;

  /** Compute the second derivative of the archimedean generator */
  virtual Scalar computeArchimedeanGeneratorSecondDerivative(const Scalar t) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using DistributionImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const override;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

protected:


private:


}; /* class ArchimedeanCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ARCHIMEDEANCOPULA_HXX */
