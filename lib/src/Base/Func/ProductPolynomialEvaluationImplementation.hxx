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
#ifndef OPENTURNS_PRODUCTPOLYNOMIALEVALUATIONIMPLEMENTATION_HXX
#define OPENTURNS_PRODUCTPOLYNOMIALEVALUATIONIMPLEMENTATION_HXX

#include "NumericalMathEvaluationImplementation.hxx"
#include "UniVariatePolynomial.hxx"
#include "PersistentCollection.hxx"
#include "NumericalPoint.hxx"



BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProductPolynomialEvaluationImplementation
 *
 * This is a nD polynomial build as a product of n 1D polynomial
 */

class OT_API ProductPolynomialEvaluationImplementation
  : public NumericalMathEvaluationImplementation
{
  CLASSNAME;
public:

  typedef Collection<UniVariatePolynomial>           PolynomialCollection;
  typedef PersistentCollection<UniVariatePolynomial> PolynomialPersistentCollection;

  /** Constructor */
  ProductPolynomialEvaluationImplementation(const PolynomialCollection & coll);

  /** Virtual constructor */
  virtual ProductPolynomialEvaluationImplementation * clone() const;

  /** String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Operator () */
  virtual NumericalPoint operator() (const NumericalPoint & inP) const;
  virtual NumericalSample operator() (const NumericalSample & inS) const;

  /** Accessor for input point dimension */
  virtual UnsignedInteger getInputDimension() const;

  /** Accessor for output point dimension */
  virtual UnsignedInteger getOutputDimension() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);



protected:

  friend class Factory<ProductPolynomialEvaluationImplementation>;

  /* Default constructor */
  ProductPolynomialEvaluationImplementation();

private:
  /** The set of 1d polynomials to build the nD polynomial */
  PolynomialPersistentCollection polynomials_;

} ; /* class ProductPolynomialEvaluationImplementation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTPOLYNOMIALEVALUATIONIMPLEMENTATION_HXX */
