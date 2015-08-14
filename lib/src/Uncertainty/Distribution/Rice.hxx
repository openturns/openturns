//                                               -*- C++ -*-
/**
 *  @brief The Rice distribution
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_RICE_HXX
#define OPENTURNS_RICE_HXX

#include "ContinuousDistribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Rice
 *
 * The Rice distribution.
 */
class OT_API Rice
  : public ContinuousDistribution
{
  CLASSNAME;
public:

  typedef Pointer<DistributionImplementation> Implementation;

  /** Default constructor */
  Rice();

  /** Parameters constructor */
  explicit Rice(const NumericalScalar sigma,
                const NumericalScalar nu = 0.0);


  /** Comparison operator */
  Bool operator ==(const Rice & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;



  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual Rice * clone() const;

  /** Get one realization of the Rice distribution */
  NumericalPoint getRealization() const;

  /** Get the PDF of the distribution, i.e. P(point < X < point+dx) = PDF(point)dx + o(dx) */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the logarithm of the PDF of the distribution */
  using ContinuousDistribution::computeLogPDF;
  NumericalScalar computeLogPDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution, i.e. P(X <= point) = CDF(point). If tail=true, compute P(X >= point) */
  using ContinuousDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;
  using ContinuousDistribution::computeComplementaryCDF;
  NumericalScalar computeComplementaryCDF(const NumericalPoint & point) const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the raw moments of the standardized distribution */
  NumericalPoint getStandardMoment(const UnsignedInteger n) const;

  /** Get the standard representative in the parametric family, associated with the standard moments */
  Implementation getStandardRepresentative() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using ContinuousDistribution::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /* Interface specific to Rice */

  /** Sigma accessor */
  void setSigma(const NumericalScalar sigma);
  NumericalScalar getSigma() const;

  /** Nu accessor */
  void setNu(const NumericalScalar nu);
  NumericalScalar getNu() const;

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
  NumericalScalar sigma_;

  /** The position parameter */
  NumericalScalar nu_;

  /** The maximum iterations for the PDF and CDF computations */
  UnsignedInteger maximumIteration_;

}; /* class Rice */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RICE_HXX */
