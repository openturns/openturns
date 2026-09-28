//                                               -*- C++ -*-
/**
 *  @brief Implementation of the combinations experiment plane
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
  restart();
}

/* Constructor with parameters */
Combinations::Combinations(const UnsignedInteger k,
                           const UnsignedInteger n)
  : CombinatorialGeneratorImplementation()
  , k_(k)
  , n_(n)
{
  restart();
}

/* Virtual constructor */
Combinations * Combinations::clone() const
{
  return new Combinations(*this);
}

/* Next combination generation, stateful iteration like SplitterImplementation */
Indices Combinations::generateNext() const
{
  const UnsignedInteger size = getSize();
  if (currentIndex_ >= size)
    throw OutOfBoundException(HERE) << "No more combinations to generate";
  const Indices result(current_);
  ++currentIndex_;
  if ((currentIndex_ < size) && (k_ > 0))
  {
    /* Update the indices to the next combination in lexical order */
    UnsignedInteger t = k_ - 1;
    while ((t != 0) && (current_[t] == n_ + t - k_)) --t;
    ++current_[t];
    for (UnsignedInteger i = t + 1; i < k_; ++i) current_[i] = current_[i - 1] + 1;
  }
  return result;
}

/* Number of combinations accessor */
UnsignedInteger Combinations::getSize() const
{
  if (k_ > n_) return 0;
  if ((k_ == 0) || (k_ == n_)) return 1;
  return SpecFunc::BinomialCoefficient(n_, k_);
}

/* Dimension of the generated combinations accessor */
UnsignedInteger Combinations::getDimension() const
{
  return k_;
}

/* Restart the combinatorial sequence */
void Combinations::restart() const
{
  current_.resize(k_);
  if (k_ <= n_) current_.fill();
  currentIndex_ = 0;
}

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
  restart();
}

UnsignedInteger Combinations::getK() const
{
  return k_;
}

/* Set size accessor */
void Combinations::setN(const UnsignedInteger n)
{
  n_ = n;
  restart();
}

UnsignedInteger Combinations::getN() const
{
  return n_;
}

END_NAMESPACE_OPENTURNS
