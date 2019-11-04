//                                               -*- C++ -*-
/**
 *  @brief IndicesCollectionImplementation stores a Collection of Indices contiguously.
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/IndicesCollectionImplementation.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"

#include <algorithm>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(IndicesCollectionImplementation)


static const Factory<IndicesCollectionImplementation> Factory_IndicesCollectionImplementation;

/* Default constructor */
IndicesCollectionImplementation::IndicesCollectionImplementation()
  : PersistentObject()
  , size_(0)
  , values_(0)
  , offsets_(1)
{
  // Nothing to do
}

/* Constructor from size and stride */
IndicesCollectionImplementation::IndicesCollectionImplementation(const UnsignedInteger size,
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
IndicesCollectionImplementation::IndicesCollectionImplementation(const UnsignedInteger size,
    const UnsignedInteger stride, const Indices & values)
  : PersistentObject()
  , size_(size)
  , values_(values)
  , offsets_(size + 1)
{
  if (values.getSize() != size * stride) throw InvalidArgumentException(HERE);
  for(UnsignedInteger i = 0; i <= size; ++i)
    offsets_[i] = i * stride;
}

/* Constructor from external Collection<Indices> */
IndicesCollectionImplementation::IndicesCollectionImplementation(const Collection<Indices> & values)
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
IndicesCollectionImplementation * IndicesCollectionImplementation::clone() const
{
  return new IndicesCollectionImplementation(*this);
}

/* String converter */
String IndicesCollectionImplementation::toString(Bool full) const
{
  OSS oss(full);
  oss << "[";
  Bool firstLine = true;
  for (UnsignedInteger i = 0; i < size_; ++i)
  {
    if (!firstLine) oss << ",";
    firstLine = false;

    // This method may be used by derived classes, so only use virtual methods
    oss << "[";
    std::copy( this->cbegin_at(i), this->cend_at(i), OSS_iterator<UnsignedInteger>(oss, ",") );
    oss << "]";
    const UnsignedInteger indicesSize = this->cend_at(i) - this->cbegin_at(i);
    if (!full && indicesSize >= ResourceMap::GetAsUnsignedInteger("Collection-size-visible-in-str-from"))
      oss << "#" << getSize();
  }
  oss << "]";
  return oss;
}

String IndicesCollectionImplementation::__repr__() const
{
  return toString(true);
}

String IndicesCollectionImplementation::__str__(const String & ) const
{
  OSS oss;
  oss << toString(false);
  if (size_ >= ResourceMap::GetAsUnsignedInteger("Collection-size-visible-in-str-from"))
    oss << "#" << size_;
  return oss;
}

/* Get the number of Indices */
UnsignedInteger IndicesCollectionImplementation::getSize() const
{
  return size_;
}

IndicesCollectionImplementation::iterator IndicesCollectionImplementation::begin_at(const UnsignedInteger index)
{
  if (index >= size_) throw OutOfBoundException(HERE) << "index=" << index << " too large, size=" << size_;
  return values_.begin() + offsets_[index];
}

IndicesCollectionImplementation::iterator IndicesCollectionImplementation::end_at(const UnsignedInteger index)
{
  if (index >= size_) throw OutOfBoundException(HERE) << "index=" << index << " too large, size=" << size_;
  return values_.begin() + offsets_[index + 1];
}

IndicesCollectionImplementation::const_iterator IndicesCollectionImplementation::cbegin_at(const UnsignedInteger index) const
{
  if (index >= size_) throw OutOfBoundException(HERE) << "index=" << index << " too large, size=" << size_;
  return values_.begin() + offsets_[index];
}

IndicesCollectionImplementation::const_iterator IndicesCollectionImplementation::cend_at(const UnsignedInteger index) const
{
  if (index >= size_) throw OutOfBoundException(HERE) << "index=" << index << " too large, size=" << size_;
  return values_.begin() + offsets_[index + 1];
}

// Accessors to values_[index][pos]
const UnsignedInteger & IndicesCollectionImplementation::operator()(const UnsignedInteger index, const UnsignedInteger pos) const
{
  return values_[offsets_[index] + pos];
}

UnsignedInteger & IndicesCollectionImplementation::operator()(const UnsignedInteger index, const UnsignedInteger pos)
{
  return values_[offsets_[index] + pos];
}

Bool IndicesCollectionImplementation::operator ==(const IndicesCollectionImplementation & rhs) const
{
  if (this == &rhs) return true;
  return size_ == rhs.size_ && offsets_ == rhs.offsets_ && values_ == rhs.values_;
}

/* Method save() stores the object through the StorageManager */
void IndicesCollectionImplementation::save(Advocate & adv) const
{
  PersistentObject::save(adv);
  adv.saveAttribute( "size_", size_);
  adv.saveAttribute( "values_", values_);
  adv.saveAttribute( "offsets_", offsets_);
}

/* Method load() reloads the object from the StorageManager */
void IndicesCollectionImplementation::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute( "size_", size_);
  adv.loadAttribute( "values_", values_);
  adv.loadAttribute( "offsets_", offsets_);
}


END_NAMESPACE_OPENTURNS
