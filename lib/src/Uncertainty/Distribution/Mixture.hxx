//                                               -*- C++ -*-
/**
 *  @brief The class that implements mixtures
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
#ifndef OPENTURNS_MIXTURE_HXX
#define OPENTURNS_MIXTURE_HXX

#include "Distribution.hxx"
#include "DistributionImplementation.hxx"
#include "Collection.hxx"
#include "UserDefined.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Mixture
 *
 * The class describes the probabilistic concept of Mixture.
 */
class OT_API Mixture
  : public DistributionImplementation
{
  CLASSNAME;
public:

  /** A type for distribution collection */
  typedef Collection<Distribution>           DistributionCollection;
  typedef PersistentCollection<Distribution> DistributionPersistentCollection;

  /** Default constructor */
  Mixture();

  /** Parameters constructor */
  explicit Mixture(const DistributionCollection & coll);
  Mixture(const DistributionCollection & coll,
          const NumericalPoint & weights);

  /** Comparison operator */
  Bool operator ==(const Mixture & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll);
  const DistributionCollection & getDistributionCollection() const;

  /** get/set weights accessor */
  NumericalPoint getWeights() const;
  void setWeights(const NumericalPoint & weights);

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual Mixture * clone() const;

  /** Get one realization of the Mixture */
  NumericalPoint getRealization() const;

  /** Get the DDF of the Mixture */
  using DistributionImplementation::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the Mixture */
  using DistributionImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the Mixture */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Compute the survival function */
  using DistributionImplementation::computeSurvivalFunction;
  NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  NumericalComplex computeCharacteristicFunction(const NumericalScalar x) const;

  /** Get the PDF gradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDF gradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Get the i-th marginal distribution */
  Implementation getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Implementation getMarginal(const Indices & indices) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;

  /** Check if the distribution is a copula */
  Bool isCopula() const;

  /** Check if the distribution is elliptical */
  Bool isElliptical() const;

  /** Check if the distribution is continuous */
  Bool isContinuous() const;

  /** Check if the distribution is discrete */
  Bool isDiscrete() const;

  /** Check if the distribution is integral */
  Bool isIntegral() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Get the support of a discrete distribution that intersect a given interval */
  using DistributionImplementation::getSupport;
  NumericalSample getSupport(const Interval & interval) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:
  /** Set distribution collection with weights given in a vector */
  void setDistributionCollectionWithWeights(const DistributionCollection & coll,
      const NumericalPoint & weights);

  /** Compute the mean of a mixture */
  void computeMean() const;

  /** Compute the covariance of a mixture */
  void computeCovariance() const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** Weights distribution accessor */
  void setWeightsDistribution(const UserDefined & weighstDistribution);
  UserDefined getWeightsDistribution() const;

  /** The collection of distribution of the mixture */
  DistributionPersistentCollection distributionCollection_;

  /** The discrete distribution of the weights */
  UserDefined weightsDistribution_;

  /** PDF approximation associated to the CDF approximation */
  PiecewiseHermiteEvaluationImplementation pdfApproximationCDF_;

  /** CDF approximation */
  PiecewiseHermiteEvaluationImplementation cdfApproximation_;

  /** PDF approximation associated to the CCDF approximation */
  PiecewiseHermiteEvaluationImplementation pdfApproximationCCDF_;

  /** Complementary CDF approximation */
  PiecewiseHermiteEvaluationImplementation ccdfApproximation_;

  /** Do I have an approximation for the CDF? */
  Bool useApproximatePDFCDF_;

}; /* class Mixture */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MIXTURE_HXX */
