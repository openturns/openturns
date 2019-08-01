//                                               -*- C++ -*-
/**
 *  @brief The class that implements assembly distributions
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
#ifndef OPENTURNS_ORDINALSUMCOPULA_HXX
#define OPENTURNS_ORDINALSUMCOPULA_HXX

#include "openturns/CopulaImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/UserDefined.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OrdinalSumCopula
 *
 * The class describes the probabilistic concept of copulas
 * made from a collection of copulas joined by an independent copula
 */
class OT_API OrdinalSumCopula
  : public CopulaImplementation
{
  CLASSNAME
public:

  /** A type for distribution collection */
  typedef Collection<Distribution>                 DistributionCollection;
  typedef PersistentCollection<Distribution>       DistributionPersistentCollection;

  /** Default constructor for save/load methods : 1D distribution with default Uniform marginal and IndependentCopula */
  OrdinalSumCopula();

  /** Default constructor */
  OrdinalSumCopula(const DistributionCollection & coll,
                   const Point & bounds);

  /** Comparison operator */
  Bool operator ==(const OrdinalSumCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Copula collection accessor */
  void setCopulaCollection(const DistributionCollection & coll);
  DistributionCollection getCopulaCollection() const;

  /** Bounds accessor */
  void setBounds(const Point & bounds);
  const Point & getBounds() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual OrdinalSumCopula * clone() const;

  /** Get one realization of the OrdinalSumCopula */
  Point getRealization() const;

  /** Get the DDF of the OrdinalSumCopula */
  using CopulaImplementation::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the OrdinalSumCopula */
  using CopulaImplementation::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the OrdinalSumCopula */
  using CopulaImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the probability content of an interval */
  Scalar computeProbability(const Interval & interval) const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the PDF gradient of the distribution */
  Point computePDFGradient(const Point & point) const;

  /** Get the CDF gradient of the distribution */
  Point computeCDFGradient(const Point & point) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalPDF;
  virtual Scalar computeConditionalPDF(const Scalar x, const Point & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  virtual Scalar computeConditionalCDF(const Scalar x, const Point & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  virtual Scalar computeConditionalQuantile(const Scalar q, const Point & y) const;

  /** Parameters value and description accessor */
  PointWithDescriptionCollection getParametersCollection() const;
  using CopulaImplementation::setParametersCollection;
  void setParametersCollection(const PointCollection & setParametersCollection);

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Get the isoprobabilistic transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilistic transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);


private:
  /** Find the block number of a given real wrt the bounds. Returns -1 if negative and -2 if greater than 1 */
  SignedInteger findBlock(const Scalar x) const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Check if the given point is in the given block */
  Bool isInBlock(const Point & point,
                 const UnsignedInteger index) const;

  /** The collection of distribution of the ComposedCopula */
  DistributionPersistentCollection copulaCollection_;

  /** Bounds of the ordinal sum */
  Point bounds_;

  /** Lenghts of the blocks */
  Point blockLengths_;

  /** Discrete distribution to select the component of the sum for sampling */
  UserDefined blockDistribution_;
}; /* class OrdinalSumCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORDINALSUMCOPULA_HXX */
