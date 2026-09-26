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
#include "openturns/ComplexHDF5Backend.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"

#include <algorithm>
#include <complex>

#ifdef OPENTURNS_HAVE_HDF5
#include <H5Cpp.h>
#include <H5Lpublic.h>
#endif

BEGIN_NAMESPACE_OPENTURNS

/* Constructor: creates a new HDF5 file and the re/im datasets */
ComplexHDF5Backend::ComplexHDF5Backend(const FileName & fileName,
                                       const String & dataSetName,
                                       UnsignedInteger n,
                                       UnsignedInteger compressionLevel)
  : fileName_(fileName)
  , dataSetName_(dataSetName)
  , size_(n)
  , buffer_(n)
  , compressionLevel_(compressionLevel)
  , readOnly_(false)
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "ComplexHDF5Backend requires HDF5 support";
#else
  try
  {
    H5::Exception::dontPrint();

    H5::H5File * p_file = new H5::H5File(fileName_.c_str(), H5F_ACC_TRUNC);
    h5File_ = p_file;

    hsize_t dims[1] = { static_cast<hsize_t>(n) };
    hsize_t maxdims[1] = { H5S_UNLIMITED };
    H5::DataSpace * p_dspace = new H5::DataSpace(1, dims, maxdims);
    h5Dataspace_ = p_dspace;

    hsize_t chunkDims[1] = { std::min(static_cast<hsize_t>(n), static_cast<hsize_t>(4096)) };
    H5::DSetCreatPropList prop;
    prop.setChunk(1, chunkDims);
    if (compressionLevel > 0)
      prop.setDeflate(compressionLevel);

    H5::DataSet * p_dsetRe = new H5::DataSet(
      p_file->createDataSet((dataSetName_ + "_re").c_str(),
                            H5::PredType::NATIVE_DOUBLE,
                            *p_dspace,
                            prop));
    h5DatasetRe_ = p_dsetRe;
    H5::DataSet * p_dsetIm = new H5::DataSet(
      p_file->createDataSet((dataSetName_ + "_im").c_str(),
                            H5::PredType::NATIVE_DOUBLE,
                            *p_dspace,
                            prop));
    h5DatasetIm_ = p_dsetIm;

    std::fill(buffer_.begin(), buffer_.end(), Complex(0.0, 0.0));
  }
  catch (const H5::Exception &)
  {
    throw FileOpenException(HERE) << "Error creating HDF5 file " << fileName_;
  }
#endif
}

/* Factory: open existing re/im datasets (read-only) */
Pointer<ComplexHDF5Backend> ComplexHDF5Backend::Open(const FileName & fileName,
                                                     const String & dataSetName)
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "ComplexHDF5Backend requires HDF5 support";
#else
  H5::Exception::dontPrint();

  Pointer<ComplexHDF5Backend> p_backend(new ComplexHDF5Backend());
  p_backend->fileName_ = fileName;
  p_backend->dataSetName_ = dataSetName;
  p_backend->readOnly_ = true;

  try
  {
    H5::H5File * p_file = new H5::H5File(fileName.c_str(), H5F_ACC_RDONLY);
    p_backend->h5File_ = p_file;

    H5::DataSet dsetRe = p_file->openDataSet((dataSetName + "_re").c_str());
    H5::DataSpace dspace = dsetRe.getSpace();
    hsize_t dims[1];
    dspace.getSimpleExtentDims(dims);
    p_backend->size_ = static_cast<UnsignedInteger>(dims[0]);

    H5::DataSet * p_dsetRe = new H5::DataSet(dsetRe);
    p_backend->h5DatasetRe_ = p_dsetRe;
    H5::DataSet * p_dsetIm = new H5::DataSet(p_file->openDataSet((dataSetName + "_im").c_str()));
    p_backend->h5DatasetIm_ = p_dsetIm;
    H5::DataSpace * p_dspace = new H5::DataSpace(dspace);
    p_backend->h5Dataspace_ = p_dspace;

    p_backend->buffer_.resize(p_backend->size_);
    std::vector<Scalar> re(p_backend->size_), im(p_backend->size_);
    dsetRe.read(re.data(), H5::PredType::NATIVE_DOUBLE);
    p_dsetIm->read(im.data(), H5::PredType::NATIVE_DOUBLE);
    for (UnsignedInteger i = 0; i < p_backend->size_; ++i)
      p_backend->buffer_[i] = Complex(re[i], im[i]);
  }
  catch (const H5::Exception &)
  {
    throw FileOpenException(HERE) << "Error opening HDF5 dataset " << fileName << ":/" << dataSetName;
  }

  return p_backend;
#endif
}

ComplexHDF5Backend::~ComplexHDF5Backend()
{
#ifndef OPENTURNS_HAVE_HDF5
  return;
#else
  try
  {
    flush();
  }
  catch (...)
  {
    // Never throw from a destructor.
  }

  delete reinterpret_cast<H5::DataSet *>(h5DatasetRe_);
  delete reinterpret_cast<H5::DataSet *>(h5DatasetIm_);
  delete reinterpret_cast<H5::DataSpace *>(h5Dataspace_);
  delete reinterpret_cast<H5::H5File *>(h5File_);

  h5DatasetRe_ = nullptr;
  h5DatasetIm_ = nullptr;
  h5Dataspace_ = nullptr;
  h5File_ = nullptr;
#endif
}

Complex * ComplexHDF5Backend::data()
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "ComplexHDF5Backend requires HDF5 support";
#else
  return buffer_.data();
#endif
}

const Complex * ComplexHDF5Backend::data() const
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "ComplexHDF5Backend requires HDF5 support";
#else
  return buffer_.data();
#endif
}

void ComplexHDF5Backend::resize(UnsignedInteger n)
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "ComplexHDF5Backend requires HDF5 support";
#else
  if (n == size_) return;

  flush();
  try
  {
    hsize_t newDims[1] = { static_cast<hsize_t>(n) };
    reinterpret_cast<H5::DataSet *>(h5DatasetRe_)->extend(newDims);
    reinterpret_cast<H5::DataSet *>(h5DatasetIm_)->extend(newDims);
    buffer_.resize(n);
    size_ = n;
  }
  catch (const H5::Exception &)
  {
    throw InternalException(HERE) << "Error resizing HDF5 datasets";
  }
#endif
}

UnsignedInteger ComplexHDF5Backend::size() const
{
  return size_;
}

Bool ComplexHDF5Backend::isShareable() const
{
  return false;
}

Pointer<ComplexDataContainer::StorageBackend> ComplexHDF5Backend::clone() const
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "ComplexHDF5Backend requires HDF5 support";
#else
  static UnsignedInteger counter = 0;
  FileName tempFile = String("/tmp/ot_chdf5_") + String(OSS() << ++counter) + ".h5";

  Pointer<ComplexHDF5Backend> p_copy(new ComplexHDF5Backend(tempFile, dataSetName_ + "_copy", size_, compressionLevel_));
  std::copy(buffer_.begin(), buffer_.end(), p_copy->buffer_.begin());
  return p_copy;
#endif
}

/* Write pending modifications to disk: split complex values into re/im datasets */
void ComplexHDF5Backend::flush()
{
#ifndef OPENTURNS_HAVE_HDF5
  throw NotYetImplementedException(HERE) << "ComplexHDF5Backend requires HDF5 support";
#else
  if (readOnly_) return;
  try
  {
    const UnsignedInteger n = buffer_.size();
    std::vector<Scalar> re(n), im(n);
    for (UnsignedInteger i = 0; i < n; ++i)
    {
      re[i] = buffer_[i].real();
      im[i] = buffer_[i].imag();
    }
    reinterpret_cast<H5::DataSet *>(h5DatasetRe_)->write(re.data(), H5::PredType::NATIVE_DOUBLE);
    reinterpret_cast<H5::DataSet *>(h5DatasetIm_)->write(im.data(), H5::PredType::NATIVE_DOUBLE);
  }
  catch (const H5::Exception &)
  {
    throw InternalException(HERE) << "Error flushing complex HDF5 datasets to disk";
  }
#endif
}

END_NAMESPACE_OPENTURNS
