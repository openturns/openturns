//                                               -*- C++ -*-
/**
 *  @brief The bijective function to select polynomials in the orthogonal basis
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
#include "openturns/NormInfEnumerateFunction.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(NormInfEnumerateFunction)

static const Factory<NormInfEnumerateFunction> Factory_NormInfEnumerateFunction;


/* Parameter constructor */
NormInfEnumerateFunction::NormInfEnumerateFunction(const UnsignedInteger dimension)
  : EnumerateFunctionImplementation(dimension)
  , multiIndices_(getDimension())
{
  // Nothing to do
}


/* Virtual constrcutor */
NormInfEnumerateFunction * NormInfEnumerateFunction::clone() const
{
  return new NormInfEnumerateFunction(*this);
}


/* String converter */
String NormInfEnumerateFunction::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " dimension=" << getDimension();
}


Indices NormInfEnumerateFunction::operator() (const UnsignedInteger index) const
{
  const UnsignedInteger dimension = getDimension();

  // reuse the previous state if next index is an unit increment
  if (index != (index_ + 1))
  {
    multiIndices_ = Indices(dimension);
    index_ = 0;
    strataIndex_ = 0;
  }

  while (index_ < index)
  {
    // find direction that can be incremented
    UnsignedInteger i = 0;
    while ((i < dimension) && (multiIndices_[i] == std::min(upperBound_[i], strataIndex_)))
    {
      ++ i;
    }

    // reset previous directions
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      multiIndices_[j] = 0;
    }

    // no more direction to update, go to next strata
    if (i == dimension)
    {
      ++ strataIndex_;
      continue;
    }

    // increment direction
    ++ multiIndices_[i];

    // count candidate only if part of the strata (at the surface of the cube)
    // ie it exist j such as c[j]=strataIndex
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      if (multiIndices_[j] == strataIndex_)
      {
        ++ index_;
        break;
      }
    }
  }
  return multiIndices_;
}


/* The cardinal of the given strata
 * = strataIndex^dimension-(strataIndex-1)^dimension
 */
UnsignedInteger NormInfEnumerateFunction::getStrataCardinal(const UnsignedInteger strataIndex) const
{
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger j = 0; j < dimension; ++ j)
    if (strataIndex > upperBound_[j])
      throw NotYetImplementedException(HERE) << "in NormInfEnumerateFunction::getStrataCardinal";

  if (strataIndex > 0)
    return getStrataCumulatedCardinal(strataIndex) - getStrataCumulatedCardinal(strataIndex - 1);
  else
    return 1;
}

/* The cardinal of the cumulated strata less or equal to the given strata
 * = strataIndex^dimension
 */
UnsignedInteger NormInfEnumerateFunction::getStrataCumulatedCardinal(const UnsignedInteger strataIndex) const
{
  const UnsignedInteger dimension = getDimension();
  for (UnsignedInteger j = 0; j < dimension; ++ j)
    if (strataIndex > upperBound_[j])
      throw NotYetImplementedException(HERE) << "in NormInfEnumerateFunction::getStrataCumulatedCardinal";

  return std::pow(1.0 * strataIndex, 1.0 * dimension);
}

/* The index of the strata of degree max < degree */
UnsignedInteger NormInfEnumerateFunction::getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const
{
  return maximumDegree / getDimension();
}

/* Method save() stores the object through the StorageManager */
void NormInfEnumerateFunction::save(Advocate & adv) const
{
  EnumerateFunctionImplementation::save(adv);
  // Dont serialize state: index_, multiIndices_
}


/* Method load() reloads the object from the StorageManager */
void NormInfEnumerateFunction::load(Advocate & adv)
{
  EnumerateFunctionImplementation::load(adv);
  // Dont serialize state: index_, multiIndices_
}


END_NAMESPACE_OPENTURNS
