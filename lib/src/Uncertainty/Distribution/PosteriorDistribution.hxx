//                                               -*- C++ -*-
/**
 *  @brief The PosteriorDistribution distribution
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
#ifndef OPENTURNS_POSTERIORDISTRIBUTION_HXX
#define OPENTURNS_POSTERIORDISTRIBUTION_HXX

#include "OTprivate.hxx"
#include "ContinuousDistribution.hxx"
#include "ConditionalDistribution.hxx"
#include "ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS




/**
 * @class PosteriorDistribution
 *
 * The PosteriorDistribution distribution.
 */
class OT_API PosteriorDistribution
  : public ContinuousDistribution
{
  CLASSNAME;
public:

  /** Default constructor */
  PosteriorDistribution();

  /** Parameters constructor */
  PosteriorDistribution(const ConditionalDistribution & conditionalDistribution,
                        const NumericalSample & observations);

  /** Comparison operator */
  Bool operator ==(const PosteriorDistribution & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual PosteriorDistribution * clone() const;

  /** Get the PDF of the distribution */
  using ContinuousDistribution::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the distribution */
  using ContinuousDistribution::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using ContinuousDistribution::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);


  /* Interface specific to PosteriorDistribution */

  /** Conditional distribution accessor */
  void setConditionalDistribution(const ConditionalDistribution & conditionalDistribution);
  ConditionalDistribution getConditionalDistribution() const;

  /** Observations accessor */
  void setObservations(const NumericalSample & observations);
  NumericalSample getObservations() const;

  /** Log normalization factor accessor */
  NumericalScalar getLogNormalizationFactor() const;

  /** Get the standard deviation of the distribution */
  NumericalPoint getStandardDeviation() const;

  /** Get the skewness of the distribution */
  NumericalPoint getSkewness() const;

  /** Get the kurtosis of the distribution */
  NumericalPoint getKurtosis() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


  /** Compute the likelihood of the observations */
  NumericalPoint computeLikelihood(const NumericalPoint & theta) const;

protected:


private:

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Compute the mean of the distribution */
  void computeMean() const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** The underlying conditional distribution */
  ConditionalDistribution conditionalDistribution_;

  /** The observations */
  NumericalSample observations_;

  /** The Bayes normalization constant */
  NumericalScalar logNormalizationFactor_;

}; /* class PosteriorDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_POSTERIORDISTRIBUTION_HXX */
