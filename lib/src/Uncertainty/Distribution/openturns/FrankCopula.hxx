//                                               -*- C++ -*-
/**
 *  @brief The FrankCopula distribution
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
#ifndef OPENTURNS_FRANKCOPULA_HXX
#define OPENTURNS_FRANKCOPULA_HXX

#include "openturns/ArchimedeanCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FrankCopula
 *
 * The FrankCopula distribution.
 */
class OT_API FrankCopula
  : public ArchimedeanCopula
{
  CLASSNAME
public:

  /** Default constructor */
  FrankCopula();

  /** Parameters constructor */
  explicit FrankCopula(const Scalar theta);

  /** Comparison operator */
  Bool operator ==(const FrankCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const;
public:

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;


  /* Interface inherited from Distribution */
  /** Virtual constructor */
  virtual FrankCopula * clone() const;

  /** Get one realization of the distribution */
  Point getRealization() const;

  /** Get the DDF of the distribution */
  using ArchimedeanCopula::computeDDF;
  Point computeDDF(const Point & point) const;

  /** Get the PDF of the distribution */
  using ArchimedeanCopula::computePDF;
  Scalar computePDF(const Point & point) const;

  /** Get the CDF of the distribution */
  using ArchimedeanCopula::computeCDF;
  Scalar computeCDF(const Point & point) const;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const;

  /** Get the PDFGradient of the distribution */
  Point computePDFGradient(const Point & point) const;

  /** Get the CDFGradient of the distribution */
  Point computeCDFGradient(const Point & point) const;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const Scalar prob,
                        const Bool tail = false) const;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using CopulaImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const;

  /** Compute the archimedean generator of the archimedean copula, i.e.
   *  the function phi such that the CDF of the copula can
   *  be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
   */
  Scalar computeArchimedeanGenerator(const Scalar t) const;

  /** Compute the inverse of the archimedean generator */
  Scalar computeInverseArchimedeanGenerator(const Scalar t) const;

  /** Compute the derivative of the archimedean generator */
  Scalar computeArchimedeanGeneratorDerivative(const Scalar t) const;

  /** Compute the second derivative of the archimedean generator */
  Scalar computeArchimedeanGeneratorSecondDerivative(const Scalar t) const;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const;

  /** Compute the entropy of the distribution */
  Scalar computeEntropy() const;

  /** Parameters value accessors */
  void setParameter(const Point & parameter);
  Point getParameter() const;

  /** Parameters description accessor */
  Description getParameterDescription() const;

  /* Interface specific to FrankCopula */

  /** Theta accessor */
  void setTheta(const Scalar theta);
  Scalar getTheta() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:


private:

  /** Compute the covariance of the distribution */
  void computeCovariance() const;

  /** The parameter of the FrankCopula distribution */
  Scalar theta_;

}; /* class FrankCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FRANKCOPULA_HXX */
