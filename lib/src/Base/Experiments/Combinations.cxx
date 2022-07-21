//                                               -*- C++ -*-
/**
 *  @brief Implementation of the combinations experiment plane
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OTprivate.hxx"
#include "openturns/Combinations.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Combinations)

/* Default constructor */
Combinations::Combinations()
  : CombinatorialGeneratorImplementation()
  , k_(1)
  , n_(1)
{
  // Nothing to do
}

/* Constructor with parameters */
Combinations::Combinations(const UnsignedInteger k,
                           const UnsignedInteger n)
  : CombinatorialGeneratorImplementation()
  , k_(k)
  , n_(n)
{
  // Nothing to do
}

/* Virtual constructor */
Combinations * Combinations::clone() const
{
  return new Combinations(*this);
}

/* Experiment plane generation :
 *  all the combinations of k elements amongst {0, ..., n-1}
 */
IndicesCollection Combinations::generate() const
{
  /* Quick return for trivial cases */
  if (k_ > n_) return IndicesCollection(0, k_);
  if (k_ == 0) return IndicesCollection(1, 0);
  Indices indices(k_);
  indices.fill();
  if (k_ == n_) return IndicesCollection(1, k_, indices);
  /* Size of the sample to be generated: C(k, n) */
  const UnsignedInteger size = SpecFunc::BinomialCoefficient(n_, k_);
  IndicesCollection allCombinations(size, k_);
  for (UnsignedInteger flatIndex = 0; flatIndex < size; ++flatIndex)
  {
    std::copy(indices.begin(), indices.end(), allCombinations.begin_at(flatIndex));
    /* Update the indices */
    UnsignedInteger t = k_ - 1;
    while ((t != 0) && (indices[t] == n_ + t - k_)) --t;
    ++indices[t];
    for (UnsignedInteger i = t + 1; i < k_; ++i) indices[i] = indices[i - 1] + 1;
  }
  return allCombinations;
} // generate()

/* String converter */
String Combinations::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " k=" << k_
      << " n=" << n_;
  return oss;
}

/* Subset size accessor */
void Combinations::setK(const UnsignedInteger k)
{
  k_ = k;
}

UnsignedInteger Combinations::getK() const
{
  return k_;
}

/* Set size accessor */
void Combinations::setN(const UnsignedInteger n)
{
  n_ = n;
}

UnsignedInteger Combinations::getN() const
{
  return n_;
}

END_NAMESPACE_OPENTURNS
