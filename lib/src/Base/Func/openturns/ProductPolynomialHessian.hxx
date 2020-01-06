//                                               -*- C++ -*-
/**
 *  @brief This is a nD polynomial build as a product of n 1D polynomial
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PRODUCTPOLYNOMIALHESSIAN_HXX
#define OPENTURNS_PRODUCTPOLYNOMIALHESSIAN_HXX

#include "openturns/HessianImplementation.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"



BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProductPolynomialHessian
 *
 * This is a nD polynomial build as a product of n 1D polynomial
 */

class OT_API ProductPolynomialHessian
  : public HessianImplementation
{
  CLASSNAME
public:

  typedef Collection<UniVariatePolynomial>           PolynomialCollection;
  typedef PersistentCollection<UniVariatePolynomial> PolynomialPersistentCollection;

  /** Constructor */
  ProductPolynomialHessian(const PolynomialCollection & coll);

  /** Virtual constructor */
  virtual ProductPolynomialHessian * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Compute the hessian of a product of univariate polynomials */
  virtual SymmetricTensor hessian(const Point & inP) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);



protected:

  friend class Factory<ProductPolynomialHessian>;

  /* Default constructor */
  ProductPolynomialHessian();

private:

  /** The set of 1d polynomials to build the nD polynomial */
  PolynomialPersistentCollection polynomials_;

} ; /* class ProductPolynomialHessian */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTPOLYNOMIALHESSIAN_HXX */
