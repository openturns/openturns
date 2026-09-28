//                                               -*- C++ -*-
/**
 *  @brief Implementation of the kPermutations experiment plane
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
#include <algorithm>
#include "openturns/OTprivate.hxx"
#include "openturns/KPermutations.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(KPermutations)

/* Default constructor */
KPermutations::KPermutations()
  : CombinatorialGeneratorImplementation()
  , k_(1)
  , n_(1)
{
  restart();
}

/* Constructor with parameters */
KPermutations::KPermutations(const UnsignedInteger n)
  : CombinatorialGeneratorImplementation()
  , k_(n)
  , n_(n)
{
  restart();
}

KPermutations::KPermutations(const UnsignedInteger k,
                             const UnsignedInteger n)
  : CombinatorialGeneratorImplementation()
  , k_(k)
  , n_(n)
{
  restart();
}

/* Virtual constructor */
KPermutations * KPermutations::clone() const
{
  return new KPermutations(*this);
}

/* Next k-permutation generation, stateful iteration like SplitterImplementation */
Indices KPermutations::generateNext() const
{
  const UnsignedInteger size = getSize();
  if (currentIndex_ >= size)
    throw OutOfBoundException(HERE) << "No more k-permutations to generate";
  Indices result(k_);
  for (UnsignedInteger j = 0; j < k_; ++j) result[j] = currentCombination_[currentPermutation_[j]];
  ++currentIndex_;
  if (currentIndex_ < size)
  {
    if (!std::next_permutation(currentPermutation_.begin(), currentPermutation_.end()))
    {
      /* All the permutations of the current combination have been generated, move to the next combination */
      currentPermutation_.fill();
      UnsignedInteger t = k_ - 1;
      while ((t != 0) && (currentCombination_[t] == n_ + t - k_)) --t;
      ++currentCombination_[t];
      for (UnsignedInteger i = t + 1; i < k_; ++i) currentCombination_[i] = currentCombination_[i - 1] + 1;
    }
  }
  return result;
}

/* Number of k-permutations accessor: A(n, k) = n! / (n - k)! */
UnsignedInteger KPermutations::getSize() const
{
  if (k_ > n_) return 0;
  if (k_ == 0) return 1;
  const UnsignedInteger maxUInt = std::numeric_limits<UnsignedInteger>::max();
  UnsignedInteger size = 1;
  for (UnsignedInteger i = 0; i < k_; ++i)
  {
    const UnsignedInteger factor = n_ - i;
    if (size > maxUInt / factor)
      throw InvalidArgumentException(HERE) << "KPermutations size would overflow integer limit " << maxUInt;
    size *= factor;
  }
  return size;
}

/* Dimension of the generated k-permutations accessor */
UnsignedInteger KPermutations::getDimension() const
{
  return k_;
}

/* Restart the combinatorial sequence */
void KPermutations::restart() const
{
  currentIndex_ = 0;
  if (k_ > n_)
  {
    currentCombination_.clear();
    currentPermutation_.clear();
    return;
  }
  currentCombination_.resize(k_);
  currentCombination_.fill();
  currentPermutation_.resize(k_);
  currentPermutation_.fill();
}

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
  restart();
}

UnsignedInteger KPermutations::getK() const
{
  return k_;
}

/* Set size accessor */
void KPermutations::setN(const UnsignedInteger n)
{
  n_ = n;
  restart();
}

UnsignedInteger KPermutations::getN() const
{
  return n_;
}

END_NAMESPACE_OPENTURNS
