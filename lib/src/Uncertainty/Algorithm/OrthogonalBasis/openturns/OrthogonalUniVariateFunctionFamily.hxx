//                                               -*- C++ -*-
/**
 *  @brief This is the interface class for orthogonal function factories
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFAMILY_HXX
#define OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFAMILY_HXX

#include "openturns/TypedInterfaceObject.hxx"
#include "openturns/OrthogonalUniVariateFunctionFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalUniVariateFunctionFamily
 *
 * This is the interface class for orthogonal function factories
 */

class OT_API OrthogonalUniVariateFunctionFamily
  : public TypedInterfaceObject<OrthogonalUniVariateFunctionFactory>
{
  CLASSNAME
public:

  /** Default constructor */
  OrthogonalUniVariateFunctionFamily();

  /** Constructor from implementation */
  OrthogonalUniVariateFunctionFamily(const OrthogonalUniVariateFunctionFactory & implementation);

#ifndef SWIG
  /** Constructor from implementation */
  OrthogonalUniVariateFunctionFamily(const Implementation & p_implementation);

  /** Constructor from implementation */
  OrthogonalUniVariateFunctionFamily(OrthogonalUniVariateFunctionFactory * p_implementation);
#endif

  /** String converter */
  String __repr__() const override;

  /** The method to get the function of any order. */
  UniVariateFunction build(const UnsignedInteger order) const;

  /** Measure accessor */
  Distribution getMeasure() const;

protected:

private:

} ; /* class OrthogonalUniVariateFunctionFamily */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFAMILY_HXX */
