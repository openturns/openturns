//                                               -*- C++ -*-
/**
 *  @brief The class that implements assembly distributions
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
#ifndef OPENTURNS_COMPOSEDCOPULA_HXX
#define OPENTURNS_COMPOSEDCOPULA_HXX

#include "openturns/CopulaImplementation.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Copula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComposedCopula
 *
 * The class describes the probabilistic concept of copulas
 * made from a collection of copulas joined by an independent copula
 */
class OT_API ComposedCopula
  : public CopulaImplementation
{
  CLASSNAME;
public:

  /** A type for distribution collection */
  typedef Collection<Copula>                 CopulaCollection;
  typedef PersistentCollection<Copula>       CopulaPersistentCollection;

  /** Default constructor for save/load methods : 1D distribution with default Uniform marginal and IndependentCopula */
  ComposedCopula();

  /** Default constructor */
  explicit ComposedCopula(const CopulaCollection & coll);

  /** Comparison operator */
  Bool operator ==(const ComposedCopula & other) const;
 protected:
  Bool equals(const DistributionImplementation & other) const;
 public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /** Copula collection accessor */
  void setCopulaCollection(const CopulaCollection & coll);
  const CopulaCollection & getCopulaCollection() const;

  /* Here is the interface that all derived class must implement */

  /** Virtual constructor */
  virtual ComposedCopula * clone() const;

  /** Get one realization of the ComposedCopula */
  NumericalPoint getRealization() const;

  /** Get the DDF of the ComposedCopula */
  using CopulaImplementation::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the ComposedCopula */
  using CopulaImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the ComposedCopula */
  using CopulaImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the shape matrix of the distribution, ie the correlation matrix
      of its copula if it is elliptical */
  CorrelationMatrix getShapeMatrix() const;

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

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** The collection of copules of the ComposedCopula */
  CopulaPersistentCollection copulaCollection_;

  /** Flag to tell if the copula is independent */
  Bool isIndependent_;

}; /* class ComposedCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPOSEDCOPULA_HXX */
