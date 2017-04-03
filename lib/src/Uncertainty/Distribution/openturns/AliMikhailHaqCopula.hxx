//                                               -*- C++ -*-
/**
 *  @brief The AliMikhailHaqCopula distribution
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
#ifndef OPENTURNS_ALIMIKHAILHAQCOPULA_HXX
#define OPENTURNS_ALIMIKHAILHAQCOPULA_HXX

#include "openturns/ArchimedeanCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class AliMikhailHaqCopula
 *
 * The AliMikhailHaqCopula distribution.
 */
class OT_API AliMikhailHaqCopula
  : public ArchimedeanCopula
{
  CLASSNAME;
public:

  /** Default constructor */
  AliMikhailHaqCopula();

  /** Parameters constructor */
  explicit AliMikhailHaqCopula(const NumericalScalar theta);

  /** Comparison operator */
  Bool operator ==(const AliMikhailHaqCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /* Interface inherited from Distribution */
  /** Virtual constructor */
  virtual AliMikhailHaqCopula * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get the DDF of the distribution */
  using ArchimedeanCopula::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the distribution */
  using ArchimedeanCopula::computePDF;
  NumericalScalar computePDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using ArchimedeanCopula::computeCDF;
  NumericalScalar computeCDF(const Point & point) const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the PDFGradient of the distribution */
  Point computePDFGradient(const Point & point) const;

  /** Get the CDFGradient of the distribution */
  Point computeCDFGradient(const Point & point) const;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const NumericalScalar prob,
                                 const Bool tail = false) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  NumericalScalar computeConditionalCDF(const NumericalScalar x, const Point & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  NumericalScalar computeConditionalQuantile(const NumericalScalar q, const Point & y) const;

  /** Compute the archimedean generator of the archimedean copula, i.e.
   *  the function phi such that the CDF of the copula can
   *  be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
   */
  NumericalScalar computeArchimedeanGenerator(const NumericalScalar t) const;

  /** Compute the inverse of the archimedean generator */
  NumericalScalar computeInverseArchimedeanGenerator(const NumericalScalar t) const;

  /** Compute the derivative of the archimedean generator */
  NumericalScalar computeArchimedeanGeneratorDerivative(const NumericalScalar t) const;

  /** Compute the second derivative of the archimedean generator */
  NumericalScalar computeArchimedeanGeneratorSecondDerivative(const NumericalScalar t) const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /* Interface specific to AliMikhailHaqCopula */

  /** Theta accessor */
  void setTheta(const NumericalScalar theta);
  NumericalScalar getTheta() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** The parameter of the AliMikhailHaqCopula distribution */
  NumericalScalar theta_;

}; /* class AliMikhailHaqCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ALIMIKHAILHAQCOPULA_HXX */
