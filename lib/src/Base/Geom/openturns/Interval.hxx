//                                               -*- C++ -*-
/**
 *  @brief Interval is defined as the cartesian product of n 1D intervalls ]low_1, up_1]x...x]low_n,up_n]
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
#ifndef OPENTURNS_INTERVAL_HXX
#define OPENTURNS_INTERVAL_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"
#include "openturns/DomainImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Interval
 *
 * A class that holds a collection of interval
 */
class OT_API Interval
  : public DomainImplementation
{
  CLASSNAME

public:
  typedef Collection<UnsignedInteger>           BoolCollection;
  typedef PersistentCollection<UnsignedInteger> BoolPersistentCollection;


  /** Default constructor */
  explicit Interval(const UnsignedInteger dimension = 0);

  /** Parameters constructor, simplified for 1D case */
  Interval(const Scalar lowerBound,
           const Scalar upperBound);

  /** Parameters constructor */
  Interval(const Point & lowerBound,
           const Point & upperBound);

  /** Parameters constructor */
  Interval(const Point & lowerBound,
           const Point & upperBound,
           const BoolCollection & finiteLowerBound,
           const BoolCollection & finiteUpperBound);

  /** Virtual constructor method */
  Interval * clone() const override;

  /** Check if the interval is empty, i.e. if we have lowerBound >= upperBound for at least one component */
  Bool isEmpty() const;

  /** Check if the interval is numerically empty, i.e. its volume is zero */
  Bool isNumericallyEmpty() const;

  /** Check if the given point is inside of the closed interval */
  Bool contains(const Point & point) const override;
  using DomainImplementation::contains;

  /** Check if the given point is numerically inside of the closed interval, i.e. using only the bounds part of the interval */
  Bool numericallyContains(const Point & point) const;

  /** Returns the interval equals to the intersection between the interval and another one */
  Interval intersect(const Interval & other) const;

  /** Returns the interval equals to the union between the interval and another one */
  Interval join(const Interval & other) const;

  /** Marginal accessor */
  Interval getMarginal(const Indices & indices) const;
  Interval getMarginal(const UnsignedInteger index) const;

  /** Addition operator */
  Interval operator +(const Interval & rhs) const;

  /** Subtraction operator */
  Interval operator -(const Interval & rhs) const;

  /** In-place addition operator */
  Interval & operator +=(const Interval & other);

  /** In-place subtraction operator */
  Interval & operator -=(const Interval & other);

  /** Product operator */
  Interval operator *(const Scalar scalar) const;

  /**  In-place product operator */
  Interval & operator *=(const Scalar scalar);

  /** Comparison operator */
  Bool operator == (const Interval & other) const;
  Bool operator != (const Interval & other) const;
  using DomainImplementation::operator ==;
  using DomainImplementation::operator !=;

  /** Volume accessor */
  Scalar getVolume() const;

  /** Lower bound accessor */
  Point getLowerBound() const;
  void setLowerBound(const Point & lowerBound);

  /** Upper bound accessor */
  Point getUpperBound() const;
  void setUpperBound(const Point & upperBound);

  /** Lower bound flag accessor */
  BoolCollection getFiniteLowerBound() const;
  void setFiniteLowerBound(const BoolCollection & finiteLowerBound);

  /** Upper bound flag accessor */
  BoolCollection getFiniteUpperBound() const;
  void setFiniteUpperBound(const BoolCollection & finiteUpperBound);

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  // An n-D intervall is defined as the cartesian product of n 1D intervalls ]low_1, up_1]x...x]low_n,up_n]
  Point lowerBound_;
  Point upperBound_;

  // Flags that tell if the bounds are finite or not.
  // The bound is finite if the flag is true, else it is equal to -inf if the associated value is < 0, +inf else
  BoolPersistentCollection finiteLowerBound_;
  BoolPersistentCollection finiteUpperBound_;

}; /* class Interval */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_INTERVAL_HXX */
