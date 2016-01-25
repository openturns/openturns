//                                               -*- C++ -*-
/**
 *  @brief LevelSet is defined as the set of points such that f(x_1,...,x_n) <= level
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
#ifndef OPENTURNS_LEVELSET_HXX
#define OPENTURNS_LEVELSET_HXX

#include "PersistentObject.hxx"
#include "Collection.hxx"
#include "PersistentCollection.hxx"
#include "NumericalPoint.hxx"
#include "DomainImplementation.hxx"
#include "NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class LevelSet
 *
 * A class that holds a collection of levelSet
 */
class OT_API LevelSet
  : public DomainImplementation
{
  CLASSNAME;

public:

  /** Default constructor */
  explicit LevelSet(const UnsignedInteger dimension = 1);

  /** Parameters constructor, simplified for 1D case */
  explicit LevelSet(const NumericalMathFunction & function,
                    const NumericalScalar level = 0.0);

  /** Virtual constructor method */
  virtual LevelSet * clone() const;

  /** Check if the given point is inside of the closed levelSet */
  Bool contains(const NumericalPoint & point) const;

  /** Returns the levelSet equals to the intersection between the levelSet and another one */
  LevelSet intersect(const LevelSet & other) const;

  /** Returns the levelSet equals to the union between the levelSet and another one */
  LevelSet join(const LevelSet & other) const;

  /** Comparison operator */
  Bool operator == (const LevelSet & other) const;

  /** Function accessor */
  NumericalMathFunction getFunction() const;
  void setFunction(const NumericalMathFunction & function);

  /** Level accessor */
  NumericalScalar getLevel() const;
  void setLevel(const NumericalScalar level);

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

private:

  /** Function defining the level set*/
  NumericalMathFunction function_;

  /** Level defining the level set */
  NumericalScalar level_;

}; /* class LevelSet */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LEVELSET_HXX */
