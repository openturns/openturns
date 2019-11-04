//                                               -*- C++ -*-
/**
 *  @brief The bijective function to select polynomials in the orthogonal basis
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
#ifndef OPENTURNS_LINEARENUMERATEFUNCTION_HXX
#define OPENTURNS_LINEARENUMERATEFUNCTION_HXX

#include "openturns/EnumerateFunctionImplementation.hxx"


BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LinearEnumerateFunction
 *
 * The bijective function to select polynomials in the orthogonal basis
 */

class OT_API LinearEnumerateFunction
  : public EnumerateFunctionImplementation
{
  CLASSNAME
public:


  /** Default constructor */
  LinearEnumerateFunction();

  /** Parameter constructor */
  explicit LinearEnumerateFunction(const UnsignedInteger dimension);

  /** Virtual constrcutor */
  virtual LinearEnumerateFunction * clone() const;

  /** String converter */
  virtual String __repr__() const;

  /** The bijective association between an integer and a set of indices */
  virtual Indices operator() (const UnsignedInteger index) const;

  /** The inverse of the association */
  virtual UnsignedInteger inverse(const Indices & indices) const;

  /** The cardinal of the given strata */
  virtual UnsignedInteger getStrataCardinal(const UnsignedInteger strataIndex) const;

  /** The cardinal of the cumulated strata above or equal to the given strata */
  virtual UnsignedInteger getStrataCumulatedCardinal(const UnsignedInteger strataIndex) const;

  /** The index of the strata of degree max <= maximumDegree */
  virtual UnsignedInteger getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const;

  /** Method save() stores the object through the StorageManager */
  virtual void save(Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  virtual void load(Advocate & adv);

protected:

private:
  /** Find the smallest n such that Binomial(n, n + dimension) > index and also returns the value of Binomial(n, n + dimension) */
  UnsignedInteger findBinomialCoefficient(const UnsignedInteger index, const UnsignedInteger dimension, UnsignedInteger & BinomialCoefficient) const;

} ; /* class LinearEnumerateFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LINEARENUMERATEFUNCTION_HXX */
