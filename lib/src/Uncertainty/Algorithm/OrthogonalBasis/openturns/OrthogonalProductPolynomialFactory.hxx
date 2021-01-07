//                                               -*- C++ -*-
/**
 *  @brief This is the orthogonal polynomial basis
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ORTHOGONALPRODUCTPOLYNOMIALFACTORY_HXX
#define OPENTURNS_ORTHOGONALPRODUCTPOLYNOMIALFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Indices.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
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
  CLASSNAME
public:
  typedef Collection<Distribution>                                   DistributionCollection;
  typedef Collection<OrthogonalUniVariatePolynomialFamily>           PolynomialFamilyCollection;
  typedef PersistentCollection<OrthogonalUniVariatePolynomialFamily> PolynomialFamilyPersistentCollection;

  /** Default constructor */
  OrthogonalProductPolynomialFactory();

  /** Constructor */
  explicit OrthogonalProductPolynomialFactory(const PolynomialFamilyCollection & coll);

  /** Constructor */
  OrthogonalProductPolynomialFactory(const PolynomialFamilyCollection & coll,
                                     const EnumerateFunction & phi);

  /** Simplified constructor */
  explicit OrthogonalProductPolynomialFactory(const DistributionCollection & marginals);

  /** Build the Function of the given index */
  using OrthogonalFunctionFactory::build;
  Function build(const UnsignedInteger index) const override;

  /** Return the enumerate function that translate unidimensional indices into multidimensional indices */
  EnumerateFunction getEnumerateFunction() const override;

  /** Return the collection of univariate orthogonal polynomial families */
  PolynomialFamilyCollection getPolynomialFamilyCollection() const;

  /** Virtual constructor */
  OrthogonalProductPolynomialFactory * clone() const override;

  /** Nodes and weights of the multivariate polynomial associated with the marginal degrees indices[0], ...,indices[dimension] as the tensor product of the marginal orthogonal univariate polynomials, to build multivariate quadrature rules */
  Sample getNodesAndWeights(const Indices & degrees,
                            Point & weightsOut) const;

  /** String converter */
  String __repr__() const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

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
