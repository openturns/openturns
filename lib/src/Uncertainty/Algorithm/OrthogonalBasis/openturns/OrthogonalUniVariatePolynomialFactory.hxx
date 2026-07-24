//                                               -*- C++ -*-
/**
 *  @brief This an abstract class for 1D polynomial factories
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIALFACTORY_HXX
#define OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIALFACTORY_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Point.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/OrthogonalUniVariatePolynomial.hxx"
#include "openturns/Distribution.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalUniVariatePolynomialFactory
 *
 * This an abstract class for 1D polynomial factories
 */

class OT_API OrthogonalUniVariatePolynomialFactory
  : public PersistentObject
{
  CLASSNAME

public:

  typedef OrthogonalUniVariatePolynomial::Coefficients Coefficients;
  typedef Collection<Coefficients>                     CoefficientsCollection;
  typedef PersistentCollection<Coefficients>           CoefficientsPersistentCollection;
  typedef Collection<OrthogonalUniVariatePolynomial>   OrthogonalUniVariatePolynomialCollection;

  enum ParameterSet { ANALYSIS, PROBABILITY };

  /** Constructor */
  explicit OrthogonalUniVariatePolynomialFactory(const Distribution & measure);

  /** Virtual constructor */
  OrthogonalUniVariatePolynomialFactory * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** The method to get the polynomial of any degree. */
  OrthogonalUniVariatePolynomial build(const UnsignedInteger degree) const;

  /** Build the 3 terms recurrence coefficients up to the needed degree */
  Sample buildRecurrenceCoefficientsCollection(const UnsignedInteger degree) const;

  /** Roots of the polynomial of degree n as the eigenvalues of the associated matrix */
  virtual Point getRoots(const UnsignedInteger n) const;

  /** Nodes and weights of the polynomial of degree n as the eigenvalues of the associated matrix, to build quadrature rules */
  virtual Point getNodesAndWeights(const UnsignedInteger n,
                                   Point & weightsOut) const;

  /** Measure accessor */
  Distribution getMeasure() const;

  /** Affine coefficients accessors */
  Scalar getA() const;
 protected:
  void setA(const Scalar a);
 public:
  Scalar getB() const;
 protected:
  void setB(const Scalar b);
 public:
  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;


  /** Calculate the coefficients of recurrence a0, a1, a2 such that
      Pn+1(x) = (a0 * x + a1) * Pn(x) + a2 * Pn-1(x) */
  virtual Coefficients getRecurrenceCoefficients(const UnsignedInteger n) const;

protected:

  friend class Factory<OrthogonalUniVariatePolynomialFactory>;

  /** Default constructor */
  OrthogonalUniVariatePolynomialFactory();

  /** Cache initialization */
  virtual void initializeCache();

  /** The distribution of the particular Orthonormal polynomial */
  Distribution measure_;

  /** The affine transformation mapping the measure to its standard representative */
  Scalar a_ = 1.0;
  Scalar b_ = 0.0;

  /** A cache to save already computed recurrence coefficients */
  mutable Sample recurrenceCoefficientsCache_;

  /** A cache to save already computed polynomials */
  mutable OrthogonalUniVariatePolynomialCollection polynomialsCache_;
} ; /* class OrthogonalUniVariatePolynomialFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALUNIVARIATEPOLYNOMIALFACTORY_HXX */
