//                                               -*- C++ -*-
/**
 *  @brief Interval is defined as the cartesian product of n 1D intervalls ]low_1, up_1]x...x]low_n,up_n]
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
  CLASSNAME;

public:
  typedef Collection<UnsignedInteger>           BoolCollection;
  typedef PersistentCollection<UnsignedInteger> BoolPersistentCollection;


  /** Default constructor */
  explicit Interval(const UnsignedInteger dimension = 0);

  /** Parameters constructor, simplified for 1D case */
  Interval(const NumericalScalar lowerBound,
           const NumericalScalar upperBound);

  /** Parameters constructor */
  Interval(const Point & lowerBound,
           const Point & upperBound);

  /** Parameters constructor */
  Interval(const Point & lowerBound,
           const Point & upperBound,
           const BoolCollection & finiteLowerBound,
           const BoolCollection & finiteUpperBound);

  /** Virtual constructor method */
  virtual Interval * clone() const;

  /** Check if the interval is empty, i.e. if we have lowerBound >= upperBound for at least one component */
  Bool isEmpty() const;

  /** Check if the given point is inside of the closed interval */
  Bool contains(const Point & point) const;

  /** Check if the given point is numerically inside of the closed interval, i.e. using only the bounds part of the interval */
  Bool numericallyContains(const Point & point) const;

  /** Returns the interval equals to the intersection between the interval and another one */
  Interval intersect(const Interval & other) const;

  /** Returns the interval equals to the union between the interval and another one */
  Interval join(const Interval & other) const;

  /** Addition operator */
  Interval operator +(const Interval & rhs) const;

  /** Substraction operator */
  Interval operator -(const Interval & rhs) const;

  /** In-place addition operator */
  Interval & operator +=(const Interval & other);

  /** In-place substraction operator */
  Interval & operator -=(const Interval & other);

  /** Product operator */
  Interval operator *(const NumericalScalar scalar) const;

  /**  In-place product operator */
  Interval & operator *=(const NumericalScalar scalar);

  /** Comparison operator */
  Bool operator == (const Interval & other) const;
  Bool operator != (const Interval & other) const;
  using DomainImplementation::operator ==;
  using DomainImplementation::operator !=;

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
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Get the numerical volume of the interval */
  void computeVolume() const;

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
