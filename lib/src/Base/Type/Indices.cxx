//                                               -*- C++ -*-
/**
 *  @brief Set of indices defined as a Collection of UnsignedInteger class
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
#include <algorithm>
#include "openturns/Indices.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(Indices)
static const Factory<Indices> Factory_Indices;

/* Check that no value is repeated and no value exceeds the given bound */
Bool Indices::check(const UnsignedInteger bound) const
{
  if (!(getSize() > 0)) return true;
  // Check if the values are all different
  Indices copy(*this);
  std::sort(copy.begin(), copy.end());
  iterator iter = std::unique(copy.begin(), copy.end());
  if (iter < copy.end()) return false;
  // Check if the values are in the given bound
  if (*max_element(begin(), end()) >= bound) return false;
  return true;
}

/* Check if the indices are increasing */
Bool Indices::isIncreasing() const
{
  const UnsignedInteger size = getSize();
  if (!(size > 1)) return true;
  for (UnsignedInteger i = 1; i < size; ++i)
    if (operator[](i) < operator[](i - 1)) return false;
  return true;
}

/* Fill the indices with a linear progression, starting from start value by step stepsize */
void Indices::fill(const UnsignedInteger initialValue,
                   const UnsignedInteger stepSize)
{
  const UnsignedInteger size = getSize();
  UnsignedInteger value = initialValue;
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    operator[](i) = value;
    value += stepSize;
  }
}

/* Compute the complement between the indices and {0,...,n-1} for a given n */
Indices Indices::complement(const UnsignedInteger n) const
{
  // Check if the values are in the given bound
  const UnsignedInteger maxIndex = isEmpty() ? 0 : *max_element(begin(), end());
  if (!(maxIndex < n)) throw InvalidArgumentException(HERE) << "Error: the given bound=" << n << " must be greater than the maximum index=" << maxIndex;
  Indices flags(n, 1);
  for (UnsignedInteger i = 0; i < getSize(); ++i)
    flags[(*this)[i]] = 0;
  // The size of result is unknown as the base Indices may contain repeated values
  Indices result(0);
  for (UnsignedInteger i = 0; i < n; ++i)
    if (flags[i] == 1) result.add(i);
  return result;
}

END_NAMESPACE_OPENTURNS
