//                                               -*- C++ -*-
/**
 *  @brief The class that implements assembly distributions
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
#ifndef OPENTURNS_ORDINALSUMCOPULA_HXX
#define OPENTURNS_ORDINALSUMCOPULA_HXX

#include "openturns/CopulaImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Copula.hxx"
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
  CLASSNAME;
public:

  /** A type for distribution collection */
  typedef Collection<Copula>                 CopulaCollection;
  typedef PersistentCollection<Copula>       CopulaPersistentCollection;

  /** Default constructor for save/load methods : 1D distribution with default Uniform marginal and IndependentCopula */
  OrdinalSumCopula();

  /** Default constructor */
  OrdinalSumCopula(const CopulaCollection & coll,
                   const NumericalPoint & bounds);

  /** Comparison operator */
  Bool operator ==(const OrdinalSumCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Copula collection accessor */
  void setCopulaCollection(const CopulaCollection & coll);
  const CopulaCollection & getCopulaCollection() const;

  /** Bounds accessor */
  void setBounds(const NumericalPoint & bounds);
  const NumericalPoint & getBounds() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual OrdinalSumCopula * clone() const;

  /** Get one realization of the OrdinalSumCopula */
  NumericalPoint getRealization() const;

  /** Get the DDF of the OrdinalSumCopula */
  using CopulaImplementation::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the OrdinalSumCopula */
  using CopulaImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the OrdinalSumCopula */
  using CopulaImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the PDF gradient of the distribution */
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDF gradient of the distribution */
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Implementation getMarginal(const Indices & indices) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalPDF;
  virtual NumericalScalar computeConditionalPDF(const NumericalScalar x, const NumericalPoint & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  virtual NumericalScalar computeConditionalCDF(const NumericalScalar x, const NumericalPoint & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  virtual NumericalScalar computeConditionalQuantile(const NumericalScalar q, const NumericalPoint & y) const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using CopulaImplementation::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & setParametersCollection);

  /** Parameters value accessors */
  void setParameter(const NumericalPoint & parameter);
  NumericalPoint getParameter() const;

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
  SignedInteger findBlock(const NumericalScalar x) const;

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** Check if the given point is in the given block */
  Bool isInBlock(const NumericalPoint & point,
                 const UnsignedInteger index) const;

  /** The collection of distribution of the ComposedCopula */
  CopulaPersistentCollection copulaCollection_;

  /** Bounds of the ordinal sum */
  NumericalPoint bounds_;

  /** Lenghts of the blocks */
  NumericalPoint blockLengths_;

  /** Discrete distribution to select the component of the sum for sampling */
  UserDefined blockDistribution_;
}; /* class ComposedCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORDINALSUMCOPULA_HXX */
