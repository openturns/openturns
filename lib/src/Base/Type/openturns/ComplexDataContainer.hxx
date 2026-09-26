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
#ifndef OPENTURNS_COMPLEXDATACONTAINER_HXX
#define OPENTURNS_COMPLEXDATACONTAINER_HXX

#include <vector>
#include "openturns/PersistentObject.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Collection.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComplexDataContainer
 *
 * Complex-valued counterpart of DataContainer: unified complex data storage
 * with pluggable memory backends.
 */
class OT_API ComplexDataContainer
  : public PersistentObject
{
  CLASSNAME

public:

  /** Abstract storage backend interface */
  class StorageBackend
  {
  public:
    virtual ~StorageBackend() = default;
    virtual Complex * data() = 0;
    virtual const Complex * data() const = 0;
    virtual void resize(UnsignedInteger n) = 0;
    virtual UnsignedInteger size() const = 0;
    virtual Bool isShareable() const = 0;
    virtual Pointer<StorageBackend> clone() const = 0;
  };

  /** In-memory storage backend using contiguous std::vector */
  class MemoryBackend
    : public StorageBackend
  {
  public:
    MemoryBackend();
    explicit MemoryBackend(UnsignedInteger size);
    Complex * data() override;
    const Complex * data() const override;
    void resize(UnsignedInteger n) override;
    UnsignedInteger size() const override;
    Bool isShareable() const override;
    Pointer<StorageBackend> clone() const override;
  private:
    std::vector<Complex> storage_;
  };

  /** Default constructor */
  ComplexDataContainer();

  /** Constructor with size */
  explicit ComplexDataContainer(UnsignedInteger size);

  /** Virtual constructor */
  ComplexDataContainer * clone() const override;

#ifndef SWIG
  /** Copy constructor (deep-copies the storage) */
  ComplexDataContainer(const ComplexDataContainer & other);

  /** Assignment operator (deep-copies the storage) */
  ComplexDataContainer & operator = (const ComplexDataContainer & other);
#endif

  /** String converter */
  String __repr__() const override;

  /** Number of elements */
  UnsignedInteger getSize() const;

#ifndef SWIG
  /** Flat element access */
  inline Complex & operator [] (UnsignedInteger flatIndex)
  {
    return cachedData_[flatIndex];
  }
  inline const Complex & operator [] (UnsignedInteger flatIndex) const
  {
    return cachedData_[flatIndex];
  }

  /** Method begin() points to the first element */
  inline Complex * begin()
  {
    return cachedData_;
  }
  inline const Complex * begin() const
  {
    return cachedData_;
  }

  /** Method end() points beyond the last element */
  inline Complex * end()
  {
    return cachedData_ + getSize();
  }
  inline const Complex * end() const
  {
    return cachedData_ + getSize();
  }

  /** Returns a pointer to the block of memory */
  inline Complex * data()
  {
    return cachedData_;
  }
  inline const Complex * data() const
  {
    return cachedData_;
  }
#endif

  /** Resize */
  void resize(UnsignedInteger newSize);

  /** Re-synchronize the cached data pointer with the current backend */
  inline void refreshCache()
  {
    cachedData_ = backend_->data();
  }

  /** Backend access */
  Pointer<StorageBackend> getBackend();

  /** Replace the storage backend (data is copied into the new backend) */
  void setBackend(Pointer<StorageBackend> newBackend);

private:

  /** Pluggable storage backend */
  Pointer<StorageBackend> backend_;

  /** Cached raw pointer to the backend buffer */
  Complex * cachedData_;

}; /* class ComplexDataContainer */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPLEXDATACONTAINER_HXX */
