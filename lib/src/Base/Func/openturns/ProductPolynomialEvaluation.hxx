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
#ifndef OPENTURNS_PRODUCTPOLYNOMIALEVALUATION_HXX
#define OPENTURNS_PRODUCTPOLYNOMIALEVALUATION_HXX

#include "openturns/EvaluationImplementation.hxx"
#include "openturns/UniVariatePolynomial.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/Point.hxx"



BEGIN_NAMESPACE_OPENTURNS



/**
 * @class ProductPolynomialEvaluation
 *
 * This is a nD polynomial build as a product of n 1D polynomial
 */

class OT_API ProductPolynomialEvaluation
  : public EvaluationImplementation
{
  CLASSNAME
public:

  typedef Collection<UniVariatePolynomial>           PolynomialCollection;
  typedef PersistentCollection<UniVariatePolynomial> PolynomialPersistentCollection;

  /** Constructor */
  explicit ProductPolynomialEvaluation(const PolynomialCollection & coll);

  /** Virtual constructor */
  ProductPolynomialEvaluation * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Operator () */
  Point operator() (const Point & inP) const override;
  Sample operator() (const Sample & inS) const override;

  /** Accessor for input point dimension */
  UnsignedInteger getInputDimension() const override;

  /** Accessor for output point dimension */
  UnsignedInteger getOutputDimension() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;



protected:

  friend class Factory<ProductPolynomialEvaluation>;

  /* Default constructor */
  ProductPolynomialEvaluation();

private:
  /** The set of 1d polynomials to build the nD polynomial */
  PolynomialPersistentCollection polynomials_;

} ; /* class ProductPolynomialEvaluation */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PRODUCTPOLYNOMIALEVALUATION_HXX */
