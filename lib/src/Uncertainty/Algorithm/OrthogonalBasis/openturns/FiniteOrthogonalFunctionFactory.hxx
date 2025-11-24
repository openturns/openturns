//                                               -*- C++ -*-
/**
 *  @brief This is a finite orthogonal set of functions wrt a given
 *         distribution.
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_FINITEORTHOGONALFUNCTIONFACTORY_HXX
#define OPENTURNS_FINITEORTHOGONALFUNCTIONFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Function.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class FiniteOrthogonalFunctionFactory
 *
 * This is a finite set of orthogonal functions wrt a given distribution
 */

class OT_API FiniteOrthogonalFunctionFactory
  : public OrthogonalFunctionFactory
{
  CLASSNAME
public:

  typedef Collection<Function>           FunctionCollection;
  typedef PersistentCollection<Function> FunctionPersistentCollection;

  friend class Factory<FiniteOrthogonalFunctionFactory>;

  /** Default constructor */
  FiniteOrthogonalFunctionFactory();

  /** Constructor */
  FiniteOrthogonalFunctionFactory(const FunctionCollection & functions,
				  const Distribution & distribution);

  /** Build the Function of the given index */
  using OrthogonalFunctionFactory::build;
  Function build(const UnsignedInteger index) const override;

  /** Functions accessor */
  void setFunctionsCollection(const FunctionCollection & functions);
  FunctionCollection getFunctionsCollection() const;

  /** Virtual constructor */
  FiniteOrthogonalFunctionFactory * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** The underlying set of functions */
  FunctionPersistentCollection functions_;

} ; /* class FiniteOrthogonalFunctionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_FINITEORTHOGONALFUNCTIONFACTORY_HXX */

