//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Discrete distributions
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
#ifndef OPENTURNS_DISCRETEDISTRIBUTION_HXX
#define OPENTURNS_DISCRETEDISTRIBUTION_HXX

#include "OTprivate.hxx"
#include "DistributionImplementation.hxx"
#include "ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DiscreteDistribution
 *
 * A subclass for Discrete usual distributions.
 */
class OT_API DiscreteDistribution
  : public DistributionImplementation
{
  CLASSNAME;
public:


  /** Default constructor */
  DiscreteDistribution();


  /** Virtual constructor */
  virtual DiscreteDistribution * clone() const;

  /** Comparison operator */
  Bool operator ==(const DiscreteDistribution & other) const;

  /** String converter */
  String __repr__() const;

  /* Methods inherited from upper classes */

  /** Tell if the distribution is continuous */
  Bool isContinuous() const;

  /** Tell if the distribution is discrete */
  Bool isDiscrete() const;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const;

  /** Support tolerance accessor */
  void setSupportEpsilon(const NumericalScalar epsilon);
  NumericalScalar getSupportEpsilon() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:
  /** Tolerance to say if a point is in the support of the distribution */
  NumericalScalar supportEpsilon_;

private:

}; /* class DiscreteDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISCRETEDISTRIBUTION_HXX */
