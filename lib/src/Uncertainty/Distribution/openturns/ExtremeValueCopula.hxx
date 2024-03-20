//                                               -*- C++ -*-
/**
 *  @brief The ExtremeValueCopula distribution
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_EXTREMEVALUECOPULA_HXX
#define OPENTURNS_EXTREMEVALUECOPULA_HXX

#include "openturns/ArchimedeanCopula.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ExtremeValueCopula
 *
 * The ExtremeValueCopula distribution.
 */
class OT_API ExtremeValueCopula
  : public DistributionImplementation
{
  CLASSNAME
public:

  /** Default constructor */
  ExtremeValueCopula();

  /** Parameters constructor */
  explicit ExtremeValueCopula(const Function & pickandFunction);

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const ExtremeValueCopula & other) const;
protected:
  Bool equals(const DistributionImplementation & other) const override;
public:

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;


  /* Interface inherited from Distribution */
  /** Virtual constructor */
  ExtremeValueCopula * clone() const override;

  /** Get one realization of the distribution */
  Point getRealization() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the log PDF of the distribution */
  using DistributionImplementation::computeLogPDF;
  Scalar computeLogPDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Compute the CDF of Xi | X1, ..., Xi-1. x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalCDF;
  Scalar computeConditionalCDF(const Scalar x, const Point & y) const override;

  /** Compute the quantile of Xi | X1, ..., Xi-1, i.e. x such that CDF(x|y) = q with x = Xi, y = (X1,...,Xi-1) */
  using DistributionImplementation::computeConditionalQuantile;
  Scalar computeConditionalQuantile(const Scalar q, const Point & y) const override;

  /** Tell if the distribution has independent copula */
  Bool hasIndependentCopula() const override;

  /* Interface specific to ExtremeValueCopula */

  /** Pickand function accessor */
  void setPickandFunction(const Function & pickandFunction,
                          const Bool check = ResourceMap::GetAsBool("ExtremeValueCopula-CheckPickandFunction"));
  Function getPickandFunction() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:


private:

  /** The Pickand function */
  Function pickandFunction_;

}; /* class ExtremeValueCopula */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_EXTREMEVALUECOPULA_HXX */
