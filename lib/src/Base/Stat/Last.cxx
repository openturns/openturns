//                                               -*- C++ -*-
/**
 *  @brief
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
#include <cstdlib>

#include "openturns/Last.hxx"
#include "openturns/SampleImplementation.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS



/*
 * @class Last
 */

CLASSNAMEINIT(Last)
static const Factory<Last> Factory_Last;

/* Default constructor */
Last::Last()
  : HistoryStrategyImplementation(),
    maximumSize_(ResourceMap::GetAsUnsignedInteger( "Last-DefaultMaximumSize" )),
    index_(0),
    hasWrapped_(false)
{
  // Nothing to do
}

/* Constructor with parameters */
Last::Last(const UnsignedInteger maximumSize)
  : HistoryStrategyImplementation(),
    maximumSize_(maximumSize),
    index_(0),
    hasWrapped_(false)
{
  // Nothing to do
}

/*Virtual constructor */
Last * Last::clone() const
{
  return new Last(*this);
}

/*Store the point according to the strategy */
void Last::store(const Point & point)
{
  sample_.at(index_) = point;
  ++index_;
  if (index_ == maximumSize_)
  {
    index_ = 0;
    hasWrapped_ = true;
  }
}

/* Clear the history storage and change dimension of Point stored */
void Last::setDimension(const UnsignedInteger dimension)
{
  sample_ = Sample(maximumSize_, dimension);
  index_ = 0;
  hasWrapped_ = false;
}

/*Sample accessor */
Sample Last::getSample() const
{
  // If we don't have exhausted the maximum size, return just the stored data
  if (!hasWrapped_)
  {
    Sample outSample(index_, sample_.getDimension());
    for (UnsignedInteger i = 0; i < index_; ++i) outSample[i] = sample_[i];
    return outSample;
  }
  // Perform a permutation of the elements:
  Sample outSample(maximumSize_, sample_.getDimension());
  for (UnsignedInteger i = 0; i < maximumSize_; ++i)
  {
    UnsignedInteger circularIndex = (index_ + i) % maximumSize_;
    outSample[i] = sample_[circularIndex];
  }
  return outSample;
}

/*MaximumSize accessor */
UnsignedInteger Last::getMaximumSize() const
{
  return maximumSize_;
}

/*Index accessor */
UnsignedInteger Last::getIndex() const
{
  return index_;
}

/*Wrapped flag accessor */
Bool Last::getHasWrapped() const
{
  return hasWrapped_;
}

/*String converter */
String Last::__repr__() const
{
  OSS oss;
  oss << "class=" << Last::GetClassName();
  oss << " sample_= " << sample_;
  oss << " index_= " << index_;
  oss << " hasWrapped_= " << hasWrapped_;
  return oss;
}

/* Method save() stores the object through the StorageManager */
void Last::save(Advocate & adv) const
{
  HistoryStrategyImplementation::save(adv);
  adv.saveAttribute("maximumSize_", maximumSize_);
  adv.saveAttribute("index_", index_);
  adv.saveAttribute("hasWrapped_", hasWrapped_);
}

/* Method load() reloads the object from the StorageManager */
void Last::load(Advocate & adv)
{
  HistoryStrategyImplementation::load(adv);
  adv.loadAttribute("maximumSize_", maximumSize_);
  adv.loadAttribute("index_", index_);
  adv.loadAttribute("hasWrapped_", hasWrapped_);
}

END_NAMESPACE_OPENTURNS
