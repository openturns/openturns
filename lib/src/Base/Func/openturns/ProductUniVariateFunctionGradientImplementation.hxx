//                                               -*- C++ -*-
/**
 *  @brief This is a nD polynomial build as a product of n 1D polynomial
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
#ifndef OPENTURNS_PRODUCTPOLYNOMIALGRADIENTIMPLEMENTATION_HXX
#define OPENTURNS_PRODUCTPOLYNOMIALGRADIENTIMPLEMENTATION_HXX

#include "openturns/NumericalMathGradientImplementation.hxx"
#include "openturns/ProductUniVariateFunctionEvaluationImplementation.hxx"


BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProductUniVariateFunctionGradientImplementation
 *
 * This is a nD polynomial build as a product of n 1D polynomial
 */

class OT_API ProductUniVariateFunctionGradientImplementation
  : public NumericalMathGradientImplementation
{
  CLASSNAME;
public:

  /** Constructor */
  ProductUniVariateFunctionGradientImplementation(const Pointer<ProductUniVariateFunctionEvaluationImplementation> & p_evaluation);

  /** Virtual constructor */
  virtual ProductUniVariateFunctionGradientImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Compute the gradient of a product of univariate polynomials */
  virtual Matrix gradient(const NumericalPoint & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);



protected:

  friend class Factory<ProductUniVariateFunctionGradientImplementation>;

  /* Default constructor */
  ProductUniVariateFunctionGradientImplementation();

private:

  Pointer<ProductUniVariateFunctionEvaluationImplementation> p_evaluation_;

} ; /* class ProductUniVariateFunctionGradientImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTPOLYNOMIALGRADIENTIMPLEMENTATION_HXX */
