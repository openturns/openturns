//                                               -*- C++ -*-
/**
 *  @brief This is the orthogonal polynomial basis
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#ifndef OPENTURNS_ORTHOGONALPRODUCTPOLYNOMIALFACTORY_HXX
#define OPENTURNS_ORTHOGONALPRODUCTPOLYNOMIALFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Indices.hxx"
#include "openturns/NumericalPoint.hxx"
#include "openturns/NumericalSample.hxx"
#include "openturns/NumericalMathFunction.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/OrthogonalUniVariatePolynomialFamily.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class OrthogonalProductPolynomialFactory
 *
 * This is the orthogonal polynomial basis
 */

class OT_API OrthogonalProductPolynomialFactory
  : public OrthogonalFunctionFactory
{
  CLASSNAME;
public:

  typedef Collection<OrthogonalUniVariatePolynomialFamily>           PolynomialFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariatePolynomialFamily> PolynomialFamilyPersistentCollection;

  friend class Factory<OrthogonalProductPolynomialFactory>;

  /** Default constructor */
  OrthogonalProductPolynomialFactory();

  /** Constructor */
  OrthogonalProductPolynomialFactory(const PolynomialFamilyCollection & coll);

  /** Constructor */
  OrthogonalProductPolynomialFactory(const PolynomialFamilyCollection & coll,
                                     const EnumerateFunction & phi);

  /** Build the NumericalMathFunction of the given index */
  NumericalMathFunction build(const UnsignedInteger index) const;

  /** Return the enumerate function that translate unidimensional indices into multidimensional indices */
  EnumerateFunction getEnumerateFunction() const;

  /** Return the collection of univariate orthogonal polynomial families */
  PolynomialFamilyCollection getPolynomialFamilyCollection() const;

  /** Virtual constructor */
  virtual OrthogonalProductPolynomialFactory * clone() const;

  /** Nodes and weights of the multivariate polynomial associated with the marginal degrees indices[0], ...,indices[dimension] as the tensor product of the marginal orthogonal univariate polynomials, to build multivariate quadrature rules */
  NumericalSample getNodesAndWeights(const Indices & degrees,
                                     NumericalPoint & weights) const;

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
  PolynomialFamilyPersistentCollection coll_;

  /** The Phi function */
  EnumerateFunction phi_;

} ; /* class OrthogonalProductPolynomialFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALPRODUCTPOLYNOMIALFACTORY_HXX */
