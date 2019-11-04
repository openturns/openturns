//                                               -*- C++ -*-
/**
 *  @brief A factory for building ApproximationAlgorithmImplementation objects
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
#ifndef OPENTURNS_APPROXIMATIONALGORITHMIMPLEMENTATIONFACTORY_HXX
#define OPENTURNS_APPROXIMATIONALGORITHMIMPLEMENTATIONFACTORY_HXX

#include <iostream>
#include "openturns/ApproximationAlgorithmImplementation.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Collection.hxx"
#include "openturns/Function.hxx"
#include "openturns/TypedInterfaceObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ApproximationAlgorithmImplementationFactory
 *
 * A factory for building ApproximationAlgorithmImplementation objects
 */
class OT_API ApproximationAlgorithmImplementationFactory
  : public PersistentObject
{
  CLASSNAME
public:
  typedef Collection<Function> FunctionCollection;

  /** Default constructor */
  ApproximationAlgorithmImplementationFactory();

  /** Virtual constructor */
  virtual ApproximationAlgorithmImplementationFactory * clone() const;

  /** Method to create new ApproximationAlgorithmImplementation objects */
  virtual ApproximationAlgorithmImplementation * build (const Sample & x,
      const Sample & y,
      const FunctionCollection & psi,
      const Indices & indices) const;

  /** Method to create new ApproximationAlgorithmImplementation objects */
  virtual ApproximationAlgorithmImplementation * build (const Sample & x,
      const Sample & y,
      const Point & weight,
      const FunctionCollection & psi,
      const Indices & indices) const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

}; /* class ApproximationAlgorithmImplementationFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_APPROXIMATIONALGORITHMIMPLEMENTATIONFACTORY_HXX */
