//                                               -*- C++ -*-
/**
 *  @brief This is the natural orthogonal basis associated to a multidimensional
 *         distribution.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_SOIZEGHANEMFACTORY_HXX
#define OPENTURNS_SOIZEGHANEMFACTORY_HXX

#include "openturns/OrthogonalFunctionFactory.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/Indices.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Point.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Function.hxx"
#include "openturns/OrthogonalProductPolynomialFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class SoizeGhanemFactory
 *
 * This is the orthogonal polynomial basis
 */

class OT_API SoizeGhanemFactory
  : public OrthogonalFunctionFactory
{
  CLASSNAME
public:

  friend class Factory<SoizeGhanemFactory>;

  /** Default constructor */
  SoizeGhanemFactory();

  /** Constructor */
  explicit SoizeGhanemFactory(const Distribution & measure,
                              const Bool useCopula = true);

  /** Constructor */
  SoizeGhanemFactory(const Distribution & measure,
                     const EnumerateFunction & phi,
                     const Bool useCopula = true);

  /** Build the Function of the given index */
  using OrthogonalFunctionFactory::build;
  Function build(const UnsignedInteger index) const;

  /** Return the enumerate function that translate unidimensional indices into multidimensional indices */
  EnumerateFunction getEnumerateFunction() const;

  /** Virtual constructor */
  virtual SoizeGhanemFactory * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

private:

  /* Build the multivariate polynomial factory associated with the marginal distributions */
  void buildProductPolynomialAndAdaptation(const Bool useCopula);

  /** The underlying product polynomial factory */
  OrthogonalProductPolynomialFactory productPolynomial_;

  /** The Phi function */
  EnumerateFunction phi_;

  /** Has independent copula? */
  Bool hasIndependentCopula_;

  /** Adaptation factor */
  Function adaptationFactor_;
} ; /* class SoizeGhanemFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_SOIZEGHANEMFACTORY_HXX */

