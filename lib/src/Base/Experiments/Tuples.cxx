//                                               -*- C++ -*-
/**
 *  @brief Implementation of the tuples experiment plane
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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
  // Nothing to do
}

/* Constructor with parameters */
Tuples::Tuples(const Indices & bounds)
  : CombinatorialGeneratorImplementation()
  , bounds_(bounds)
{
  // Nothing to do
}

/* Virtual constructor */
Tuples * Tuples::clone() const
{
  return new Tuples(*this);
}

/* Experiment plane generation :
 *  all the tuples taking values in {0,...,bounds[0]-1}x...x{0,...,bounds[n-1]-1}
 */
IndicesCollection Tuples::generate()
{
  /* Dimension of the realizations */
  const UnsignedInteger dimension = bounds_.getSize();
  /* Size of the sample to be generated: levels[0] * ... * levels[dimension-1] */
  UnsignedInteger size = bounds_[0];
  for (UnsignedInteger i = 1; i < dimension; ++i) size *= bounds_[i];
  IndicesCollection allTuples(size, dimension);
  /* Indices would have stored the indices of the nested loops if we were able to code "dimension" nested loops dynamically */
  Indices indices(dimension);
  for (UnsignedInteger flatIndex = 0; flatIndex < size; ++flatIndex)
  {
    std::copy(indices.begin(), indices.end(), allTuples.begin_at(flatIndex));
    /* Update the indices */
    ++indices[0];
    /* Propagate the remainders */
    for (UnsignedInteger i = 0; i < dimension - 1; ++i) indices[i + 1] += (indices[i] == bounds_[i]);
    /* Correction of the indices. The last index cannot overflow. */
    for (UnsignedInteger i = 0; i < dimension - 1; ++i) indices[i] = indices[i] % bounds_[i];
  }
  return allTuples;
} // generate()

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
}

Indices Tuples::getBounds() const
{
  return bounds_;
}

END_NAMESPACE_OPENTURNS
