//                                               -*- C++ -*-
/**
 * @brief Intersection of several events
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

#ifndef OPENTURNS_INTERSECTIONEVENT_HXX
#define OPENTURNS_INTERSECTIONEVENT_HXX

#include "openturns/RandomVectorImplementation.hxx"
#include "openturns/RandomVector.hxx"

BEGIN_NAMESPACE_OPENTURNS


/**
 * @class IntersectionEvent
 *
 *
 */
class OT_API IntersectionEvent
  : public RandomVectorImplementation
{
  CLASSNAME

public:
  typedef Collection<RandomVector> RandomVectorCollection;
  typedef PersistentCollection<RandomVector> RandomVectorPersistentCollection;

  /** Default constructor */
  IntersectionEvent();

  /** Constructor from collection */
  explicit IntersectionEvent(const RandomVectorCollection & collection);

  /** Virtual constructor */
  IntersectionEvent * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Dimension accessor */
  UnsignedInteger getDimension() const override;

  /** Collection accessor */
  void setEventCollection(const RandomVectorCollection & collection);
  RandomVectorCollection getEventCollection() const;

  /** Realization accessor */
  Point getRealization() const override;

  /** Fixed value accessor */
  Point getFrozenRealization(const Point & fixedValue) const override;

  /** Whether it is an event */
  Bool isEvent() const override;

  /** Whether it is composite */
  Bool isComposite() const override;

  /** Antecedent accessor */
  RandomVector getAntecedent() const override;

  // /** Function accessor */
  // Function getFunction() const override;

  // /** Domain accessor */
  // Domain getDomain() const override;

  // /** Operator accessor */
  // ComparisonOperator getOperator() const override;

  // /** Threshold accessor */
  // Scalar getThreshold() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** The process of the eventProcess */
  RandomVectorPersistentCollection eventCollection_;

  /** The antecedent is defined here as the root cause */
  RandomVector antecedent_;

}; /* class IntersectionEvent */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTERSECTIONEVENT_HXX */
