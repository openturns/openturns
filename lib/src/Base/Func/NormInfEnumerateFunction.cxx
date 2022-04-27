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



/* Default constructor */
NormInfEnumerateFunction::NormInfEnumerateFunction()
  : EnumerateFunctionImplementation()
  , index_(0)
  , multiIndices_(getDimension())
{
  // Nothing to do
}

/* Parameter constructor */
NormInfEnumerateFunction::NormInfEnumerateFunction(const UnsignedInteger dimension)
  : EnumerateFunctionImplementation(dimension)
  , index_(0)
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
  const UnsignedInteger strataIndex = std::pow(1.0 * index, 1.0 / dimension);
  const UnsignedInteger cumulatedCardinal = std::pow(1.0 * strataIndex, 1.0 * dimension);

  // reuse the previous state if next index is an unit increment and we dont change strata
  if ((index != (index_ + 1))
      || (index == cumulatedCardinal))
  {
    // restart at beginning of the strata
    index_ = cumulatedCardinal;
    multiIndices_ = Indices(dimension);
    multiIndices_[0] = strataIndex;
  }

  while (index_ < index)
  {
    // find direction that can be incremented
    UnsignedInteger i = 0;
    while (multiIndices_[i] == strataIndex)
    {
      ++ i;
    }

    // reset previous directions
    for (UnsignedInteger j = 0; j < i; ++ j)
    {
      multiIndices_[j] = 0;
    }

    // increment direction
    ++ multiIndices_[i];

    // count candidate only if part of the strata (at the surface of the cube)
    // ie it exist j such as c[j]=strataIndex
    for (UnsignedInteger j = 0; j < dimension; ++ j)
    {
      if (multiIndices_[j] == strataIndex)
      {
        ++ index_;
        break;
      }
    }
  }
  return multiIndices_;
}


UnsignedInteger NormInfEnumerateFunction::inverse(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = indices.getSize();
  if (size != dimension) throw InvalidArgumentException(HERE)  << "Error: the size of the given indices must match the dimension, here size=" << size << " and dimension=" << dimension;

  // start from the beginning of the strata
  UnsignedInteger strataIndex = 0;
  for (UnsignedInteger i = 0; i < dimension; ++ i)
    strataIndex = std::max(strataIndex, indices[i]);
  UnsignedInteger index = strataIndex > 0 ? std::pow(1.0 * strataIndex - 1, 1.0 * dimension) : 0;
  while (operator()(index) != indices)
  {
    ++ index;
  }
  return index;
}


/* The cardinal of the given strata
 * = strataIndex^dimension-(strataIndex-1)^dimension
 */
UnsignedInteger NormInfEnumerateFunction::getStrataCardinal(const UnsignedInteger strataIndex) const
{
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
