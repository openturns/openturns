//                                               -*- C++ -*-
/**
 *  @brief Abstract top-level class for Discrete distributions
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_DISCRETEDISTRIBUTION_HXX
#define OPENTURNS_DISCRETEDISTRIBUTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DiscreteDistribution
 *
 * A subclass for Discrete usual distributions.
 */
class OT_API DiscreteDistribution
  : public DistributionImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  DiscreteDistribution();

  /** Virtual constructor */
  DiscreteDistribution * clone() const override;

  /** Comparison operator */
  Bool operator ==(const DiscreteDistribution & other) const;

  /** String converter */
  String __repr__() const override;

  /** Get the PDF of the distribution */
  using DistributionImplementation::computePDF;
  Scalar computePDF(const Point & point) const override;
  virtual Scalar computePDF(const Indices & point) const;

  /* Methods inherited from upper classes */

  /** Tell if the distribution is continuous */
  Bool isContinuous() const override;

  /** Tell if the distribution is discrete */
  Bool isDiscrete() const override;

  /** Tell if the distribution is integer valued */
  Bool isIntegral() const override;

  /** Support tolerance accessor */
  void setSupportEpsilon(const Scalar epsilon);
  Scalar getSupportEpsilon() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Tolerance to say if a point is in the support of the distribution */
  Scalar supportEpsilon_;

private:

}; /* class DiscreteDistribution */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DISCRETEDISTRIBUTION_HXX */
