//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for  function factories
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
#ifndef OPENTURNS_UNIVARIATEFUNCTIONFAMILY_HXX
#define OPENTURNS_UNIVARIATEFUNCTIONFAMILY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/UniVariateFunctionFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class UniVariateFunctionFamily
 *
 * This is the interface class for  function factories
 */

class OT_API UniVariateFunctionFamily
  : public TypedInterfaceObject<UniVariateFunctionFactory>
{
  CLASSNAME
public:

  /** Default constructor */
  UniVariateFunctionFamily();

  /** Constructor from implementation */
  UniVariateFunctionFamily(const UniVariateFunctionFactory & implementation);

  /** String converter */
  virtual String __repr__() const;

  /** The method to get the function of any order. */
  UniVariateFunction build(const UnsignedInteger order) const;

protected:

private:

} ; /* class UniVariateFunctionFamily */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_UNIVARIATEFUNCTIONFAMILY_HXX */
