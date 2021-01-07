//                                               -*- C++ -*-
/**
 *  @brief The PlackettCopula distribution
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PLACKETTCOPULA_HXX
#define OPENTURNS_PLACKETTCOPULA_HXX

#include "openturns/DistributionImplementation.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class PlackettCopula
 *
 * The PlackettCopula distribution.
 */
class OT_API PlackettCopula
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  PlackettCopula();

  /** Parameters constructor */
  explicit PlackettCopula(const Scalar theta);

  /** Comparison operator */
  Bool operator ==(const PlackettCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Interface inherited from Distribution */
  /** Virtual constructor */
  PlackettCopula * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the PDFGradient of the distribution */
  Point computePDFGradient(const Point & point) const override;

  /** Get the CDFGradient of the distribution */
  Point computeCDFGradient(const Point & point) const override;

  /** Get the quantile of the distribution */
  using DistributionImplementation::computeQuantile;
  Point computeQuantile(const Scalar prob, const Bool tail = false) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;

  /** Get the distribution of the marginal distribution corresponding to indices dimensions */
  using DistributionImplementation::getMarginal;
  Distribution getMarginal(const Indices & indices) const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /** Parameters value accessors */
  void setParameter(const Point & parameter) override;
  Point getParameter() const override;

  /** Parameters description accessor */
  Description getParameterDescription() const override;

  /* Interface specific to PlackettCopula */

  /** Theta accessor */
  void setTheta(const Scalar theta);
  Scalar getTheta() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** Compute the covariance of the distribution */
  void computeCovariance() const override;

  /** The parameter of the PlackettCopula distribution */
  Scalar theta_;
  Scalar thetaMinus1_;

}; /* class PlackettCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PLACKETTCOPULA_HXX */
