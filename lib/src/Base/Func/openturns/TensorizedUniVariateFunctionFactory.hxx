//                                               -*- C++ -*-
/**
 *  @brief This is the tensorized function basis
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
#ifndef OPENTURNS_TENSORIZEDUNIVARIATEFUNCTIONFACTORY_HXX
#define OPENTURNS_TENSORIZEDUNIVARIATEFUNCTIONFACTORY_HXX

#include "openturns/BasisImplementation.hxx"
#include "openturns/EnumerateFunction.hxx"
#include "openturns/Function.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/UniVariateFunctionFamily.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class TensorizedUniVariateFunctionFactory
 *
 * This is the  function basis
 */

class OT_API TensorizedUniVariateFunctionFactory
  : public BasisImplementation
{
  CLASSNAME
public:

  typedef Collection<UniVariateFunctionFamily>           FunctionFamilyCollection;
  typedef PersistentCollection<UniVariateFunctionFamily> FunctionFamilyPersistentCollection;

  friend class Factory<TensorizedUniVariateFunctionFactory>;

  /** Default constructor */
  TensorizedUniVariateFunctionFactory();

  /** Constructor */
  explicit TensorizedUniVariateFunctionFactory(const FunctionFamilyCollection & coll);

  /** Constructor */
  TensorizedUniVariateFunctionFactory(const FunctionFamilyCollection & coll,
                                      const EnumerateFunction & phi);

  /** Build the Function of the given index */
  Function build(const UnsignedInteger index) const override;

  /** Return the enumerate function that translate unidimensional indices into multidimensional indices */
  EnumerateFunction getEnumerateFunction() const;

  /** set the enumerate function that translate unidimensional indices into multidimensional indices */
  void setEnumerateFunction(const EnumerateFunction & phi);

  /** set the function familly collection */
  FunctionFamilyCollection getFunctionFamilyCollection() const;
  void setFunctionFamilyCollection(const FunctionFamilyCollection & coll);

  /** Virtual constructor */
  TensorizedUniVariateFunctionFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:

  /** The 1D function family collection */
  FunctionFamilyPersistentCollection coll_;

  /** The enumeration function that maps a linear index into a multi-indices */
  EnumerateFunction phi_;

} ; /* class TensorizedUniVariateFunctionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TENSORIZEDUNIVARIATEFUNCTIONFACTORY_HXX */
