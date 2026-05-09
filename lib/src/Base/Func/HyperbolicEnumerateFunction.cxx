//                                               -*- C++ -*-
/**
 * @brief The bijective function to select polynomials in the orthogonal basis
 *
 * Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <algorithm>
#include <cmath>
#include "openturns/EnumerateFunction.hxx"
#include "openturns/HyperbolicEnumerateFunction.hxx"
#include "openturns/OSS.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(HyperbolicEnumerateFunction)

static const Factory<HyperbolicEnumerateFunction> Factory_HyperbolicEnumerateFunction;

/* Default constructor */
HyperbolicEnumerateFunction::HyperbolicEnumerateFunction()
  : EnumerateFunctionImplementation()
  , q_(0.0)
{
  // Nothing to do
}

/* Parameter constructor */
HyperbolicEnumerateFunction::HyperbolicEnumerateFunction(const UnsignedInteger dimension,
    const Scalar q)
  : EnumerateFunctionImplementation(dimension)
{
  setQ(q);
  initialize();
}

/* Virtual constructor */
HyperbolicEnumerateFunction * HyperbolicEnumerateFunction::clone() const
{
  return new HyperbolicEnumerateFunction(*this);
}

/* String converter */
String HyperbolicEnumerateFunction::__repr__() const
{
  return OSS() << "class=" << getClassName()
         << " derived from " << EnumerateFunctionImplementation::__repr__()
         << " q=" << q_;
}

void HyperbolicEnumerateFunction::initialize()
{
  cache_.clear();
  candidates_.clear();
  // Insertion de l'indice 0, avec une norme q de 0.0 dans la liste des candidats
  ValueType zero(Indices(getDimension(), 0), 0.0);
  candidates_.insert(candidates_.begin(), zero);
}

/* Returns the q-norm of the multi-index */
Scalar HyperbolicEnumerateFunction::qNorm(const Indices & indices) const
{
  Scalar result = 0.0;
  UnsignedInteger dimension = indices.getSize();
  if (q_ == 1.0)
  {
    for (UnsignedInteger j = 0; j < dimension; ++ j)
      result += indices[j];
    return result;
  }
  for (UnsignedInteger j = 0; j < dimension; ++ j)
    result += std::pow(static_cast<Scalar>(indices[j]), q_);
  return std::pow(result, 1.0 / q_);
}

UnsignedInteger HyperbolicEnumerateFunction::computeDegree(const Indices& indices) const
{
  return *std::max_element(indices.begin(), indices.end());
}

Indices HyperbolicEnumerateFunction::operator() (const UnsignedInteger index) const
{
  for (UnsignedInteger i = cache_.getSize(); i <= index; ++ i)
  {
    if (candidates_.empty())
      throw NotDefinedException(HERE) << "Cannot enumerate up to index=" << index << " because of the bounds.";

    ValueType current(candidates_.front());
    candidates_.pop_front();

    if ((cache_.getSize() > 0) && (current.second > qNorm(cache_[cache_.getSize() - 1]))) 
      strataCumulatedCardinal_.add( cache_.getSize() );

    cache_.add(current.first);

    // Generate all neighbor indices
    for(UnsignedInteger j = 0; j < getDimension(); ++ j)
    {
      Indices nextIndices(current.first);
      if (nextIndices[j] >= upperBound_[j])
        continue;
      ++ nextIndices[j];

      Scalar nextNorm = qNorm(nextIndices);
      ValueType next(nextIndices, nextNorm);
      IndiceCache::iterator it = candidates_.begin();

      // Maintain candidates list sorted by q-norm
      while ((it != candidates_.end()) && (it->second < nextNorm)) ++ it;

      // Check for duplicates
      Bool duplicate = false;
      while ((it != candidates_.end()) && (it->second == nextNorm))
      {
        if (it->first == nextIndices)
        {
          duplicate = true;
          break;
        }
        ++ it;
      }

      if (!duplicate) candidates_.insert(it, next);
    } 
  }
  return cache_[index];
}

UnsignedInteger HyperbolicEnumerateFunction::inverse(const Indices & indices) const
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

UnsignedInteger HyperbolicEnumerateFunction::getStrataCardinal(const UnsignedInteger strataIndex) const
{
  UnsignedInteger result = getStrataCumulatedCardinal(strataIndex);
  if (strataIndex > 0) result -= getStrataCumulatedCardinal( strataIndex - 1 );
  return result;
}

UnsignedInteger HyperbolicEnumerateFunction::getStrataCumulatedCardinal(const UnsignedInteger strataIndex) const
{
  while (strataCumulatedCardinal_.getSize() <= strataIndex) operator()(cache_.getSize());
  return strataCumulatedCardinal_[strataIndex];
}

UnsignedInteger HyperbolicEnumerateFunction::getMaximumDegreeStrataIndex(const UnsignedInteger maximumDegree) const
{
  UnsignedInteger index = 0;
  UnsignedInteger degree = 0;
  
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
void HyperbolicEnumerateFunction::setQ(const Scalar q)
{
  if (!(q > 0.0)) throw InvalidRangeException( HERE ) << "q parameter should be positive, but q=" << q;
  q_ = q;
  initialize();
}

Scalar HyperbolicEnumerateFunction::getQ() const
{
  return q_;
}

/* Upper bound accessor */
void HyperbolicEnumerateFunction::setUpperBound(const Indices & upperBound)
{
  EnumerateFunctionImplementation::setUpperBound(upperBound);
  initialize();
}

/* The marginal enumeration function */
EnumerateFunction HyperbolicEnumerateFunction::getMarginal(const Indices & indices) const
{
  const UnsignedInteger inputDimension = getDimension();
  if (!indices.check(inputDimension))
  {
    throw InvalidArgumentException(HERE) << "Indices " << indices << "must not exceed dimension " << inputDimension;
  }
  const UnsignedInteger activeDimension = indices.getSize();
  const HyperbolicEnumerateFunction enumerateFunctionMarginal(activeDimension, q_);
  return enumerateFunctionMarginal;
}

/* Method save() stores the object through the StorageManager */
void HyperbolicEnumerateFunction::save(Advocate & adv) const
{
  EnumerateFunctionImplementation::save(adv);
  adv.saveAttribute( "q_", q_ );
}

/* Method load() reloads the object from the StorageManager */
void HyperbolicEnumerateFunction::load(Advocate & adv)
{
  EnumerateFunctionImplementation::load(adv);
  adv.loadAttribute( "q_", q_ );
  initialize();
}

END_NAMESPACE_OPENTURNS