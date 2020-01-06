//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for orthogonal basis
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ORTHOGONALBASIS_HXX
#define OPENTURNS_ORTHOGONALBASIS_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/OrthogonalFunctionFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalBasis
 *
 * This is the interface class for orthogonal basis
 */

class OT_API OrthogonalBasis
  : public TypedInterfaceObject<OrthogonalFunctionFactory>
{
  CLASSNAME
public:


  /** Default constructor */
  OrthogonalBasis();

  /** Constructor from implementation */
  OrthogonalBasis(const OrthogonalFunctionFactory & implementation);

  /** Build the Function of the given index */
  Function build(const UnsignedInteger index) const;

  /** Build the Function of the given multi-indices */
  Function build(const Indices & indices) const;

  /** Return the measure upon which the basis is orthogonal */
  Distribution getMeasure() const;

  /** Return the enumerate function that translate unidimensional indices into multidimensional indices */
  EnumerateFunction getEnumerateFunction() const;

  /** String converter */
  virtual String __repr__() const;

protected:

private:

} ; /* class OrthogonalBasis */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALBASIS_HXX */
