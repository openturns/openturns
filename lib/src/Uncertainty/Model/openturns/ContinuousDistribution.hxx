//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Continuous distributions
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_CONTINUOUSDISTRIBUTION_HXX
#define OPENTURNS_CONTINUOUSDISTRIBUTION_HXX

#include "DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ContinuousDistribution
 *
 * A subclass for Continuous usual distributions.
 */
class OT_API ContinuousDistribution
  : public DistributionImplementation
{

  CLASSNAME;
public:

  /** Default constructor */
  ContinuousDistribution();

  /** Virtual constructor */
  virtual ContinuousDistribution * clone() const;

  /** Comparison operator */
  Bool operator ==(const ContinuousDistribution & other) const;

  /** Get the PDF of the distributionImplementation */
  using DistributionImplementation::computeDDF;
  NumericalPoint computeDDF(const NumericalPoint & point) const;

  /** Get the PDF of the distributionImplementation */
  using DistributionImplementation::computePDF;
  NumericalScalar computePDF(const NumericalPoint & point) const;

  /** Get the CDF of the distributionImplementation */
  using DistributionImplementation::computeCDF;
  NumericalScalar computeCDF(const NumericalPoint & point) const;

  /** Get the survival function of the distributionImplementation */
  using DistributionImplementation::computeSurvivalFunction;
  NumericalScalar computeSurvivalFunction(const NumericalPoint & point) const;

protected:
  /** Interpolate the CDF for smooth continuous distributions */
  Collection<PiecewiseHermiteEvaluationImplementation> interpolateCDF(const UnsignedInteger n);

public:
  /** String converter */
  String __repr__() const;


  /* Methods inherited from upper classes */

  /** Tell if the distribution is continuous */
  Bool isContinuous() const;

protected:

}; /* class ContinuousDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONTINUOUSDISTRIBUTION_HXX */
