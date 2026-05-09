//                                               -*- C++ -*-
/**
 * @brief The bijective function to select polynomials in the orthogonal basis
 *
 * Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OPENTURNS_HYPERBOLICENUMERATEFUNCTION_HXX
#define OPENTURNS_HYPERBOLICENUMERATEFUNCTION_HXX

#include <list>
#include <queue>
#include <unordered_set>
#include <vector>
#include <utility>

#include "openturns/EnumerateFunctionImplementation.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HyperbolicEnumerateFunction
 *
 * The bijective function to select polynomials in the orthogonal basis
 * using an isotropic hyperbolic truncation rule.
 */

class OT_API HyperbolicEnumerateFunction
  : public EnumerateFunctionImplementation
{
  CLASSNAME
public:

  typedef std::pair< Indices, Scalar > ValueType;

  /** Default constructor */
  HyperbolicEnumerateFunction();

  /** Parameter constructor */
  HyperbolicEnumerateFunction(const UnsignedInteger dimension,
                              const Scalar q = ResourceMap::GetAsScalar("HyperbolicEnumerateFunction-DefaultQ"));

  /** Virtual constructor */
  HyperbolicEnumerateFunction * clone() const override;

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

  /** Upper bound accessor */
  void setUpperBound(const Indices & upperBound) override;

  /** The marginal enumerate function */
  using EnumerateFunctionImplementation::getMarginal;
  EnumerateFunction getMarginal(const Indices & indices) const override;

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

protected:
  /** Returns the q-norm of the indice set */
  Scalar qNorm(const Indices & indices) const;

  /** Initialize the generation of indices */
  void initialize();

  /** Returns the maximum degree of the indice set */
  UnsignedInteger computeDegree(const Indices & indices) const;

private:

#ifndef SWIG
  /** Custom hash functor for Indices to be used in std::unordered_set */
  struct IndicesHash
  {
    std::size_t operator()(const Indices & indices) const;
  };

  /** Comparator for the priority queue (implements strict total ordering for a min-heap) */
  struct CandidateComparator
  {
    bool operator()(const ValueType & lhs, const ValueType & rhs) const;
  };

  typedef std::priority_queue<ValueType, std::vector<ValueType>, CandidateComparator> IndiceCandidates;
  typedef std::unordered_set<Indices, IndicesHash> IndiceVisitedSet;
#endif /* SWIG */

  /** Q-Norm q term */
  Scalar q_;

  /** Lookup table for cached power evaluations */
  mutable std::vector<Scalar> powLUT_;

  /** Cache of already generated indices */
  mutable IndiceCandidates candidates_;

  /** Hash set tracking all discovered indices to prevent duplicates */
  mutable IndiceVisitedSet visitedCandidates_;

  /** Candidate indices */
  mutable Collection<Indices> cache_;

  /** Cumulated strata cardinals */
  mutable Indices strataCumulatedCardinal_;

} ; /* class HyperbolicEnumerateFunction */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HYPERBOLICENUMERATEFUNCTION_HXX */