//                                               -*- C++ -*-
/**
 *  @brief H5StorageManager provides an interface for different
 *  HDF5-based storage classes
 *
 *  Copyright 2005-2020 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/XMLH5StorageManager.hxx"
#include "openturns/PersistentObject.hxx"

#include <H5Cpp.h>

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(XMLH5StorageManager)

const UnsignedInteger BufferSize = 2048; //Buffer size for writing hdf5 slabs

/* Default constructor */
XMLH5StorageManager::XMLH5StorageManager(const FileName & filename,
    const UnsignedInteger compressionLevel)
  : XMLStorageManager(filename, compressionLevel)
  , h5FileName_(filename.substr(0, filename.find_last_of('.')) + ".h5")
{
  p_state_ = new XMLH5StorageManagerState;
  isFirstDS_ = true;
  isChunked_ = false;
}

/*
 * Virtual constructor
 */
XMLH5StorageManager * XMLH5StorageManager::clone() const
{
  return new XMLH5StorageManager(*this);
}

void XMLH5StorageManager::addIndexedValue(Pointer<InternalObject> & p_obj,
    UnsignedInteger index,
    Scalar value)
{
  // Get XML node associated to the Collection
  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  hsize_t dsetSize = std::stoi(XML::GetAttributeByName(node, "size"));
  //append values in buffer
  valBuf_.push_back(value);


  //Every nth value write to dataset
  if(index && index % BufferSize == 0)
  {
    isChunked_ = true;
    String dataSetName = XML::GetAttributeByName(node, "id");
    writeToH5(dataSetName);
  }

  // if last value, write to dataset and add corresponding XML node
  if(index == dsetSize - 1)
  {
    String dataSetName = XML::GetAttributeByName(node, "id");
    writeToH5(dataSetName);
    XML::Node child = XML::NewNode(XML_STMGR::string_tag::Get(), h5FileName_ + ":/" + dataSetName);
    assert(child);
    XML::AddChild( node, child );
    isChunked_ = false;
  }
}

void XMLH5StorageManager::writeToH5(const String & dataSetName)
{
  H5::Exception::dontPrint();
  H5::H5File h5File;
  if(isFirstDS_)
  {
    //Create new or overwrite existing
    h5File = H5::H5File(h5FileName_.c_str(), H5F_ACC_TRUNC);
    isFirstDS_ = false;
  }
  else
  {
    //R+W access
    h5File = H5::H5File(h5FileName_.c_str(), H5F_ACC_RDWR);
  }

  hsize_t dims[1] = {valBuf_.size()};

  if(!H5Lexists(h5File.getId(), dataSetName.c_str(), H5P_DEFAULT))
  {
    //Dataset does not exist, need to create it and initialize it with first chunk
    hsize_t maxdims[1] = {H5S_UNLIMITED};
    //Set dataspace to unlimited
    H5::DataSpace *dsp = new H5::DataSpace (1, dims, maxdims);
    //Propagate dataspace properties to dataset
    H5::DSetCreatPropList prop;
    prop.setChunk(1, dims);
    //Create new dataset and write it
    H5::DataSet *dset = new H5::DataSet(h5File.createDataSet(dataSetName, H5::PredType::IEEE_F64LE, *dsp, prop));
    dset->write(valBuf_.data(), H5::PredType::IEEE_F64LE);
    prop.close();
    delete dsp;
    delete dset;
  }
  else
  {
    //Dataset exists, and will be appended with buffer values
    H5::DataSet * dset = new H5::DataSet(h5File.openDataSet(dataSetName));
    //Get actual dset size
    hsize_t offset[1] = {(hsize_t)dset->getSpace().getSimpleExtentNpoints()};
    hsize_t extent[1];
    extent[0] = dims[0] + offset[0];
    //Extend dset size by the buffer size
    dset->extend(extent);
    //Get updated dataspace
    H5::DataSpace *filespace = new H5::DataSpace(dset->getSpace());
    filespace->selectHyperslab(H5S_SELECT_SET, dims, offset);
    //Create space for new data
    H5::DataSpace *memspace = new H5::DataSpace(1, dims, NULL);
    //Write new data
    dset->write(valBuf_.data(), H5::PredType::IEEE_F64LE, *memspace, *filespace);
    delete dset;
    delete filespace;
    delete memspace;
  }
  isChunked_ = true;
  valBuf_.clear();
}

void XMLH5StorageManager::readIndexedValue(Pointer<StorageManager::InternalObject> & p_obj,
    UnsignedInteger index,
    Scalar & value)
{
  assert(p_obj);
  //Read values only once
  XMLH5StorageManagerState & state = dynamic_cast<XMLH5StorageManagerState &>(*p_obj);

  if(index == 0)
  {
    XML::Node node = state.current_->parent;
    String dataSetName = XML::GetAttributeByName(node, "id");
    readFromH5(dataSetName);
    state.reachedEnd_ = false;
  }
  if(index == valBuf_.size() - 1)
  {
    state.reachedEnd_ = true;
  }
  state.next();
  //Get value from index
  value = valBuf_[index];
}

void XMLH5StorageManager::readFromH5(const String & dataSetName)
{
  H5::Exception::dontPrint();
  H5::H5File file(h5FileName_.c_str(), H5F_ACC_RDONLY);
  H5::DataSet dataset = file.openDataSet(dataSetName.c_str());
  H5::DataSpace dataspace = dataset.getSpace();
  const int size = dataspace.getSimpleExtentNpoints();

  double *data = new double[size];
  dataset.read(data, H5::PredType::IEEE_F64LE);
  valBuf_.clear();

  std::vector<double> d_vector(data, data + size);
  valBuf_ = d_vector;

  dataspace.close();
  dataset.close();
  file.close();
}

END_NAMESPACE_OPENTURNS
