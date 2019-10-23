//                                               -*- C++ -*-
/**
 *  @brief The class that implements mixtures
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_MIXTURE_HXX
#define OPENTURNS_MIXTURE_HXX

#include "openturns/Distribution.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Collection.hxx"
#include "openturns/UserDefined.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Mixture
 *
 * The class describes the probabilistic concept of Mixture.
 */
class OT_API Mixture
  : public DistributionImplementation
{
  CLASSNAME

public:

  /** A type for distribution collection */
  typedef Collection<Distribution>           DistributionCollection;
  typedef PersistentCollection<Distribution> DistributionPersistentCollection;

  /** Default constructor */
  Mixture();

  /** Parameters constructor */
  explicit Mixture(const DistributionCollection & coll);
  Mixture(const DistributionCollection & coll,
          const Point & weights);

  /** Comparison operator */
  Bool operator ==(const Mixture & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Distribution collection accessor */
  void setDistributionCollection(const DistributionCollection & coll);
  const DistributionCollection & getDistributionCollection() const;

  /** get/set weights accessor */
  Point getWeights() const;
  void setWeights(const Point & weights);

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual Mixture * clone() const;

  /** Get one realization of the Mixture */
  Point getRealization() const;

  /** Get the DDF of the Mixture */
  using DistributionImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the Mixture */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the Mixture */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Compute the survival function */
  using DistributionImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Get the characteristic function of the distribution, i.e. phi(u) = E(exp(I*u*X)) */
  Complex computeCharacteristicFunction(const Scalar x) const;

  /** Get the PDF gradient of the distribution */
  using DistributionImplementation::computePDFGradient;
  Point computePDFGradient(const Point & point) const;

  /** Get the CDF gradient of the distribution */
  using DistributionImplementation::computeCDFGradient;
  Point computeCDFGradient(const Point & point) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalPDF;
  Scalar computeConditionalPDF(const Scalar x, const Point & y) const;
  Point computeSequentialConditionalPDF(const Point & x) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const;
  Point computeSequentialConditionalCDF(const Point & x) const;

  /** Get the i-th marginal distribution */
  Distribution getMarginal(const UnsignedInteger i) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  Distribution getMarginal(const Indices & indices) const;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const;

  Point getParameter() const;
  Description getParameterDescription() const;
  void setParameter(const Point & parameter);

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
  Sample getSupport(const Interval & interval) const;

  /** Get the PDF singularities inside of the range - 1D only */
  Point getSingularities() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:
  /** Set distribution collection with weights given in a vector */
  void setDistributionCollectionWithWeights(const DistributionCollection & coll,
      const Point & weights);

  /** Compute the mean of a mixture */
  void computeMean() const;

  /** Compute the covariance of a mixture */
  void computeCovariance() const;

  /** Compute the numerical range of the distribution given the parameters values */
  void computeRange();

  /** The collection of distribution of the mixture */
  DistributionPersistentCollection distributionCollection_;

  /** The discrete distribution of the weights */
  /** Structures for the alias sampling method */
  mutable Point base_;
  mutable Indices alias_;
  Bool uniformWeights_;
  Point p_;

  /** PDF approximation associated to the CDF approximation */
  PiecewiseHermiteEvaluation pdfApproximationCDF_;

  /** CDF approximation */
  PiecewiseHermiteEvaluation cdfApproximation_;

  /** PDF approximation associated to the CCDF approximation */
  PiecewiseHermiteEvaluation pdfApproximationCCDF_;

  /** Complementary CDF approximation */
  PiecewiseHermiteEvaluation ccdfApproximation_;

  /** Do I have an approximation for the CDF? */
  Bool useApproximatePDFCDF_;

}; /* class Mixture */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MIXTURE_HXX */
