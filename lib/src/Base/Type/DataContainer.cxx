//                                               -*- C++ -*-
/**
 *  @brief DataContainer implements unified data storage with pluggable backends
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
#include <mutex>
#include <numeric>
#include <cstring>

#include "openturns/DataContainer.hxx"
#include "openturns/PersistentObjectFactory.hxx"
#include "openturns/Exception.hxx"
#include "openturns/StorageManager.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(DataContainer)

static const Factory<DataContainer> Factory_DataContainer;

/* ==================== MemoryBackend ==================== */

DataContainer::MemoryBackend::MemoryBackend()
  : storage_()
{
  // Nothing to do
}

DataContainer::MemoryBackend::MemoryBackend(UnsignedInteger size, Scalar value)
  : storage_(size, value)
{
  // Nothing to do
}

Scalar * DataContainer::MemoryBackend::data()
{
  return storage_.data();
}

const Scalar * DataContainer::MemoryBackend::data() const
{
  return storage_.data();
}

void DataContainer::MemoryBackend::resize(UnsignedInteger n)
{
  storage_.resize(n, 0.0);
}

UnsignedInteger DataContainer::MemoryBackend::size() const
{
  return static_cast<UnsignedInteger>(storage_.size());
}

Bool DataContainer::MemoryBackend::isShareable() const
{
  return true;
}

Pointer<DataContainer::StorageBackend> DataContainer::MemoryBackend::clone() const
{
  Pointer<MemoryBackend> result(new MemoryBackend());
  result->storage_ = storage_;
  return result;
}

/* ==================== ViewBackend ==================== */

DataContainer::ViewBackend::ViewBackend(Scalar * ptr, UnsignedInteger size)
  : ptr_(ptr)
  , size_(size)
{
  // Nothing to do
}

Scalar * DataContainer::ViewBackend::data()
{
  return ptr_;
}

const Scalar * DataContainer::ViewBackend::data() const
{
  return ptr_;
}

void DataContainer::ViewBackend::resize(UnsignedInteger /*n*/)
{
  throw InternalException(HERE) << "Cannot resize a ViewBackend";
}

UnsignedInteger DataContainer::ViewBackend::size() const
{
  return size_;
}

Bool DataContainer::ViewBackend::isShareable() const
{
  return false;
}

Pointer<DataContainer::StorageBackend> DataContainer::ViewBackend::clone() const
{
  // Materialize: copy view data into a new MemoryBackend
  Pointer<MemoryBackend> result(new MemoryBackend(size_));
  if (size_ > 0)
    std::copy(ptr_, ptr_ + size_, result->data());
  return result;
}

/* ==================== DataContainer ==================== */

/* Default constructor */
DataContainer::DataContainer()
  : PersistentObject()
    , size_(0)
    , dimension_(0)
    , layout_(COLUMN_MAJOR)
    , backend_(new MemoryBackend())
    , cachedData_(nullptr)
    , p_description_(new Description())
{
  refreshCache();
  // Nothing to do
}

/* Constructor with size */
DataContainer::DataContainer(const UnsignedInteger size,
                             const Scalar value,
                             const Layout layout)
  : PersistentObject()
    , size_(size)
    , dimension_(1)
    , layout_(layout)
    , backend_(new MemoryBackend(size, value))
    , cachedData_(nullptr)
    , p_description_(new Description())
{
  refreshCache();
  // Nothing to do
}

/* Constructor with size and dimension (2D container) */
DataContainer::DataContainer(const UnsignedInteger nbRows,
                             const UnsignedInteger nbColumns,
                             const Scalar value,
                             const Layout layout)
  : PersistentObject()
    , size_(nbRows)
    , dimension_(nbColumns)
    , layout_(layout)
    , backend_(new MemoryBackend(nbRows * nbColumns, value))
    , cachedData_(nullptr)
    , p_description_(new Description())
{
  refreshCache();
  // Nothing to do
}

/* Constructor from external collection */
DataContainer::DataContainer(const UnsignedInteger size,
                             const ScalarCollection & elementsValues,
                             const Layout layout)
  : PersistentObject()
    , size_(size)
    , dimension_(1)
    , layout_(layout)
    , p_description_(new Description())
{
  const UnsignedInteger totalSize = std::min(size, elementsValues.getSize());
  Pointer<MemoryBackend> memBackend(new MemoryBackend(totalSize));
  std::copy(elementsValues.begin(), elementsValues.begin() + totalSize, memBackend->data());
  if (totalSize < size)
    memBackend->resize(size);
  backend_ = memBackend;
  refreshCache();
}

/* Constructor from external collection with dimensions */
DataContainer::DataContainer(const UnsignedInteger nbRows,
                             const UnsignedInteger nbColumns,
                             const ScalarCollection & elementsValues,
                             const Layout layout)
  : PersistentObject()
    , size_(nbRows)
    , dimension_(nbColumns)
    , layout_(layout)
    , p_description_(new Description())
{
  const UnsignedInteger totalSize = nbRows * nbColumns;
  Pointer<MemoryBackend> memBackend(new MemoryBackend(totalSize));
  const UnsignedInteger copySize = std::min(totalSize, elementsValues.getSize());
  std::copy(elementsValues.begin(), elementsValues.begin() + copySize, memBackend->data());
  if (copySize < totalSize)
    std::fill(memBackend->data() + copySize, memBackend->data() + totalSize, 0.0);
  backend_ = memBackend;
  refreshCache();
}

#ifndef SWIG
/* Copy constructor: owning storage is deep-copied so that each container
 * owns its buffer (historical value semantics); views keep aliasing their
 * external memory on purpose. */
DataContainer::DataContainer(const DataContainer & other)
  : PersistentObject(other)
  , size_(other.size_)
  , dimension_(other.dimension_)
  , layout_(other.layout_)
  , backend_(other.backend_)
    , cachedData_(nullptr)
  , p_description_(other.p_description_)
{
  refreshCache();
  if (!other.isView())
    backend_ = backend_->clone();
  refreshCache();
}

/* Assignment operator */
DataContainer & DataContainer::operator = (const DataContainer & other)
{
  if (this != &other)
  {
    PersistentObject::operator = (other);
    size_ = other.size_;
    dimension_ = other.dimension_;
    layout_ = other.layout_;
    p_description_ = other.p_description_;
    backend_ = other.backend_;
    if (!other.isView())
      backend_ = backend_->clone();
    refreshCache();
  }
  return *this;
}
#endif

/* Virtual constructor */
DataContainer * DataContainer::clone() const
{
  return new DataContainer(*this);
}

/* Constructor from external pointer (non-owning view) */
DataContainer::DataContainer(Scalar * data,
                             UnsignedInteger nbRows,
                             UnsignedInteger nbColumns,
                             const Layout layout)
  : PersistentObject()
    , size_(nbRows)
    , dimension_(nbColumns)
    , layout_(layout)
    , backend_(new ViewBackend(data, nbRows * nbColumns))
    , cachedData_(nullptr)
    , p_description_(new Description())
{
  refreshCache();
  // Nothing to do
}

/* String converter */
String DataContainer::__repr__() const
{
  return OSS(true) << "class=" << GetClassName()
         << " name=" << getName()
         << " size=" << size_
         << " dimension=" << dimension_
         << " layout=" << (layout_ == ROW_MAJOR ? "ROW_MAJOR" : "COLUMN_MAJOR");
}

String DataContainer::__str__(const String & /*offset*/) const
{
  return __repr__();
}

/* Dimension accessors */
UnsignedInteger DataContainer::getSize() const
{
  return size_;
}

UnsignedInteger DataContainer::getDimension() const
{
  return dimension_;
}

UnsignedInteger DataContainer::getNbRows() const
{
  return (layout_ == COLUMN_MAJOR) ? size_ : dimension_;
}

UnsignedInteger DataContainer::getNbColumns() const
{
  return (layout_ == COLUMN_MAJOR) ? dimension_ : size_;
}

DataContainer::Layout DataContainer::getLayout() const
{
  return layout_;
}

/* Set dimension */
void DataContainer::setDimension(UnsignedInteger dimension)
{
  dimension_ = dimension;
}

/* Check if empty */
Bool DataContainer::isEmpty() const
{
  return (size_ == 0) || (dimension_ == 0);
}

Bool DataContainer::operator==(const DataContainer & other) const
{
  if (size_ != other.size_ || dimension_ != other.dimension_) return false;
  const UnsignedInteger n = size_ * dimension_;
  for (UnsignedInteger i = 0; i < n; ++i)
  {
    if ((*this)[i] != other[i]) return false;
  }
  return true;
}

Bool DataContainer::operator!=(const DataContainer & other) const
{
  return !(*this == other);
}

/* Resize */
void DataContainer::resize(UnsignedInteger newSize)
{
  /* A null dimension denotes a flat 1D storage */
  if (dimension_ == 0) dimension_ = 1;
  const UnsignedInteger totalSize = newSize * dimension_;
  size_ = newSize;
  backend_->resize(totalSize);
  refreshCache();
}

/* Bounds-checked flat element access */
Scalar & DataContainer::at(UnsignedInteger flatIndex)
{
  const UnsignedInteger totalSize = size_ * dimension_;
  if (flatIndex >= totalSize)
    throw OutOfBoundException(HERE) << "DataContainer index " << flatIndex << " exceeds total size " << totalSize;
  return cachedData_[flatIndex];
}

const Scalar & DataContainer::at(UnsignedInteger flatIndex) const
{
  const UnsignedInteger totalSize = size_ * dimension_;
  if (flatIndex >= totalSize)
    throw OutOfBoundException(HERE) << "DataContainer index " << flatIndex << " exceeds total size " << totalSize;
  return cachedData_[flatIndex];
}

/* Erase a range of elements [firstFlat, lastFlat) from the flat buffer */
void DataContainer::erase(UnsignedInteger firstFlat, UnsignedInteger lastFlat)
{
  if (firstFlat >= lastFlat) return;
  const UnsignedInteger totalSize = size_ * dimension_;
  if (lastFlat > totalSize)
    throw InvalidArgumentException(HERE) << "erase range [" << firstFlat << ", " << lastFlat << ") exceeds total size " << totalSize;
  if (isView())
    throw InternalException(HERE) << "Cannot erase elements from a non-owning DataContainer view";
  makeUnique();
  Scalar * d = data();
  const UnsignedInteger count = lastFlat - firstFlat;
  if (lastFlat < totalSize)
    std::memmove(d + firstFlat, d + lastFlat, (totalSize - lastFlat) * sizeof(Scalar));
  const UnsignedInteger newTotal = totalSize - count;
  size_ = (dimension_ > 0) ? newTotal / dimension_ : 0;
  backend_->resize(newTotal);
  refreshCache();
}

/* Clear all data, resetting to empty */
void DataContainer::clear()
{
  size_ = 0;
  dimension_ = 1;
  backend_ = new MemoryBackend();
  refreshCache();
}

/* Copy-on-write support */
void DataContainer::makeUnique()
{
  // Views are never COW-materialized: the caller owns the underlying memory.
  if (isView()) return;
  // Copies of this container may live in different threads; serializes the
  // materialization so that concurrent first writes cannot race on backend_.
  static std::mutex mutex;
  std::lock_guard<std::mutex> guard(mutex);
  if (!backend_->isShareable() || backend_.unique()) return;
  Pointer<StorageBackend> newBackend = backend_->clone();
  backend_.swap(newBackend);
  refreshCache();
}

/* Check if this container is a non-owning view */
Bool DataContainer::isView() const
{
  return !backend_->isShareable();
}

/* Return a non-owning view over a contiguous row range [startRow, startRow + nrows) */
DataContainer DataContainer::subView(UnsignedInteger startRow, UnsignedInteger nrows)
{
  if (startRow + nrows > size_)
    throw InvalidArgumentException(HERE) << "subView range [" << startRow << ", " << startRow + nrows << ") exceeds size " << size_;
  if (dimension_ == 0)
    throw InvalidArgumentException(HERE) << "Cannot create subView of container with dimension 0";
  // Compute the byte offset for row-major layout: row i starts at i * dimension_
  const UnsignedInteger offset = startRow * dimension_;
  return DataContainer(data() + offset, nrows, dimension_, layout_);
}

/* Method save() stores the object through the StorageManager */
void DataContainer::save(Advocate & adv) const
{
  if (isView())
    throw InternalException(HERE) << "Cannot serialize a non-owning DataContainer view";
  PersistentObject::save(adv);
  adv.saveAttribute("size", size_);
  adv.saveAttribute("dimension", dimension_);
  adv.saveAttribute("layout", static_cast<UnsignedInteger>(layout_));
  // Save data elements using AdvocateIterator, same pattern as PersistentCollection
  const UnsignedInteger totalSize = size_ * dimension_;
  AdvocateIterator<Scalar> adv_it(adv);
  const Scalar * pData = cachedData_;
  for (UnsignedInteger i = 0; i < totalSize; ++i, ++adv_it)
  {
    *adv_it = pData[i];
  }
}

/* Method load() reloads the object from the StorageManager */
void DataContainer::load(Advocate & adv)
{
  PersistentObject::load(adv);
  adv.loadAttribute("size", size_);
  adv.loadAttribute("dimension", dimension_);
  UnsignedInteger layoutInt = 0;
  adv.loadAttribute("layout", layoutInt);
  layout_ = static_cast<Layout>(layoutInt);
  const UnsignedInteger totalSize = size_ * dimension_;
  Pointer<MemoryBackend> memBackend(new MemoryBackend(totalSize));
  std::generate(memBackend->data(), memBackend->data() + totalSize, AdvocateIterator<Scalar>(adv));
  backend_ = memBackend;
  refreshCache();
}

/* Stream operators */
OStream & operator << (OStream & OS, const DataContainer & container)
{
  return OS << container.__repr__();
}

std::ostream & operator << (std::ostream & os, const DataContainer & container)
{
  return os << container.__str__();
}

END_NAMESPACE_OPENTURNS
