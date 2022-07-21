//                                               -*- C++ -*-
/**
 *  @brief Univariate function
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
#ifndef OPENTURNS_UNIVARIATEFUNCTION_HXX
#define OPENTURNS_UNIVARIATEFUNCTION_HXX

#include "openturns/OTprivate.hxx"
#include "openturns/UniVariateFunctionImplementation.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class UniVariateFunction
 *
 * Univariate function
 */

class OT_API UniVariateFunction
  : public TypedInterfaceObject<UniVariateFunctionImplementation>
{
  CLASSNAME
public:

  /** Default constructor */
  UniVariateFunction();

  /** Constructor from implementation */
  UniVariateFunction(const UniVariateFunctionImplementation & implementation);

  /** Constructor from implementation pointer */
  UniVariateFunction(const Implementation & p_implementation);

#ifndef SWIG
  /** Constructor from implementation pointer */
  UniVariateFunction(UniVariateFunctionImplementation * p_implementation);
#endif

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** UniVariateFunction are evaluated as functors */
  Scalar operator() (const Scalar x) const;

  /** UniVariateFunctionImplementation gradient */
  Scalar gradient(const Scalar x) const;

  /** UniVariateFunctionImplementation hessian */
  Scalar hessian(const Scalar x) const;

  /** Method to draw the graph of the polynomial between given bounds */
  Graph draw(const Scalar xMin,
             const Scalar xMax,
             const UnsignedInteger pointNumber) const;
} ; /* class UniVariateFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_UNIVARIATEFUNCTION_HXX */
