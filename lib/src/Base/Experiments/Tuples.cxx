//                                               -*- C++ -*-
/**
 *  @brief Implementation of the tuples experiment plane
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
#include "openturns/Tuples.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Tuples)

/* Default constructor */
Tuples::Tuples()
  : CombinatorialGeneratorImplementation()
  , bounds_(1)
{
  restart();
}

/* Constructor with parameters */
Tuples::Tuples(const Indices & bounds)
  : CombinatorialGeneratorImplementation()
  , bounds_(bounds)
{
  restart();
}

/* Virtual constructor */
Tuples * Tuples::clone() const
{
  return new Tuples(*this);
}

/* Next tuple generation, stateful iteration like SplitterImplementation */
Indices Tuples::generateNext() const
{
  const UnsignedInteger size = getSize();
  if (currentIndex_ >= size)
    throw OutOfBoundException(HERE) << "No more tuples to generate";
  const Indices result(current_);
  ++currentIndex_;
  if (currentIndex_ < size)
  {
    const UnsignedInteger dimension = bounds_.getSize();
    /* Update the indices */
    ++current_[0];
    /* Propagate the remainders */
    for (UnsignedInteger i = 0; i < dimension - 1; ++i) current_[i + 1] += (current_[i] == bounds_[i]);
    /* Correction of the indices. The last index cannot overflow. */
    for (UnsignedInteger i = 0; i < dimension - 1; ++i) current_[i] = current_[i] % bounds_[i];
  }
  return result;
}

/* Number of tuples accessor */
UnsignedInteger Tuples::getSize() const
{
  /* Dimension of the realizations */
  const UnsignedInteger dimension = bounds_.getSize();
  /* Size of the sample to be generated: bounds[0] * ... * bounds[dimension-1] */
  UnsignedInteger size = std::min(1UL, dimension);
  const UnsignedInteger maxUInt = std::numeric_limits<UnsignedInteger>::max();
  for (UnsignedInteger i = 0; i < dimension; ++ i)
  {
    const UnsignedInteger bI = bounds_[i];
    if (bI == 0) return 0;
    if (size > maxUInt / bI)
      throw InvalidArgumentException(HERE) << "Tuples size would overflow integer limit " << maxUInt;
    size *= bI;
  }
  return size;
}

/* Dimension of the generated tuples accessor */
UnsignedInteger Tuples::getDimension() const
{
  return bounds_.getSize();
}

/* Restart the combinatorial sequence */
void Tuples::restart() const
{
  current_ = Indices(bounds_.getSize());
  currentIndex_ = 0;
}

/* String converter */
String Tuples::__repr__() const
{
  OSS oss;
  oss << "class=" << GetClassName()
      << " name=" << getName()
      << " bounds=" << bounds_;
  return oss;
}

/** Bounds accessor */
void Tuples::setBounds(const Indices & bounds)
{
  bounds_ = bounds;
  restart();
}

Indices Tuples::getBounds() const
{
  return bounds_;
}

END_NAMESPACE_OPENTURNS
