//                                               -*- C++ -*-
/**
 *  @brief The Rice distribution
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_RICE_HXX
#define OPENTURNS_RICE_HXX

#include "openturns/ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Rice
 *
 * The Rice distribution.
 */
class OT_API Rice
  : public ContinuousDistribution
{
  CLASSNAME
public:

  /** Default constructor */
  Rice();

  /** Parameters constructor */
  explicit Rice(const Scalar beta,
                const Scalar nu = 0.0);


  /** Comparison operator */
  Bool operator ==(const Rice & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual Rice * clone() const;

  /** Get one realization of the Rice distribution */
  Point getRealization() const;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using ContinuousDistribution::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the logarithm of the PDF of the distribution */
  using ContinuousDistribution::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point). If tail=true, compute P(X >= point) */
  using ContinuousDistribution::computeCDF;
  Scalar computeCDF(const Point & point) const;
  using ContinuousDistribution::computeComplementaryCDF;
  Scalar computeComplementaryCDF(const Point & point) const;

  /** Get the standard deviation of the distribution */
  Point getStandardDeviation() const;

  /** Get the raw moments of the standardized distribution */
  Point getStandardMoment(const UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Distribution getStandardRepresentative() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /* Interface specific to Rice */

  /** Beta accessor */
  void setBeta(const Scalar beta);
  Scalar getBeta() const;

  /** @deprecated Sigma accessor */
  void setSigma(const Scalar sigma);
  Scalar getSigma() const;

  /** Nu accessor */
  void setNu(const Scalar nu);
  Scalar getNu() const;

  /** Maximum iterations accessor */
  void setMaximumIteration(const UnsignedInteger maximumIteration);
  UnsignedInteger getMaximumIteration() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

private:

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The dispersion parameter */
  Scalar beta_;

  /** The position parameter */
  Scalar nu_;

  /** The maximum iterations for the PDF and CDF computations */
  UnsignedInteger maximumIteration_;

}; /* class Rice */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RICE_HXX */
