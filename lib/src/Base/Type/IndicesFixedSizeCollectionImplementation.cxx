//                                               -*- C++ -*-
/**
 *  @brief IndicesFixedSizeCollectionImplementation stores a Collection of Indices contiguously.
 *
 *  Copyright 2005-2018 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "openturns/IndicesFixedSizeCollectionImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

#include <algorithm>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicesFixedSizeCollectionImplementation)


static const Factory<IndicesFixedSizeCollectionImplementation> Factory_IndicesFixedSizeCollectionImplementation;

/* Default constructor */
IndicesFixedSizeCollectionImplementation::IndicesFixedSizeCollectionImplementation()
  : PersistentObject()
  , size_(0)
  , values_(0)
  , offsets_(1)
{
  // Nothing to do
}

/* Constructor from size and stride */
IndicesFixedSizeCollectionImplementation::IndicesFixedSizeCollectionImplementation(const UnsignedInteger size,
   const UnsignedInteger stride)
  : PersistentObject()
  , size_(size)
  , values_(size * stride)
  , offsets_(size + 1)
{
  for(UnsignedInteger i = 0; i <= size; ++i)
    offsets_[i] = i * stride;
}

/* Constructor from size, stride and values */
IndicesFixedSizeCollectionImplementation::IndicesFixedSizeCollectionImplementation(const UnsignedInteger size,
   const UnsignedInteger stride, const Indices & values)
  : PersistentObject()
  , size_(size)
  , values_(values)
  , offsets_(size + 1)
{
  if (values.getSize() != size * stride) throw InvalidArgumentException(HERE) << "collection size is " << values.getSize() << " instead of " << (size * stride);
  for(UnsignedInteger i = 0; i <= size; ++i)
    offsets_[i] = i * stride;
}

/* Constructor from external Collection<Indices> */
IndicesFixedSizeCollectionImplementation::IndicesFixedSizeCollectionImplementation(const Collection<Indices> & values)
  : PersistentObject()
  , size_(values.getSize())
  , values_()
  , offsets_(values.getSize() + 1)
{
  offsets_[0] = 0;
  if (values.getSize() == 0) return;

  // Compute position of the first element of each Indices
  Indices::iterator offsetIterator = offsets_.begin();
  for(Collection<Indices>::const_iterator it = values.begin(), guard = values.end(); it != guard; ++it, ++offsetIterator)
    *(offsetIterator + 1) = (*offsetIterator) + it->getSize();

  values_.resize(offsets_[size_]);
  Indices::iterator flatIterator = values_.begin();
  for(Collection<Indices>::const_iterator it = values.begin(), guard = values.end(); it != guard; ++it)
    flatIterator = std::copy(it->begin(), it->end(), flatIterator);
}

/* Virtual constructor */
IndicesFixedSizeCollectionImplementation * IndicesFixedSizeCollectionImplementation::clone() const
{
  return new IndicesFixedSizeCollectionImplementation(*this);
}


/* String converter */
String IndicesFixedSizeCollectionImplementation::__repr__() const
{
  return OSS(true) << "class=" << getClassName()
         << " name=" << getName()
         << " size=" << size_
         << " offsets=" << offsets_
         << " values=" << values_;
}

String IndicesFixedSizeCollectionImplementation::__str__(const String & offset) const
{
  OSS oss(false);
  Bool firstLine = true;
  for (UnsignedInteger i = 0; i < size_; ++ i)
  {
    if (!firstLine) oss << "\n";
    firstLine = false;

    oss << i << " : [";
    const char * sep = "";
    // This method may be used by derived classes, so only use virtual methods
    const UnsignedInteger indicesSize = this->cend_at(i) - this->cbegin_at(i);
    for(UnsignedInteger j = 0; j < indicesSize; ++j, sep=" ")
    {
      oss << sep << operator()(i, j);
    }
    oss << "]";
  }
  return oss;
}

/* Get the number of Indices */
UnsignedInteger IndicesFixedSizeCollectionImplementation::getSize() const
{
  return size_;
}

IndicesFixedSizeCollectionImplementation::iterator IndicesFixedSizeCollectionImplementation::begin_at(const UnsignedInteger index)
{
  if (index >= size_) throw OutOfBoundException(HERE) << "index=" << index << " too large, size=" << size_;
  return values_.begin() + offsets_[index];
}

IndicesFixedSizeCollectionImplementation::iterator IndicesFixedSizeCollectionImplementation::end_at(const UnsignedInteger index)
{
  if (index >= size_) throw OutOfBoundException(HERE) << "index=" << index << " too large, size=" << size_;
  return values_.begin() + offsets_[index + 1];
}

IndicesFixedSizeCollectionImplementation::const_iterator IndicesFixedSizeCollectionImplementation::cbegin_at(const UnsignedInteger index) const
{
  if (index >= size_) throw OutOfBoundException(HERE) << "index=" << index << " too large, size=" << size_;
  return values_.begin() + offsets_[index];
}

IndicesFixedSizeCollectionImplementation::const_iterator IndicesFixedSizeCollectionImplementation::cend_at(const UnsignedInteger index) const
{
  if (index >= size_) throw OutOfBoundException(HERE) << "index=" << index << " too large, size=" << size_;
  return values_.begin() + offsets_[index + 1];
}

// Accessors to values_[index][pos]
const UnsignedInteger & IndicesFixedSizeCollectionImplementation::operator()(const UnsignedInteger index, const UnsignedInteger pos) const
{
  return values_[offsets_[index] + pos];
}

UnsignedInteger & IndicesFixedSizeCollectionImplementation::operator()(const UnsignedInteger index, const UnsignedInteger pos)
{
  return values_[offsets_[index] + pos];
}

Bool IndicesFixedSizeCollectionImplementation::operator ==(const IndicesFixedSizeCollectionImplementation & rhs) const
{
  if (this == &rhs) return true;
  return size_ == rhs.size_ && offsets_ == rhs.offsets_ && values_ == rhs.values_;
}

/* Method save() stores the object through the StorageManager */
void IndicesFixedSizeCollectionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "values_", values_);
  adv.saveAttribute( "offsets_", offsets_);
}

/* Method load() reloads the object from the StorageManager */
void IndicesFixedSizeCollectionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "values_", values_);
  adv.loadAttribute( "offsets_", offsets_);
}


END_NAMESPACE_OPENTURNS
