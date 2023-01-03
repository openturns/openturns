//                                               -*- C++ -*-
/**
 *  @brief Implementation of the kPermutations experiment plane
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
#include <cmath>
#include <algorithm>
#include <iterator>
#include "openturns/OTprivate.hxx"
#include "openturns/KPermutations.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KPermutations)

/* Default constructor */
KPermutations::KPermutations()
  : CombinatorialGeneratorImplementation()
  , k_(1)
  , n_(1)
{
  // Nothing to do
}

/* Constructor with parameters */
KPermutations::KPermutations(const UnsignedInteger n)
  : CombinatorialGeneratorImplementation()
  , k_(n)
  , n_(n)
{
  // Nothing to do
}

KPermutations::KPermutations(const UnsignedInteger k,
                             const UnsignedInteger n)
  : CombinatorialGeneratorImplementation()
  , k_(k)
  , n_(n)
{
  // Nothing to do
}

/* Virtual constructor */
KPermutations * KPermutations::clone() const
{
  return new KPermutations(*this);
}

/* Experiment plane generation :
 *  all the kPermutations of k elements amongst {0, ..., n-1}
 */
IndicesCollection KPermutations::generate() const
{
  /* Quick return for trivial cases */
  if (k_ > n_) return IndicesCollection(0, k_);
  if (k_ == 0) return IndicesCollection(1, 0);
  Indices indices(k_);
  indices.fill();
  /* Size of the sample to be generated: A(k, n) */
  const UnsignedInteger size = static_cast< UnsignedInteger >(round(exp(SpecFunc::LogGamma(n_ + 1) - SpecFunc::LogGamma(n_ - k_ + 1))));
  IndicesCollection allKPermutations(size, k_);
  /* First, generate all the permutations of k integers */
  IndicesCollection allPermutations(static_cast< UnsignedInteger >(round(exp(SpecFunc::LogGamma(k_ + 1)))), k_);
  std::copy(indices.begin(), indices.end(), allPermutations.begin_at(0));
  UnsignedInteger flatIndex = 1;
  while (std::next_permutation(indices.begin(), indices.end()))
  {
    std::copy(indices.begin(), indices.end(), allPermutations.begin_at(flatIndex));
    ++flatIndex;
  }
  /* Quick return if k == n */
  if (k_ == n_) return allPermutations;
  /* Second, generate all the combinations of k out of n elements */
  IndicesCollection allCombinations(Combinations(k_, n_).generate());
  flatIndex = 0;
  const UnsignedInteger combinationSize = allCombinations.getSize();
  const UnsignedInteger permutationSize = allPermutations.getSize();
  for (UnsignedInteger i = 0; i < combinationSize; ++i)
  {
    /* Generate all the permutations of the base combination */
    for (UnsignedInteger j = 0; j < permutationSize; ++j)
    {
      for (UnsignedInteger k = 0; k < k_; ++k) allKPermutations(flatIndex, k) = allCombinations(i, allPermutations(j, k));
      ++flatIndex;
    }
  }
  return allKPermutations;
} // generate()

/* String converter */
String KPermutations::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " k=" << k_
      << " n=" << n_;
  return oss;
}

/* Subset size accessor */
void KPermutations::setK(const UnsignedInteger k)
{
  k_ = k;
}

UnsignedInteger KPermutations::getK() const
{
  return k_;
}

/* Set size accessor */
void KPermutations::setN(const UnsignedInteger n)
{
  n_ = n;
}

UnsignedInteger KPermutations::getN() const
{
  return n_;
}

END_NAMESPACE_OPENTURNS
