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
#ifndef OPENTURNS_DATACONTAINER_HXX
#define OPENTURNS_DATACONTAINER_HXX

#include "openturns/PersistentObject.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Description.hxx"
#include "openturns/Collection.hxx"
#include <mutex>

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class DataContainer
 *
 * DataContainer implements unified data storage with pluggable backends.
 * It separates data storage from algebra and statistical services,
 * enabling zero-copy interoperability between different operations.
 */
class OT_API DataContainer
  : public PersistentObject
{
  CLASSNAME

public:

  typedef Collection<Scalar> ScalarCollection;

  /** Layout enum for element access */
  enum Layout
  {
    ROW_MAJOR = 0,    /** Row-major layout (Sample convention) */
    COLUMN_MAJOR = 1  /** Column-major layout (Matrix convention) */
  };

  /** Abstract storage backend interface */
  class StorageBackend
  {
  public:
    virtual ~StorageBackend() = default;
    virtual Scalar * data() = 0;
    virtual const Scalar * data() const = 0;
    virtual void resize(UnsignedInteger n) = 0;
    virtual UnsignedInteger size() const = 0;
    virtual Bool isShareable() const = 0;
    virtual Pointer<StorageBackend> clone() const = 0;
    virtual void flush() {}
  };

  /** In-memory storage backend using contiguous std::vector */
  class MemoryBackend
    : public StorageBackend
  {
  public:
    MemoryBackend();
    explicit MemoryBackend(UnsignedInteger size, Scalar value = 0.0);
    Scalar * data() override;
    const Scalar * data() const override;
    void resize(UnsignedInteger n) override;
    UnsignedInteger size() const override;
    Bool isShareable() const override;
    Pointer<StorageBackend> clone() const override;
  private:
    std::vector<Scalar> storage_;
  };

  /** Non-owning view backend over external memory */
  class ViewBackend
    : public StorageBackend
  {
  public:
    ViewBackend(Scalar * ptr, UnsignedInteger size);
    Scalar * data() override;
    const Scalar * data() const override;
    void resize(UnsignedInteger n) override;
    UnsignedInteger size() const override;
    Bool isShareable() const override;
    Pointer<StorageBackend> clone() const override;
  private:
    Scalar * ptr_;
    UnsignedInteger size_;
  };

  /** Default constructor */
  DataContainer();

  /** Constructor with size */
  explicit DataContainer(const UnsignedInteger size,
                         const Scalar value = 0.0,
                         const Layout layout = COLUMN_MAJOR);

  /** Constructor with size and dimension (2D container) */
  DataContainer(const UnsignedInteger nbRows,
                const UnsignedInteger nbColumns,
                const Scalar value,
                const Layout layout = COLUMN_MAJOR);

  /** Constructor from external collection */
  DataContainer(const UnsignedInteger size,
                const ScalarCollection & elementsValues,
                const Layout layout = COLUMN_MAJOR);

  /** Constructor from external collection with dimensions */
  DataContainer(const UnsignedInteger nbRows,
                const UnsignedInteger nbColumns,
                const ScalarCollection & elementsValues,
                const Layout layout = COLUMN_MAJOR);

  /** Constructor from external pointer (non-owning view) */
  DataContainer(Scalar * data,
                UnsignedInteger nbRows,
                UnsignedInteger nbColumns,
                const Layout layout = COLUMN_MAJOR);

#ifndef SWIG
  /** Copy constructor (deep-copies owning storage, aliases views) */
  DataContainer(const DataContainer & other);

  /** Assignment operator (deep-copies owning storage, aliases views) */
  DataContainer & operator = (const DataContainer & other);
#endif

  /** Virtual constructor */
  DataContainer * clone() const override;

  /** String converter */
  String __repr__() const override;
  String __str__(const String & offset = "") const override;

  /** Dimension accessors */
  UnsignedInteger getSize() const;
  UnsignedInteger getDimension() const;
  UnsignedInteger getNbRows() const;
  UnsignedInteger getNbColumns() const;
  Layout getLayout() const;

  /** Set dimensions (must match total size) */
  void setDimension(UnsignedInteger dimension);

  /** Element access operators */
  Scalar & operator()(UnsignedInteger i, UnsignedInteger j);
  const Scalar & operator()(UnsignedInteger i, UnsignedInteger j) const;
  Scalar & operator[](UnsignedInteger flatIndex);
  const Scalar & operator[](UnsignedInteger flatIndex) const;

  /** Bounds-checked flat element access */
  Scalar & at(UnsignedInteger flatIndex);
  const Scalar & at(UnsignedInteger flatIndex) const;

  /** Comparison operators */
  Bool operator==(const DataContainer & other) const;
  Bool operator!=(const DataContainer & other) const;

  /** Raw data access */
  inline Scalar * data()
  {
    return cachedData_;
  }
  inline const Scalar * data() const
  {
    return cachedData_;
  }


  /** Low-level access with stride */
  UnsignedInteger stride(const UnsignedInteger dim) const;

  /** Backend access */
  Pointer<StorageBackend> getBackend();

  /** Debug: raw backend size */
  UnsignedInteger backendSizeDebug() const
  {
    return backend_->size();
  }

  /** Replace the storage backend (data is copied into the new backend) */
  void setBackend(Pointer<StorageBackend> newBackend);

  /** Check if empty */
  Bool isEmpty() const;

  /** Resize */
  void resize(UnsignedInteger newSize);

  /** Erase a range of elements [firstFlat, lastFlat) from the flat buffer */
  void erase(UnsignedInteger firstFlat, UnsignedInteger lastFlat);

  /** Clear all data, resetting to empty */
  void clear();

  /** Copy-on-write support */
  void makeUnique();

  /** Check if this container is a non-owning view */
  Bool isView() const;

  /** Return a non-owning view over a contiguous row range [startRow, startRow + nrows) */
  DataContainer subView(UnsignedInteger startRow, UnsignedInteger nrows);

  /** Method save() stores the object through the StorageManager */
  void save(Advocate & adv) const override;

  /** Method load() reloads the object from the StorageManager */
  void load(Advocate & adv) override;

private:

  /** Storage dimensions */
  UnsignedInteger size_;
  UnsignedInteger dimension_;

  /** Data layout */
  Layout layout_;

  /** Pluggable storage backend */
  Pointer<StorageBackend> backend_;

  /** Cached raw pointer to the backend buffer, refreshed whenever the
   * backend or its size changes; avoids a virtual dispatch per access. */
  Scalar * cachedData_;

  /** Description of elements */
  Pointer<Description> p_description_;

  /** Position conversion function */
  inline UnsignedInteger convertPosition(UnsignedInteger i,
                                         UnsignedInteger j) const;

  /** Re-synchronize the cached data pointer with the current backend */
  inline void refreshCache()
  {
    cachedData_ = backend_->data();
  }

}; /* class DataContainer */

/** Stream operators */
OT_API OStream & operator << (OStream & OS, const DataContainer & container);
OT_API std::ostream & operator << (std::ostream & os, const DataContainer & container);

/* Inline implementations */

inline UnsignedInteger DataContainer::convertPosition(UnsignedInteger i,
                                                      UnsignedInteger j) const
{
  return (layout_ == ROW_MAJOR)
    ? j + i * dimension_    /* row-major: Sample convention */
    : i + size_ * j;        /* column-major: Matrix convention */
}

inline Scalar & DataContainer::operator()(UnsignedInteger i, UnsignedInteger j)
{
  return cachedData_[convertPosition(i, j)];
}

inline const Scalar & DataContainer::operator()(UnsignedInteger i, UnsignedInteger j) const
{
  return cachedData_[convertPosition(i, j)];
}

inline Scalar & DataContainer::operator[](UnsignedInteger flatIndex)
{
  return cachedData_[flatIndex];
}

inline const Scalar & DataContainer::operator[](UnsignedInteger flatIndex) const
{
  return cachedData_[flatIndex];
}

inline UnsignedInteger DataContainer::stride(const UnsignedInteger dim) const
{
  if (dim == 0) return (layout_ == COLUMN_MAJOR) ? 1 : dimension_;
  if (dim == 1) return (layout_ == COLUMN_MAJOR) ? size_ : 1;
  return 0;
}

inline Pointer<DataContainer::StorageBackend> DataContainer::getBackend()
{
  return backend_;
}

inline void DataContainer::setBackend(Pointer<StorageBackend> newBackend)
{
  if (newBackend.isNull()) throw InvalidArgumentException(HERE) << "Cannot set a null backend";
  // Copy current data into the new backend
  const UnsignedInteger totalSize = size_ * dimension_;
  newBackend->resize(totalSize);
  if (totalSize > 0)
    std::copy(backend_->data(), backend_->data() + totalSize, newBackend->data());
  newBackend->flush();
  backend_ = newBackend;
}

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_DATACONTAINER_HXX */
