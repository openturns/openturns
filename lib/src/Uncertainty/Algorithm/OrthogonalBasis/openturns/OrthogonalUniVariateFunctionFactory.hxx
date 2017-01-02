//                                               -*- C++ -*-
/**
 *  @brief This an abstract class for 1D function factories
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
#ifndef OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFACTORY
#define OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFACTORY

#include "openturns/PersistentObject.hxx"
#include "openturns/PersistentCollection.hxx"
#include "openturns/OrthogonalUniVariatePolynomial.hxx"
#include "openturns/Distribution.hxx"
#include "openturns/UniVariateFunction.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class OrthogonalUniVariateFunctionFactory
 *
 * This an abstract class for 1D function factories
 */

class OT_API OrthogonalUniVariateFunctionFactory
  : public PersistentObject
{
  CLASSNAME;

public:

  typedef Collection<UniVariateFunction> UniVariateFunctionCollection;

  enum ParameterSet { ANALYSIS, PROBABILITY };

  /** Constructor */
  explicit OrthogonalUniVariateFunctionFactory(const Distribution & measure);

  /** Virtual constructor */
  virtual OrthogonalUniVariateFunctionFactory * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** The method to get the function of any order. */
  virtual UniVariateFunction build(const UnsignedInteger order) const;

  /** Measure accessor */
  Distribution getMeasure() const;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv);

protected:

  friend class Factory<OrthogonalUniVariateFunctionFactory>;

  /** Default constructor */
  OrthogonalUniVariateFunctionFactory();

  /** Cache initialization */
  virtual void initializeCache();

  /** The distribution of the particular Orthonormal polynomial */
  Distribution measure_;

  /** A cache to save already computed functions */
  mutable UniVariateFunctionCollection functionsCache_;
} ; /* class OrthogonalUniVariateFunctionFactory */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ORTHOGONALUNIVARIATEFUNCTIONFACTORY */
