//                                               -*- C++ -*-
/**
 *  @brief This is the orthogonal function basis
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
#ifndef OPENTURNS_ORTHOGONALPRODUCTFUNCTIONFACTORY_HXX
#define OPENTURNS_ORTHOGONALPRODUCTFUNCTIONFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Indices.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/OrthogonalUniVariateFunctionFamily.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalProductFunctionFactory
 *
 * This is the orthogonal function basis
 */

class OT_API OrthogonalProductFunctionFactory
  : public OrthogonalFunctionFactory
{
  CLASSNAME;
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

  /** Build the NumericalMathFunction of the given index */
  NumericalMathFunction build(const UnsignedInteger index) const;

  /** Return the enumerate function that translate unidimensional indices nto multidimensional indices */
  EnumerateFunction getEnumerateFunction() const;

  /** Return the collection of univariate orthogonal polynomial families */
  FunctionFamilyCollection getFunctionFamilyCollection() const;

  /** Virtual constructor */
  virtual OrthogonalProductFunctionFactory * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

private:
  /** Build the measure based on the one found in the family collection */
  void buildMeasure();

  /** The 1D polynomial family collection */
  FunctionFamilyPersistentCollection coll_;

  /** The Phi function */
  EnumerateFunction phi_;

} ; /* class OrthogonalProductFunctionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALPRODUCTFUNCTIONFACTORY_HXX */
