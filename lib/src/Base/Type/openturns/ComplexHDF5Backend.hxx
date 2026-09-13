//                                               -*- C++ -*-
/**
 *  @brief ComplexHDF5Backend provides disk-backed storage for ComplexDataContainer
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
#ifndef OPENTURNS_COMPLEXHDF5BACKEND_HXX
#define OPENTURNS_COMPLEXHDF5BACKEND_HXX

#include "openturns/ComplexDataContainer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ComplexHDF5Backend
 *
 * HDF5-backed storage for complex values. Values are persisted as two real
 * datasets (<name>_re / <name>_im) for portability, and kept in an in-memory
 * write-through buffer exactly like HDF5Backend.
 */
class OT_API ComplexHDF5Backend
  : public ComplexDataContainer::StorageBackend
{
public:

  /** Constructor: creates a new HDF5 file and the re/im datasets */
  ComplexHDF5Backend(const FileName & fileName,
                     const String & dataSetName,
                     UnsignedInteger n,
                     UnsignedInteger compressionLevel = 0);

  /** Factory: open existing re/im datasets (read-only) */
  static Pointer<ComplexHDF5Backend> Open(const FileName & fileName,
                                          const String & dataSetName);

  ~ComplexHDF5Backend() override;

  Complex * data() override;
  const Complex * data() const override;
  void resize(UnsignedInteger n) override;
  UnsignedInteger size() const override;
  Bool isShareable() const override;
  Pointer<ComplexDataContainer::StorageBackend> clone() const override;

  /** Write pending modifications to disk */
  void flush();

private:
  ComplexHDF5Backend() = default;
  void writeBlock(const Complex * src, UnsignedInteger offset, UnsignedInteger n);

  FileName fileName_;
  String dataSetName_;
  UnsignedInteger size_ = 0;
  std::vector<Complex> buffer_;
  void * h5File_ = nullptr;
  void * h5DatasetRe_ = nullptr;
  void * h5DatasetIm_ = nullptr;
  void * h5Dataspace_ = nullptr;
  UnsignedInteger compressionLevel_ = 0;
  Bool readOnly_ = false;
}; /* class ComplexHDF5Backend */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_COMPLEXHDF5BACKEND_HXX */
