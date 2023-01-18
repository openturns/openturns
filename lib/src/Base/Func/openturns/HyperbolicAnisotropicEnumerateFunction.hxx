//                                               -*- C++ -*-
/**
 *  @brief The bijective function to select polynomials in the orthogonal basis
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_HYPERBOLICANISOTROPICENUMERATEFUNCTION_HXX
#define OPENTURNS_HYPERBOLICANISOTROPICENUMERATEFUNCTION_HXX

#include <list>

#include "openturns/EnumerateFunctionImplementation.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HyperbolicAnisotropicEnumerateFunction
 *
 * The bijective function to select polynomials in the orthogonal basis
 */

class OT_API HyperbolicAnisotropicEnumerateFunction
  : public EnumerateFunctionImplementation
{
  CLASSNAME
public:

  typedef std::pair< Indices, Scalar > ValueType;
  typedef std::list<ValueType>                   IndiceCache;

  /** Default constructor */
  HyperbolicAnisotropicEnumerateFunction();

  /** Parameter constructor */
  HyperbolicAnisotropicEnumerateFunction(const UnsignedInteger dimension,
                                         const Scalar q = ResourceMap::GetAsScalar("HyperbolicAnisotropicEnumerateFunction-DefaultQ"));

  /** Parameter constructor */
  HyperbolicAnisotropicEnumerateFunction(const Point & weight,
                                         const Scalar q = ResourceMap::GetAsScalar("HyperbolicAnisotropicEnumerateFunction-DefaultQ"));

  /** Virtual constructor */
  HyperbolicAnisotropicEnumerateFunction * clone() const override;

  /** String converter */
  String __repr__() const override;

  /** The bijective association between an integer and a set of indices */
  Indices operator() (const UnsignedInteger index) const override;

  /** The inverse of the association */
  UnsignedInteger inverse(const Indices & indices) const override;

  /** The cardinal of the given strata */
  UnsignedInteger getStrataCardinal(const UnsignedInteger strataIndex) const override;

  /** The cardinal of the cumulated strata above or equal to the given strata */
  UnsignedInteger getStrataCumulatedCardinal(const UnsignedInteger strataIndex) const override;

  /** The index of the strata of degree max < degree */
  UnsignedInteger getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const override;

  /** Q accessor */
  void setQ(const Scalar q);
  Scalar getQ() const;

  /** Weight accessor */
  void setWeight(const Point & weight);
  Point getWeight() const;

  /** Upper bound accessor */
  void setUpperBound(const Indices & upperBound) override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Initialize the generation of indices */
  void initialize();

  /** Returns the q-norm of the indice set */
  Scalar qNorm(const Indices & indices) const;

  /** Returns the maximum degree of the indice set */
  UnsignedInteger computeDegree(const Indices & indices) const;

private:

  /** Weight of each component */
  Point weight_;

  /** Q-Norm q term */
  Scalar q_;

  /** Cache of already generated indices */
  mutable IndiceCache candidates_;

  /** Candidate indices */
  mutable Collection<Indices> cache_;

  /** Cumulated strata cardinals */
  mutable Indices strataCumulatedCardinal_;

} ; /* class HyperbolicAnisotropicEnumerateFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HYPERBOLICANISOTROPICENUMERATEFUNCTION_HXX */
