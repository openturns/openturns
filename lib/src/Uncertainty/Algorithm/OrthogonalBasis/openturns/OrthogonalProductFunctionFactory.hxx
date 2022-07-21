//                                               -*- C++ -*-
/**
 *  @brief This is the orthogonal function basis
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
#ifndef OPENTURNS_ORTHOGONALPRODUCTFUNCTIONFACTORY_HXX
#define OPENTURNS_ORTHOGONALPRODUCTFUNCTIONFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Function.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/OrthogonalUniVariateFunctionFamily.hxx"
#include "openturns/TensorizedUniVariateFunctionFactory.hxx"
#include "openturns/EnumerateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalProductFunctionFactory
 *
 * This is the orthogonal function basis
 */

class OT_API OrthogonalProductFunctionFactory
  : public OrthogonalFunctionFactory
{
  CLASSNAME
public:

  typedef Collection<OrthogonalUniVariateFunctionFamily>           FunctionFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariateFunctionFamily> FunctionFamilyPersistentCollection;

  friend class Factory<OrthogonalProductFunctionFactory>;

  /** Default constructor */
  OrthogonalProductFunctionFactory();

  /** Constructor */
  OrthogonalProductFunctionFactory(const FunctionFamilyCollection & coll);

  /** Constructor */
  OrthogonalProductFunctionFactory(const FunctionFamilyCollection & coll,
                                   const EnumerateFunction & phi);

  /** Build the Function of the given index */
  Function build(const UnsignedInteger index) const override;

  /** Return the enumerate function that translate unidimensional indices nto multidimensional indices */
  EnumerateFunction getEnumerateFunction() const override;

  /** Return the collection of univariate orthogonal polynomial families */
  FunctionFamilyCollection getFunctionFamilyCollection() const;

  /** Virtual constructor */
  OrthogonalProductFunctionFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:

private:
  /** Build tensorized function factory */
  void buildTensorizedFunctionFactory(const FunctionFamilyCollection & coll,
                                      const EnumerateFunction & phi);

  /** Build the measure based on the one found in the family collection */
  using OrthogonalFunctionFactory::build;
  void buildMeasure(const FunctionFamilyCollection & coll);

  /** The product function factory */
  TensorizedUniVariateFunctionFactory tensorizedFunctionFactory_;

} ; /* class OrthogonalProductFunctionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALPRODUCTFUNCTIONFACTORY_HXX */
