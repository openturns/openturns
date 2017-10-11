//                                               -*- C++ -*-
/**
 *  @brief This is an abstract class for basis
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_FUNCTIONALBASISIMPLEMENTATION_HXX
#define OPENTURNS_FUNCTIONALBASISIMPLEMENTATION_HXX

#include "openturns/BasisImplementation.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class FunctionalBasisImplementation
 *
 * This is an abstract class for orthogonal basis
 */

class OT_API FunctionalBasisImplementation
  : public BasisImplementation
{
  CLASSNAME
public:
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  FunctionalBasisImplementation();

  /** Virtual constructor */
  virtual FunctionalBasisImplementation * clone() const;

  virtual Bool isFunctional() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);


protected:

private:

} ; /* class FunctionalBasisImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FUNCTIONALBASISIMPLEMENTATION_HXX */
