//                                               -*- C++ -*-
/**
 *  @brief This is a nD function build as a product of n 1D function
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
#ifndef OPENTURNS_PRODUCTUNIVARIATEFUNCTIONHESSIANIMPLEMENTATION_HXX
#define OPENTURNS_PRODUCTUNIVARIATEFUNCTIONHESSIANIMPLEMENTATION_HXX

#include "openturns/NumericalMathHessianImplementation.hxx"
#include "openturns/UniVariateFunction.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/NumericalPoint.hxx"



BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProductUniVariateFunctionHessianImplementation
 *
 * This is a nD function build as a product of n 1D function
 */

class OT_API ProductUniVariateFunctionHessianImplementation
  : public NumericalMathHessianImplementation
{
  CLASSNAME;
public:

  typedef Collection<UniVariateFunction>           UniVariateFunctionCollection;
  typedef PersistentCollection<UniVariateFunction> UniVariateFunctionPersistentCollection;

  /** Constructor */
  ProductUniVariateFunctionHessianImplementation(const UniVariateFunctionCollection & coll);

  /** Virtual constructor */
  virtual ProductUniVariateFunctionHessianImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Compute the hessian of a product of univariate polynomials */
  virtual SymmetricTensor hessian(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);



protected:

  friend class Factory<ProductUniVariateFunctionHessianImplementation>;

  /* Default constructor */
  ProductUniVariateFunctionHessianImplementation();

private:

  /** The set of 1d functions to build the nD function */
  UniVariateFunctionPersistentCollection functions_;

} ; /* class ProductUniVariateFunctionHessianImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTUNIVARIATEFUNCTIONHESSIANIMPLEMENTATION_HXX */
