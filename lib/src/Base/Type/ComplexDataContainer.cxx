//                                               -*- C++ -*-
/**
 *  @brief ComplexDataContainer implements unified complex data storage with pluggable backends
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
#include <algorithm>
#include "openturns/ComplexDataContainer.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ComplexDataContainer)

static const Factory<ComplexDataContainer> Factory_ComplexDataContainer;

/* Default constructor */
ComplexDataContainer::MemoryBackend::MemoryBackend()
  : storage_()
{
  // Nothing to do
}

/* Constructor with size */
ComplexDataContainer::MemoryBackend::MemoryBackend(UnsignedInteger size)
  : storage_(size)
{
  // Nothing to do
}

Complex * ComplexDataContainer::MemoryBackend::data()
{
  return storage_.data();
}

const Complex * ComplexDataContainer::MemoryBackend::data() const
{
  return storage_.data();
}

void ComplexDataContainer::MemoryBackend::resize(UnsignedInteger n)
{
  storage_.resize(n);
}

UnsignedInteger ComplexDataContainer::MemoryBackend::size() const
{
  return static_cast<UnsignedInteger>(storage_.size());
}

Bool ComplexDataContainer::MemoryBackend::isShareable() const
{
  return true;
}

Pointer<ComplexDataContainer::StorageBackend> ComplexDataContainer::MemoryBackend::clone() const
{
  Pointer<MemoryBackend> result(new MemoryBackend());
  result->storage_ = storage_;
  return result;
}

/* Default constructor */
ComplexDataContainer::ComplexDataContainer()
  : PersistentObject()
  , backend_(new MemoryBackend())
  , cachedData_(nullptr)
{
  refreshCache();
}

/* Constructor with size */
ComplexDataContainer::ComplexDataContainer(UnsignedInteger size)
  : PersistentObject()
  , backend_(new MemoryBackend(size))
  , cachedData_(nullptr)
{
  refreshCache();
}

#ifndef SWIG
/* Copy constructor (deep-copies the storage) */
ComplexDataContainer::ComplexDataContainer(const ComplexDataContainer & other)
  : PersistentObject(other)
  , backend_(other.backend_->clone())
  , cachedData_(nullptr)
{
  refreshCache();
}

/* Assignment operator (deep-copies the storage) */
ComplexDataContainer & ComplexDataContainer::operator = (const ComplexDataContainer & other)
{
  if (this != &other)
  {
    PersistentObject::operator = (other);
    backend_ = other.backend_->clone();
    refreshCache();
  }
  return *this;
}
#endif

/* Virtual constructor */
ComplexDataContainer * ComplexDataContainer::clone() const
{
  return new ComplexDataContainer(*this);
}

/* String converter */
String ComplexDataContainer::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " size=" << getSize();
}

/* Number of elements */
UnsignedInteger ComplexDataContainer::getSize() const
{
  return backend_->size();
}

/* Resize */
void ComplexDataContainer::resize(UnsignedInteger newSize)
{
  backend_->resize(newSize);
  refreshCache();
}

/* Backend access */
Pointer<ComplexDataContainer::StorageBackend> ComplexDataContainer::getBackend()
{
  return backend_;
}

/* Replace the storage backend (data is copied into the new backend) */
void ComplexDataContainer::setBackend(Pointer<StorageBackend> newBackend)
{
  if (newBackend.isNull()) throw InvalidArgumentException(HERE) << "Cannot set a null backend";
  const UnsignedInteger totalSize = getSize();
  newBackend->resize(totalSize);
  if (totalSize > 0)
    std::copy(cachedData_, cachedData_ + totalSize, newBackend->data());
  backend_ = newBackend;
  refreshCache();
}

END_NAMESPACE_OPENTURNS
