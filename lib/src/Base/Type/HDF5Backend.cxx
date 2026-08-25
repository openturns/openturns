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
#include "openturns/HDF5Backend.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

#ifdef OPENTURNS_HAVE_HDF5
#include <H5Cpp.h>
#include <H5Lpublic.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

HDF5Backend::HDF5Backend()
  : StorageBackend()
  , fileName_()
  , dataSetName_()
  , size_(0)
  , h5File_(nullptr)
  , h5Dataset_(nullptr)
  , h5Dataspace_(nullptr)
  , buffer_()
  , bufferOffset_(0)
  , bufferValid_(false)
  , compressionLevel_(0)
  , readOnly_(false)
{
}

HDF5Backend::HDF5Backend(const FileName & fileName,
                         const String & dataSetName,
                         UnsignedInteger n,
                         UnsignedInteger compressionLevel)
  : StorageBackend()
  , fileName_(fileName)
  , dataSetName_(dataSetName)
  , size_(n)
  , h5File_(nullptr)
  , h5Dataset_(nullptr)
  , h5Dataspace_(nullptr)
  , buffer_(n)
  , bufferOffset_(0)
  , bufferValid_(true)
  , compressionLevel_(compressionLevel)
  , readOnly_(false)
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "HDF5Backend requires HDF5 support";
#else
  try
  {
    H5::Exception::dontPrint();

    // Create or truncate the file
    H5::H5File * p_file = new H5::H5File(fileName_.c_str(), H5F_ACC_TRUNC);
    h5File_ = p_file;

    // Create 1D dataspace
    hsize_t dims[1] = { static_cast<hsize_t>(n) };
    hsize_t maxdims[1] = { H5S_UNLIMITED };
    H5::DataSpace * p_dspace = new H5::DataSpace(1, dims, maxdims);
    h5Dataspace_ = p_dspace;

    // Create dataset with chunking and optional compression
    hsize_t chunkDims[1] = { std::min(static_cast<hsize_t>(n), static_cast<hsize_t>(4096)) };
    H5::DSetCreatPropList prop;
    prop.setChunk(1, chunkDims);
    if (compressionLevel > 0)
      prop.setDeflate(compressionLevel);

    H5::DataSet * p_dset = new H5::DataSet(
      p_file->createDataSet(dataSetName_.c_str(),
                            H5::PredType::NATIVE_DOUBLE,
                            *p_dspace,
                            prop));
    h5Dataset_ = p_dset;

    // Initialize dataset to zero
    std::fill(buffer_.begin(), buffer_.end(), 0.0);
  }
  catch (const H5::Exception &)
  {
    throw FileOpenException(HERE) << "Error creating HDF5 file " << fileName_;
  }
#endif
}

Pointer<HDF5Backend> HDF5Backend::Open(const FileName & fileName,
                                       const String & dataSetName)
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "HDF5Backend requires HDF5 support";
#else
  H5::Exception::dontPrint();

  Pointer<HDF5Backend> p_backend(new HDF5Backend());
  p_backend->fileName_ = fileName;
  p_backend->dataSetName_ = dataSetName;
  p_backend->readOnly_ = true;

  try
  {
    H5::H5File * p_file = new H5::H5File(fileName.c_str(), H5F_ACC_RDONLY);
    p_backend->h5File_ = p_file;

    H5::DataSet dset = p_file->openDataSet(dataSetName.c_str());
    H5::DataSpace dspace = dset.getSpace();

    hsize_t dims[1];
    dspace.getSimpleExtentDims(dims);
    p_backend->size_ = static_cast<UnsignedInteger>(dims[0]);

    H5::DataSet * p_dset = new H5::DataSet(dset);
    p_backend->h5Dataset_ = p_dset;

    H5::DataSpace * p_dspace = new H5::DataSpace(dspace);
    p_backend->h5Dataspace_ = p_dspace;

    // Read entire dataset into buffer
    p_backend->buffer_.resize(p_backend->size_);
    dset.read(p_backend->buffer_.data(), H5::PredType::NATIVE_DOUBLE);
    p_backend->bufferOffset_ = 0;
    p_backend->bufferValid_ = true;
  }
  catch (const H5::Exception &)
  {
    throw FileOpenException(HERE) << "Error opening HDF5 dataset " << fileName << ":/" << dataSetName;
  }

  return p_backend;
#endif
}

HDF5Backend::~HDF5Backend()
{
#ifndef OPENTURNS_HAVE_HDF5
  return;
#else
  flush();

#ifdef _MSC_VER
  delete static_cast<H5::DataSet *>(h5Dataset_);
  delete static_cast<H5::DataSpace *>(h5Dataspace_);
  delete static_cast<H5::H5File *>(h5File_);
#else
  delete reinterpret_cast<H5::DataSet *>(h5Dataset_);
  delete reinterpret_cast<H5::DataSpace *>(h5Dataspace_);
  delete reinterpret_cast<H5::H5File *>(h5File_);
#endif

  h5Dataset_ = nullptr;
  h5Dataspace_ = nullptr;
  h5File_ = nullptr;
#endif
}

Scalar * HDF5Backend::data()
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "HDF5Backend requires HDF5 support";
#else
  if (!bufferValid_)
  {
    // Reload from disk
    try
    {
      reinterpret_cast<H5::DataSet *>(h5Dataset_)->read(
        buffer_.data(), H5::PredType::NATIVE_DOUBLE);
      bufferValid_ = true;
    }
    catch (const H5::Exception &)
    {
      throw InternalException(HERE) << "Error reading HDF5 dataset";
    }
  }
  return buffer_.data();
#endif
}

const Scalar * HDF5Backend::data() const
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "HDF5Backend requires HDF5 support";
#else
  if (!bufferValid_)
  {
    try
    {
      reinterpret_cast<const H5::DataSet *>(h5Dataset_)->read(
        buffer_.data(), H5::PredType::NATIVE_DOUBLE);
      bufferValid_ = true;
    }
    catch (const H5::Exception &)
    {
      throw InternalException(HERE) << "Error reading HDF5 dataset";
    }
  }
  return buffer_.data();
#endif
}

void HDF5Backend::resize(UnsignedInteger n)
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "HDF5Backend requires HDF5 support";
#else
  if (n == size_) return;

  try
  {
    // Flush pending changes first
    flush();

    // Extend the dataset
    hsize_t newDims[1] = { static_cast<hsize_t>(n) };
    reinterpret_cast<H5::DataSet *>(h5Dataset_)->extend(newDims);

    // Resize the in-memory buffer
    buffer_.resize(n);
    size_ = n;
    bufferOffset_ = 0;
    bufferValid_ = false; // invalidate, will reload on next access
  }
  catch (const H5::Exception &)
  {
    throw InternalException(HERE) << "Error resizing HDF5 dataset to " << n;
  }
#endif
}

UnsignedInteger HDF5Backend::size() const
{
  return size_;
}

Bool HDF5Backend::isShareable() const
{
  return false;
}

Pointer<DataContainer::StorageBackend> HDF5Backend::clone() const
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "HDF5Backend requires HDF5 support";
#else
  static UnsignedInteger counter = 0;
  FileName tempFile = String("/tmp/ot_hdf5_") + String(OSS() << ++counter) + ".h5";

  Pointer<HDF5Backend> p_copy(new HDF5Backend(tempFile, dataSetName_ + "_copy", size_, compressionLevel_));
  if (size_ > 0)
  {
    std::copy(buffer_.begin(), buffer_.end(), p_copy->buffer_.begin());
    p_copy->bufferValid_ = true;
  }
  return p_copy;
#endif
}

void HDF5Backend::flush()
{
#ifndef OPENTURNS_HAVE_HDF5
  return;
#else
  if (readOnly_ || !bufferValid_) return;

  try
  {
    reinterpret_cast<H5::DataSet *>(h5Dataset_)->write(
      buffer_.data(), H5::PredType::NATIVE_DOUBLE);
  }
  catch (const H5::Exception &)
  {
    throw InternalException(HERE) << "Error flushing HDF5 dataset to disk";
  }
#endif
}

FileName HDF5Backend::getFileName() const
{
  return fileName_;
}

String HDF5Backend::getDataSetName() const
{
  return dataSetName_;
}

void HDF5Backend::readBlock(Scalar * dest, UnsignedInteger offset, UnsignedInteger n) const
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "HDF5Backend requires HDF5 support";
#else
  if (offset + n > size_)
    throw InvalidArgumentException(HERE) << "Block [" << offset << ", " << offset + n << ") exceeds dataset size " << size_;

  // Fast path: if the requested block is within the in-memory buffer
  if (bufferValid_ && offset >= bufferOffset_ && offset + n <= bufferOffset_ + buffer_.size())
  {
    std::copy(buffer_.data() + (offset - bufferOffset_),
              buffer_.data() + (offset - bufferOffset_ + n),
              dest);
    return;
  }

  // Slow path: hyperslab read from disk
  try
  {
    H5::DataSet * p_dset = reinterpret_cast<H5::DataSet *>(h5Dataset_);
    H5::DataSpace * p_dspace = reinterpret_cast<H5::DataSpace *>(h5Dataspace_);

    // Select hyperslab
    hsize_t offset_h5[1] = { static_cast<hsize_t>(offset) };
    hsize_t count_h5[1] = { static_cast<hsize_t>(n) };
    H5::DataSpace memSpace(1, count_h5);

    p_dspace->selectHyperslab(H5S_SELECT_SET, count_h5, offset_h5);
    p_dset->read(dest, H5::PredType::NATIVE_DOUBLE, memSpace, *p_dspace);

    // Update buffer to cover this block for subsequent fast access
    bufferOffset_ = offset;
    buffer_.resize(n);
    std::copy(dest, dest + n, buffer_.begin());
    bufferValid_ = true;
  }
  catch (const H5::Exception &)
  {
    throw InternalException(HERE) << "Error reading block [" << offset << ", " << offset + n << ") from HDF5";
  }
#endif
}

void HDF5Backend::writeBlock(const Scalar * src, UnsignedInteger offset, UnsignedInteger n)
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "HDF5Backend requires HDF5 support";
#else
  if (offset + n > size_)
    throw InvalidArgumentException(HERE) << "Block [" << offset << ", " << offset + n << ") exceeds dataset size " << size_;

  // Fast path: if the requested block is within the in-memory buffer
  if (bufferValid_ && offset >= bufferOffset_ && offset + n <= bufferOffset_ + buffer_.size())
  {
    std::copy(src, src + n, buffer_.data() + (offset - bufferOffset_));
    return;
  }

  // Slow path: hyperslab write to disk
  try
  {
    H5::DataSet * p_dset = reinterpret_cast<H5::DataSet *>(h5Dataset_);
    H5::DataSpace * p_dspace = reinterpret_cast<H5::DataSpace *>(h5Dataspace_);

    hsize_t offset_h5[1] = { static_cast<hsize_t>(offset) };
    hsize_t count_h5[1] = { static_cast<hsize_t>(n) };
    H5::DataSpace memSpace(1, count_h5);

    p_dspace->selectHyperslab(H5S_SELECT_SET, count_h5, offset_h5);
    p_dset->write(src, H5::PredType::NATIVE_DOUBLE, memSpace, *p_dspace);

    // Update buffer to cover this block
    bufferOffset_ = offset;
    buffer_.resize(n);
    std::copy(src, src + n, buffer_.begin());
    bufferValid_ = true;
  }
  catch (const H5::Exception &)
  {
    throw InternalException(HERE) << "Error writing block [" << offset << ", " << offset + n << ") to HDF5";
  }
#endif
}


END_NAMESPACE_OPENTURNS
