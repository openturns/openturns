//                                               -*- C++ -*-
/**
 *  @brief A class that implements a normal copula
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
#ifndef OPENTURNS_NORMALCOPULA_HXX
#define OPENTURNS_NORMALCOPULA_HXX

#include "openturns/CopulaImplementation.hxx"
#include "openturns/CorrelationMatrix.hxx"
#include "openturns/Normal.hxx"
#include "openturns/DistFunc.hxx"
#include "openturns/TBB.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class NormalCopula
 *
 * The class implements a normal copula
 */
class OT_API NormalCopula
  : public CopulaImplementation
{
  CLASSNAME;

public:

  /** Default constructor */
  explicit NormalCopula(const UnsignedInteger dim = 2);

  /** Default constructor */
  explicit NormalCopula(const CorrelationMatrix & correlation);


  /** Comparison operator */
  Bool operator ==(const NormalCopula & other) const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /* Interface inherited from Distribution */

  /** Virtual constructor */
  virtual NormalCopula * clone() const;

  /** Get one realization of the NormalCopula distribution */
  NumericalPoint getRealization() const;
protected:
  NumericalSample getSampleParallel(const UnsignedInteger size) const;
public:
  NumericalSample getSample(const UnsignedInteger size) const;

  /** Get the DDF of the NormalCopula distribution */
  using CopulaImplementation::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the NormalCopula distribution */
  using CopulaImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the NormalCopula distribution */
  using CopulaImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;
  using CopulaImplementation::computeSurvivalFunction;
  NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

  /** Get the probability content of an interval */
  NumericalScalar computeProbability(const Interval & interval) const;

  /** Get the shape matrix of the copula */
  CorrelationMatrix getShapeMatrix() const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the PDF gradient of the distribution */
  NumericalPoint computePDFGradient(const NumericalPoint & point) const;

  /** Get the CDF gradient of the distribution */
  NumericalPoint computeCDFGradient(const NumericalPoint & point) const;

  /** Compute the PDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalPDF;
  NumericalScalar computeConditionalPDF(const NumericalScalar x,
                                        const NumericalPoint & y) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  NumericalScalar computeConditionalCDF(const NumericalScalar x,
                                        const NumericalPoint & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  NumericalScalar computeConditionalQuantile(const NumericalScalar q,
      const NumericalPoint & y) const;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using CopulaImplementation::getMarginal;
  Implementation getMarginal(const Indices & indices) const;

  /** Get the isoprobabilist transformation */
  IsoProbabilisticTransformation getIsoProbabilisticTransformation() const;

  /** Get the inverse isoprobabilist transformation */
  InverseIsoProbabilisticTransformation getInverseIsoProbabilisticTransformation() const;

  /** Tell if the distribution has elliptical copula */
  Bool hasEllipticalCopula() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Parameters value and description accessor */
  NumericalPointWithDescriptionCollection getParametersCollection() const;
  using CopulaImplementation::setParametersCollection;
  void setParametersCollection(const NumericalPointCollection & parametersCollection);

  /** Parameters value accessors */
  void setParameter(const NumericalPoint & parameter);
  NumericalPoint getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Compute the correlation matrix of a Normal Copula from its Spearman correlation matrix */
  static CorrelationMatrix GetCorrelationFromSpearmanCorrelation(const CorrelationMatrix & matrix);

  /** Compute the correlation matrix of a Normal Copula from its Kendall correlation matrix */
  static CorrelationMatrix GetCorrelationFromKendallCorrelation(const CorrelationMatrix & matrix);

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:


private:
  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  // Normal copula parameter
  CorrelationMatrix correlation_;

  // Underlying generic representative
  Normal normal_;

}; /* class NormalCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_NORMALCOPULA_HXX */
