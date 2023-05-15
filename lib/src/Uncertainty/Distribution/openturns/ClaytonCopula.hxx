//                                               -*- C++ -*-
/**
 *  @brief The ClaytonCopula distribution
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_CLAYTONCOPULA_HXX
#define OPENTURNS_CLAYTONCOPULA_HXX

#include "openturns/ArchimedeanCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ClaytonCopula
 *
 * The ClaytonCopula distribution.
 */
class OT_API ClaytonCopula
  : public ArchimedeanCopula
{
  CLASSNAME
public:

  /** Default constructor */
  ClaytonCopula();

  /** Parameters constructor */
  explicit ClaytonCopula(const Scalar theta);

  /** Comparison operator */
  using ArchimedeanCopula::operator ==;
  Bool operator ==(const ClaytonCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;



  /* Interface inherited from Distribution */
  /** Virtual constructor */
  ClaytonCopula * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the DDF of the distribution */
  using ArchimedeanCopula::computeDDF;
  Point computeDDF(const Point & point) const override;

  /** Get the PDF of the distribution */
  using ArchimedeanCopula::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using ArchimedeanCopula::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the Kendall concordance of the distribution */
  CorrelationMatrix getKendallTau() const override;

  /** Get the PDFGradient of the distribution */
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  Point computeCDFGradient(const Point & point) const override;

  /** Get the quantile of the distribution */
  using ArchimedeanCopula::computeQuantile;
  Point computeQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using ArchimedeanCopula::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using ArchimedeanCopula::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;

  /** Compute the archimedean generator of the archimedean copula, i.e.
   *  the function phi such that the CDF of the copula can
   *  be written as CDF(t) = phi^{-1}(phi(u)+phi(v))
   */
  Scalar computeArchimedeanGenerator(const Scalar t) const override;

  /** Compute the inverse of the archimedean generator */
  Scalar computeInverseArchimedeanGenerator(const Scalar t) const override;

  /** Compute the derivative of the archimedean generator */
  Scalar computeArchimedeanGeneratorDerivative(const Scalar t) const override;

  /** Compute the second derivative of the archimedean generator */
  Scalar computeArchimedeanGeneratorSecondDerivative(const Scalar t) const override;

  /** Dependence coefficients */
  CorrelationMatrix computeUpperTailDependenceMatrix() const override;
  CorrelationMatrix computeLowerTailDependenceMatrix() const override;
  
  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /* Interface specific to ClaytonCopula */

  /** Theta accessor */
  void setTheta(const Scalar theta);
  Scalar getTheta() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** The parameter of the ClaytonCopula distribution */
  Scalar theta_;

}; /* class ClaytonCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CLAYTONCOPULA_HXX */
