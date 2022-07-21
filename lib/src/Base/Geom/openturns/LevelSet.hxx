//                                               -*- C++ -*-
/**
 *  @brief LevelSet is defined as the set of points such that f(x_1,...,x_n) <= level
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
#ifndef OPENTURNS_LEVELSET_HXX
#define OPENTURNS_LEVELSET_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/LessOrEqual.hxx"
#include "openturns/DomainImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LevelSet
 *
 * A class that holds a collection of levelSet
 */
class OT_API LevelSet
  : public DomainImplementation
{
  CLASSNAME

public:
  typedef DomainImplementation::BoolCollection BoolCollection;

  /** Default constructor */
  explicit LevelSet(const UnsignedInteger dimension = 1);

  /** Parameters constructor, simplified for 1D case */
  explicit LevelSet(const Function & function,
                    const ComparisonOperator & op = LessOrEqual(),
                    const Scalar level = 0.0);

  /** Virtual constructor method */
  LevelSet * clone() const override;

  /** Check if the given point is inside of the closed levelSet */
  Bool contains(const Point & point) const override;

  /** Check if the given points are inside of the closed levelSet */
  BoolCollection contains(const Sample & sample) const override;
  using DomainImplementation::contains;

  /** Returns the levelSet equals to the intersection between the levelSet and another one */
  LevelSet intersect(const LevelSet & other) const;

  /** Returns the levelSet equals to the union between the levelSet and another one */
  LevelSet join(const LevelSet & other) const;

  /** Comparison operator */
  Bool operator == (const LevelSet & other) const;

  /** Function accessor */
  Function getFunction() const;
  void setFunction(const Function & function);

  /** Operator accessor */
  ComparisonOperator getOperator() const;
  void setOperator(const ComparisonOperator & op);

  /** Level accessor */
  Scalar getLevel() const;
  void setLevel(const Scalar level);

  /** Lower bound of the bounding box */
  void setLowerBound(const Point & bound);
  Point getLowerBound() const;

  /** Upper bound of the bounding box */
  void setUpperBound(const Point & bound);
  Point getUpperBound() const;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Function defining the level set*/
  Function function_;

  /** Comparison operator against level */
  ComparisonOperator operator_;

  /** Level defining the level set */
  Scalar level_;

  /** Lower bound of the bounding box */
  Point lowerBound_;

  /** Upper bound of the bounding box */
  Point upperBound_;
}; /* class LevelSet */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEVELSET_HXX */
