//                                               -*- C++ -*-
/**
 *  @brief Univariate function implementation
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
#ifndef OPENTURNS_UNIVARIATEFUNCTIONIMPLEMENTATION_HXX
#define OPENTURNS_UNIVARIATEFUNCTIONIMPLEMENTATION_HXX

#include "openturns/Graph.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class UniVariateFunctionImplementation
 *
 * Univariate function implementation
 */

class OT_API UniVariateFunctionImplementation
  : public PersistentObject
{
  CLASSNAME;
public:

  typedef Pointer<UniVariateFunctionImplementation> Implementation;

  /** Default constructor */
  UniVariateFunctionImplementation();

  /** Virtual constructor */
  virtual UniVariateFunctionImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** UniVariateFunctionImplementation are evaluated as functors */
  virtual NumericalScalar operator() (const NumericalScalar x) const;

  /** UniVariateFunctionImplementation gradient */
  virtual NumericalScalar gradient(const NumericalScalar x) const;

  /** UniVariateFunctionImplementation hessian */
  virtual NumericalScalar hessian(const NumericalScalar x) const;

  /** Method to draw the graph of the function between given bounds */
  virtual Graph draw (const NumericalScalar xMin,
                      const NumericalScalar xMax,
                      const UnsignedInteger pointNumber) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

} ; /* Class UniVariateFunctionImplementation */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_UNIVARIATEFUNCTIONIMPLEMENTATION_HXX */
