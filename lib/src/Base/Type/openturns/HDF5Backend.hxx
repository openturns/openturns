//                                               -*- C++ -*-
/**
 *  @brief HDF5Backend provides disk-backed storage for DataContainer
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
#ifndef OPENTURNS_HDF5BACKEND_HXX
#define OPENTURNS_HDF5BACKEND_HXX

#include "openturns/DataContainer.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class HDF5Backend
 *
 * HDF5Backend provides disk-backed storage for DataContainer.
 * Data is stored in a single 1D HDF5 dataset within an HDF5 file.
 *
 * This backend enables out-of-core processing of large datasets.
 * LAPACK/BLAS operations cannot be called directly on this backend;
 * blocks must be loaded into memory, processed, and written back.
 * See doc/DataContainer_HDF5_constraints.md for details.
 *
 * Usage:
 *   DataContainer dc(1000, 500, 0.0, DataContainer::COLUMN_MAJOR);
 *   // ... fill data ...
 *   dc.setBackend(new HDF5Backend("/tmp/data.h5", "dataset", dc.size()));
 *   // Data is now on disk; data() pointer accesses via mmap or temp buffer
 *
 * NOTE: The file and dataset must not already exist; they are created
 * on construction. If you need to open an existing file, use the
 * static Open() factory method.
 */
class OT_API HDF5Backend
  : public DataContainer::StorageBackend
{
public:

  /**
   * Constructor: creates a new HDF5 file and dataset.
   *
   * @param fileName  Path to the HDF5 file (created if absent)
   * @param dataSetName  Name of the 1D dataset within the file
   * @param n  Number of Scalar elements to allocate
   * @param compressionLevel  HDF5 chunk compression level (0=off, 9=max)
   */
  HDF5Backend(const FileName & fileName,
              const String & dataSetName,
              UnsignedInteger n,
              UnsignedInteger compressionLevel = 0);

  /**
   * Factory: open an existing HDF5 dataset for reading.
   * The dataset must already exist in the file.
   */
  static Pointer<HDF5Backend> Open(const FileName & fileName,
                                   const String & dataSetName);

  /** Destructor: flushes and closes the HDF5 file */
  ~HDF5Backend() override;

  /* StorageBackend interface */
  Scalar * data() override;
  const Scalar * data() const override;
  void resize(UnsignedInteger n) override;
  UnsignedInteger size() const override;
  Bool isShareable() const override;
  Pointer<StorageBackend> clone() const override;

  /** Accessors */
  FileName getFileName() const;
  String getDataSetName() const;

  /** Flush pending writes to disk */
  void flush() override;

  /**
   * Block-level I/O for out-of-core processing.
   * Read a contiguous block of n Scalars starting at offset into dest.
   */
  void readBlock(Scalar * dest, UnsignedInteger offset, UnsignedInteger n) const;

  /**
   * Write n Scalars from src into the dataset starting at offset.
   * Flushed to disk on flush() or destruction.
   */
  void writeBlock(const Scalar * src, UnsignedInteger offset, UnsignedInteger n);

private:

  /** Default constructor (disabled) */
  HDF5Backend();

  /** Reopen the file in read/write mode and remap the dataset */
  void openFile();

  /** HDF5 file path */
  FileName fileName_;

  /** HDF5 dataset name */
  String dataSetName_;

  /** Current logical size (number of Scalars) */
  UnsignedInteger size_;

  /** HDF5 file handle (kept open for read/write) */
  void * h5File_;

  /** HDF5 dataset handle */
  void * h5Dataset_;

  /** HDF5 dataspace handle */
  void * h5Dataspace_;

  /** Temporary in-memory buffer for current access window */
  mutable std::vector<Scalar> buffer_;

  /** Buffer offset (where in the dataset the buffer starts) */
  mutable UnsignedInteger bufferOffset_;

  /** Buffer valid flag */
  mutable Bool bufferValid_;

  /** Compression level (0 = no compression) */
  UnsignedInteger compressionLevel_;

  /** Whether the file was opened read-only */
  Bool readOnly_;

}; /* class HDF5Backend */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_HDF5BACKEND_HXX */
