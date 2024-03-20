//                                               -*- C++ -*-
/**
 *  @brief The NonCentralStudent distribution
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
#ifndef OPENTURNS_NONCENTRALSTUDENT_HXX
#define OPENTURNS_NONCENTRALSTUDENT_HXX

#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NonCentralStudent
 *
 * The NonCentralStudent distribution.
 */
class OT_API NonCentralStudent
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  NonCentralStudent(const Scalar nu = 5.0,
                    const Scalar delta = 0.0,
                    const Scalar gamma = 0.0);

  /** Comparison operator */
  using ContinuousDistribution::operator ==;
  Bool operator ==(const NonCentralStudent & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  NonCentralStudent * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the PDFGradient of the distribution */
  using ContinuousDistribution::computePDFGradient;
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  using ContinuousDistribution::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const override;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const override;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const override;

  /** Get the skewness of the distribution */
  Point getSkewness() const override;

  /** Get the kurtosis of the distribution */
  Point getKurtosis() const override;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Nu accessor */
  void setNu(const Scalar nu);
  Scalar getNu() const;

  /** Delta accessor */
  void setDelta(const Scalar delta);
  Scalar getDelta() const;

  /** Gamma accessor */
  void setGamma(const Scalar gamma);
  Scalar getGamma() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


protected:


private:

  /** Compute the mean of the distribution */
  void computeMean() const override;

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** Number of degrees of freedom */
  Scalar nu_;

  /** Non-centrality parameter */
  Scalar delta_;

  /** Location parameter */
  Scalar gamma_;

}; /* class NonCentralStudent */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NONCENTRALSTUDENT_HXX */
