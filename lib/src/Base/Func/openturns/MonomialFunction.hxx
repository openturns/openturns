//                                               -*- C++ -*-
/**
 *  @brief Monomial function implementation
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_MONOMIALFUNCTION_HXX
#define OPENTURNS_MONOMIALFUNCTION_HXX

#include "openturns/UniVariateFunctionImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class MonomialFunction
 *
 * Monomial function implementation
 */

class OT_API MonomialFunction
  : public UniVariateFunctionImplementation
{
  CLASSNAME
public:

  typedef Pointer<UniVariateFunctionImplementation> Implementation;

  /** Default constructor */
  explicit MonomialFunction();

  /** Standard constructor */
  MonomialFunction(const UnsignedInteger degree);

  /** Virtual constructor */
  virtual MonomialFunction * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** UniVariateFunctionImplementation are evaluated as functors */
  Scalar operator() (const Scalar x) const;

  /** UniVariateFunctionImplementation gradient */
  Scalar gradient(const Scalar x) const;

  /** UniVariateFunctionImplementation hessian */
  Scalar hessian(const Scalar x) const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

private:

  UnsignedInteger degree_;

} ; /* Class MonomialFunction */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MONOMIALFUNCTION_HXX */
