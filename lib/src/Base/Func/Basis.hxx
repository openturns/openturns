//                                               -*- C++ -*-
/**
 *  @brief A collection of numerical math function
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
#ifndef OPENTURNS_BASIS_HXX
#define OPENTURNS_BASIS_HXX

#include "TypedInterfaceObject.hxx"
#include "BasisImplementation.hxx"
#include "NumericalMathFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Basis
 *
 * A collection of numerical math function.
 */
class OT_API Basis
  : public TypedInterfaceObject<BasisImplementation>
{
  CLASSNAME;

public:
  typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;
  typedef PersistentCollection<NumericalMathFunction> NumericalMathFunctionPersistentCollection;

  /** Default constructor */
  Basis();

  /** Constructor from a collection */
  Basis(const NumericalMathFunctionCollection & coll);

  explicit Basis(const UnsignedInteger size);
#ifndef SWIG
  operator NumericalMathFunctionCollection() const;
#endif
  /** Constructor from implementation */
  Basis(const BasisImplementation & implementation);
//   Basis(BasisImplementation * p_implementation);

  /** Build the NumericalMathFunction of the given index */
  NumericalMathFunction build(const UnsignedInteger index) const;

  /** Build the NumericalMathFunction of the given index */
  NumericalMathFunction operator[](const UnsignedInteger index) const;

  NumericalMathFunction  & operator[](const UnsignedInteger index);

  /** Accessor to the sub-basis */
  NumericalMathFunctionCollection getSubBasis(const Indices & indices) const;

  /** Dimension accessor */
  virtual UnsignedInteger getDimension() const;
  virtual UnsignedInteger getSize() const;

  /** add an element */
  void add(const NumericalMathFunction & elt);

  Bool isFunctional() const;
  Bool isOrthogonal() const;

  /** String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

}; /* class Basis */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_BASIS_HXX */
