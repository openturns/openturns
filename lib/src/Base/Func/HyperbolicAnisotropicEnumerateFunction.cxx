//                                               -*- C++ -*-
/**
 *  @brief The bijective function to select polynomials in the orthogonal basis
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
#include <algorithm>
#include "openturns/HyperbolicAnisotropicEnumerateFunction.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/SpecFunc.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HyperbolicAnisotropicEnumerateFunction)

static const Factory<HyperbolicAnisotropicEnumerateFunction> Factory_HyperbolicAnisotropicEnumerateFunction;

/* Default constructor */
HyperbolicAnisotropicEnumerateFunction::HyperbolicAnisotropicEnumerateFunction()
  : EnumerateFunctionImplementation()
  , weight_()
  , q_(0.0)
{
  // Nothing to do
}

/* Parameter constructor */
HyperbolicAnisotropicEnumerateFunction::HyperbolicAnisotropicEnumerateFunction(const UnsignedInteger dimension,
    const Scalar q)
  : EnumerateFunctionImplementation(dimension)
  , weight_(dimension, 1.0)
{
  setQ(q);
  initialize();
}


/* Parameter constructor */
HyperbolicAnisotropicEnumerateFunction::HyperbolicAnisotropicEnumerateFunction(const Point & weight,
    const Scalar q)
  : EnumerateFunctionImplementation(weight.getDimension())
  , weight_(weight)
{
  setQ(q);
  initialize();
}


/* Virtual constructor */
HyperbolicAnisotropicEnumerateFunction * HyperbolicAnisotropicEnumerateFunction::clone() const
{
  return new HyperbolicAnisotropicEnumerateFunction(*this);
}


/* String converter */
String HyperbolicAnisotropicEnumerateFunction::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " derived from " << EnumerateFunctionImplementation::__repr__()
         << " q=" << q_
         << " weights=" << weight_;
}


void HyperbolicAnisotropicEnumerateFunction::initialize()
{
  cache_.clear();
  candidates_.clear();
  // insert indice 0, with q-norm 0.0 in the candidate list
  ValueType zero(Indices(getDimension(), 0), 0.0);
  candidates_.insert(candidates_.begin(), zero);
}


/* Returns the q-norm of the indice set */
Scalar HyperbolicAnisotropicEnumerateFunction::qNorm(const Indices & indices) const
{
  Scalar result = 0.0;
  UnsignedInteger dimension = indices.getSize();
  if (q_ == 1.0)
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
      result += indices[j] * weight_[j];
    return result;
  }
  for (UnsignedInteger j = 0; j < dimension; ++ j)
    result += pow(indices[j] * weight_[j], q_);
  return pow(result, 1.0 / q_);
}

UnsignedInteger HyperbolicAnisotropicEnumerateFunction::computeDegree(const Indices& indices) const
{
  return *max_element(indices.begin(), indices.end());
}

/*
* We start from the zero-filled indice set,
* and for every indice set asked, compute its immediate neighbors and add them to the list of candidates
* this list contains all the potential next indices, sorted according to their q-norm
* so the next indice set is the first in the list, i.e. the one closest to the origin
* then we put it in a vector, allowing for fast retrieval according to the index
*/
Indices HyperbolicAnisotropicEnumerateFunction::operator() (const UnsignedInteger index) const
{
  // if we haven't generated enough indices, generate them
  for (UnsignedInteger i = cache_.getSize(); i <= index; ++ i)
  {
    if (candidates_.empty())
      throw NotDefinedException(HERE) << "Cannot enumerate up to index=" << index << " because of the bounds.";

    // the current indice is the first candidate in the list as we maintain q-norm sorting
    ValueType current(candidates_.front());

    // move it to cache
    candidates_.pop_front();

    // detect a norm leap
    if ((cache_.getSize() > 0) && (current.second > qNorm(cache_[cache_.getSize() - 1]))) strataCumulatedCardinal_.add( cache_.getSize() );

    cache_.add(current.first);

    // generate all the neighbours indices
    for(UnsignedInteger j = 0; j < getDimension(); ++ j)
    {
      Indices nextIndices(current.first);
      if (nextIndices[j] >= upperBound_[j])
        continue;
      ++ nextIndices[j];

      Scalar nextNorm = qNorm(nextIndices);
      ValueType next(nextIndices, nextNorm);
      IndiceCache::iterator it = candidates_.begin();

      // we'll try to insert the indice in the list according to its q-norm
      while ((it != candidates_.end()) && (it->second < nextNorm)) ++ it;

      // check if the same indice was already added
      bool duplicate( false );
      while ((it != candidates_.end()) && (it->second == nextNorm))
      {
        if (it->first == nextIndices)
        {
          duplicate = true;
          break;
        }
        ++ it;
      }

      // insert it in the list if not a duplicate
      if (!duplicate) candidates_.insert(it, next);
    } // loop over neighbours
  }
  return cache_[index];
}

/* The cardinal of the given strata */
UnsignedInteger HyperbolicAnisotropicEnumerateFunction::inverse(const Indices & indices) const
{
  const UnsignedInteger dimension = getDimension();
  const UnsignedInteger size = indices.getSize();
  if (size != dimension) throw InvalidArgumentException(HERE)  << "Error: the size of the given indices must match the dimension, here size=" << size << " and dimension=" << dimension;
  UnsignedInteger result = 0;
  while ((result < cache_.getSize()) && (cache_[result] != indices)) ++result;
  if (result == cache_.getSize())
  {
    do
    {
      (void) operator()(result);
      ++result;
    }
    while (cache_[result - 1] != indices);
    return result - 1;
  }
  return result;
}


/* The cardinal of the given strata */
UnsignedInteger HyperbolicAnisotropicEnumerateFunction::getStrataCardinal(const UnsignedInteger strataIndex) const
{
  UnsignedInteger result = getStrataCumulatedCardinal(strataIndex);
  if (strataIndex > 0) result -= getStrataCumulatedCardinal( strataIndex - 1 );
  return result;
}


/* The cardinal of the cumulated strata above or equal to the given strata */
UnsignedInteger HyperbolicAnisotropicEnumerateFunction::getStrataCumulatedCardinal(const UnsignedInteger strataIndex) const
{
  while (strataCumulatedCardinal_.getSize() <= strataIndex) operator()(cache_.getSize());
  return strataCumulatedCardinal_[strataIndex];
}


UnsignedInteger HyperbolicAnisotropicEnumerateFunction::getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const
{
  // find indice
  UnsignedInteger index = 0;
  UnsignedInteger degree = 0;
  // First, a geometrical search to find an upper bound
  LOGINFO("Find upper bound");
  do
  {
    degree = computeDegree(operator()(index));
    ++index;
  }
  while (degree <= maximumDegree);
  UnsignedInteger strataIndex = 0;
  LOGINFO("Find strata index");
  while(getStrataCumulatedCardinal(strataIndex) < index)
  {
    ++ strataIndex;
  }
  return strataIndex - 1;
}


/* Q accessor */
void HyperbolicAnisotropicEnumerateFunction::setQ(const Scalar q)
{
  if (!(q > 0.0)) throw InvalidRangeException( HERE ) << "q parameter should be positive, but q=" << q;
  q_ = q;
  initialize();
}


Scalar HyperbolicAnisotropicEnumerateFunction::getQ() const
{
  return q_;
}


/* Weight accessor */
void HyperbolicAnisotropicEnumerateFunction::setWeight(const Point & weight)
{
  for (UnsignedInteger i = 0; i < getDimension(); ++ i)
  {
    if (!(weight[i] >= 0.0))
    {
      throw InvalidRangeException(HERE) << "Anisotropic weights should not be negative, but the weight of index " << i << " is " << weight[i];
    }
  }
  weight_ = weight;
  initialize();
}


Point HyperbolicAnisotropicEnumerateFunction::getWeight() const
{
  return weight_;
}


/* Upper bound accessor */
void HyperbolicAnisotropicEnumerateFunction::setUpperBound(const Indices & upperBound)
{
  EnumerateFunctionImplementation::setUpperBound(upperBound);
  initialize();
}


/* Method save() stores the object through the StorageManager */
void HyperbolicAnisotropicEnumerateFunction::save(Advocate & adv) const
{
  EnumerateFunctionImplementation::save(adv);
  adv.saveAttribute( "q_", q_ );
  adv.saveAttribute( "weight_", weight_ );
}


/* Method load() reloads the object from the StorageManager */
void HyperbolicAnisotropicEnumerateFunction::load(Advocate & adv)
{
  EnumerateFunctionImplementation::load(adv);
  adv.loadAttribute( "q_", q_ );
  adv.loadAttribute( "weight_", weight_ );
  initialize();
}


END_NAMESPACE_OPENTURNS
