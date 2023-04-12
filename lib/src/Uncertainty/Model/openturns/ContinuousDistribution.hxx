//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Continuous distributions
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
#ifndef OPENTURNS_CONTINUOUSDISTRIBUTION_HXX
#define OPENTURNS_CONTINUOUSDISTRIBUTION_HXX

#include "openturns/DistributionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ContinuousDistribution
 *
 * A subclass for Continuous usual distributions.
 */
class OT_API ContinuousDistribution
  : public DistributionImplementation
{

  CLASSNAME
public:

  /** Default constructor */
  ContinuousDistribution();

  /** Virtual constructor */
  ContinuousDistribution * clone() const override;

  /** Comparison operator */
  using DistributionImplementation::operator ==;
  Bool operator ==(const ContinuousDistribution & other) const;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;

  /** Get the CDF of the distribution */
  using DistributionImplementation::computeCDF;
  Scalar computeCDF(const Point & point) const override;

  /** Get the survival function of the distribution */
  using DistributionImplementation::computeSurvivalFunction;
  Scalar computeSurvivalFunction(const Point & point) const override;

protected:
  /** Interpolate the CDF for smooth continuous distributions */
  Collection<PiecewiseHermiteEvaluation> interpolateCDF(const UnsignedInteger n);

public:
  /** String converter */
  String __repr__() const override;


  /* Methods inherited from upper classes */

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

protected:

}; /* class ContinuousDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_CONTINUOUSDISTRIBUTION_HXX */
