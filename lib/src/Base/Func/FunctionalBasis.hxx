//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for basis
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
#ifndef OPENTURNS_FUNCTIONALBASIS_HXX
#define OPENTURNS_FUNCTIONALBASIS_HXX

#include "TypedInterfaceObject.hxx"
#include "FunctionalBasisImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FunctionalBasis
 *
 * This is the interface class for orthogonal basis
 */

class OT_API FunctionalBasis
  : public TypedInterfaceObject<FunctionalBasisImplementation>
{
  CLASSNAME;
public:
  typedef Collection<NumericalMathFunction> NumericalMathFunctionCollection;

  /** Default constructor */
  FunctionalBasis();

  /** Constructor from implementation */
  FunctionalBasis(const FunctionalBasisImplementation & implementation);

  /** Build the NumericalMathFunction of the given index */
  NumericalMathFunction build(const UnsignedInteger index) const;

  NumericalMathFunction operator[](const UnsignedInteger index) const;

  /** Tells whether the basis is orthogonal */
  Bool isOrthogonal() const;

  /** String converter */
  virtual String __repr__() const;

protected:

private:

} ; /* class FunctionalBasis */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONALBASIS_HXX */
