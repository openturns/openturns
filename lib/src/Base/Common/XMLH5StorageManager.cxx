//                                               -*- C++ -*-
/**
 *  @brief XMLH5StorageManager implements xml/h5 storage
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/Os.hxx"

#include <H5Cpp.h>
#include <libxml/tree.h>

BEGIN_NAMESPACE_OPENTURNS


class XMLH5StorageManagerImplementation
{
public:

  explicit XMLH5StorageManagerImplementation(const FileName & h5FileName,
      const UnsignedInteger compressionLevel = 0)
    : h5FileName_(h5FileName)
    , compressionLevel_(compressionLevel)
  {}

  template <class CPP_Type>
  void addIndexedValue(Pointer<StorageManager::InternalObject> & p_obj, UnsignedInteger index, CPP_Type value);

  template <class CPP_Type>
  void readIndexedValue(Pointer<StorageManager::InternalObject> & p_obj, UnsignedInteger index, CPP_Type & value);

private:
  template <class CPP_Type>
  void writeToH5(const String & dataSetName);

  template <class CPP_Type>
  void readFromH5(const String & dataSetName);

  // Buffer size for writing hdf5 slabs
  const UnsignedInteger BufferSize = 1048576;

  // 30kB (max recommended dataset header size)
  // https://support.hdfgroup.org/HDF5/doc/UG/HDF5_Users_Guide.pdf
  const UnsignedInteger MaxHeaderSize = 30720;

  template <class CPP_Type> inline std::vector<CPP_Type> & getBuffer();

  FileName h5FileName_;
  std::vector<Scalar> valBuf_Scalar_;
  std::vector<UnsignedInteger> valBuf_UnsignedInteger_;
  Bool isFirstDS_ = true;
  UnsignedInteger compressionLevel_ = 0;
};


template <> inline std::vector<Scalar> & XMLH5StorageManagerImplementation::getBuffer()
{
  return valBuf_Scalar_;
}
template <> inline std::vector<UnsignedInteger> & XMLH5StorageManagerImplementation::getBuffer()
{
  return valBuf_UnsignedInteger_;
}

template <class CPP_Type> inline H5::DataType getDataType();
template <> inline H5::DataType getDataType<Scalar>()
{
  return H5::PredType::IEEE_F64LE;
}
template <> inline H5::DataType getDataType<UnsignedInteger>()
{
  return H5::PredType::NATIVE_ULONG;
}


template <class CPP_Type>
void XMLH5StorageManagerImplementation::addIndexedValue(Pointer<StorageManager::InternalObject> & p_obj,
    UnsignedInteger index,
    CPP_Type value)
{
  // Get XML node associated to the Collection
  assert(p_obj);
  XMLInternalObject & obj = dynamic_cast<XMLInternalObject&>(*p_obj);
  XML::Node node = obj.node_;
  assert(node);
  const hsize_t dsetSize = std::stoi(XML::GetAttributeByName(node, "size"));

  // append value in buffer
  getBuffer<CPP_Type>().push_back(value);

  // if last value or buffer full then write to dataset
  if ((index == dsetSize - 1) || ((index % BufferSize == 0) && (index > 0)))
  {
    String dataSetName = XML::GetAttributeByName(node, "id");
    writeToH5<CPP_Type>(dataSetName);
  }

  // if last value then add XML node
  if (index == dsetSize - 1)
  {
    String dataSetName = XML::GetAttributeByName(node, "id");
    const size_t idx = h5FileName_.find_last_of(Os::GetDirectorySeparator());
    const FileName h5FileNameRel = h5FileName_.substr(idx + 1);
    XML::Node child = XML::NewNode(XML_STMGR::string_tag::Get(), h5FileNameRel + ":/" + dataSetName);
    assert(child);
    XML::AddChild( node, child );
  }
}


template <class CPP_Type>
void XMLH5StorageManagerImplementation::writeToH5(const String & dataSetName)
{
  H5::Exception::dontPrint();
  H5::H5File h5File;
  if (isFirstDS_)
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

  const hsize_t dims[1] = { getBuffer<CPP_Type>().size() };

  if (!H5Lexists(h5File.getId(), dataSetName.c_str(), H5P_DEFAULT))
  {
    //Dataset does not exist, need to create it and initialize it with first chunk
    const hsize_t maxdims[1] = { H5S_UNLIMITED };
    H5::DataSpace dsp;
    H5::DSetCreatPropList prop;
    if (getBuffer<CPP_Type>().size() < BufferSize)
    {
      dsp = H5::DataSpace(1, dims, dims);
      // Set dataspace compact if dataset can fit into dataset header
      // Set it contiguous otherwise
      if (getBuffer<CPP_Type>().size()*sizeof(CPP_Type) < MaxHeaderSize)
        prop.setLayout(H5D_COMPACT);
      else
        prop.setLayout(H5D_CONTIGUOUS);
    }
    //Set dataspace to unlimited if full buffer
    else
      dsp = H5::DataSpace(1, dims, maxdims);
    //Propagate dataspace properties to dataset
    prop.setChunk(1, dims);

    // enable compression
    prop.setDeflate(compressionLevel_);

    //Create new dataset and write it
    H5::DataSet dset(h5File.createDataSet(dataSetName, getDataType<CPP_Type>(), dsp, prop));
    dset.write(getBuffer<CPP_Type>().data(), getDataType<CPP_Type>());
    prop.close();
  }
  else
  {
    //Dataset exists, and will be appended with buffer values
    H5::DataSet dset(h5File.openDataSet(dataSetName));
    //Get actual dset size
    const hsize_t offset[1] = { (hsize_t)dset.getSpace().getSimpleExtentNpoints() };
    const hsize_t extent[1] = { dims[0] + offset[0] };
    //Extend dset size by the buffer size
    dset.extend(extent);
    //Get updated dataspace
    H5::DataSpace filespace(dset.getSpace());
    filespace.selectHyperslab(H5S_SELECT_SET, dims, offset);
    //Create space for new data
    H5::DataSpace memspace(1, dims, NULL);
    //Write new data
    dset.write(getBuffer<CPP_Type>().data(), getDataType<CPP_Type>(), memspace, filespace);
  }
  getBuffer<CPP_Type>().clear();
  h5File.close();
}


template <class CPP_Type>
void XMLH5StorageManagerImplementation::readIndexedValue(Pointer<StorageManager::InternalObject> & p_obj,
    UnsignedInteger index,
    CPP_Type & value)
{
  assert(p_obj);
  //Read values only once
  XMLH5StorageManagerState & state = dynamic_cast<XMLH5StorageManagerState &>(*p_obj);

  if (index == 0)
  {
    XML::Node node = state.current_->parent;
    String dataSetName = XML::GetAttributeByName(node, "id");
    readFromH5<CPP_Type>(dataSetName);
    state.reachedEnd_ = false;
  }
  if (index == getBuffer<CPP_Type>().size() - 1)
  {
    state.reachedEnd_ = true;
  }
  state.next();
  //Get value from index
  value = getBuffer<CPP_Type>()[index];
}


template <class CPP_Type>
void XMLH5StorageManagerImplementation::readFromH5(const String & dataSetName)
{
  H5::Exception::dontPrint();
  H5::H5File file(h5FileName_.c_str(), H5F_ACC_RDONLY);
  H5::DataSet dataset = file.openDataSet(dataSetName.c_str());
  H5::DataSpace dataspace = dataset.getSpace();
  const int size = dataspace.getSimpleExtentNpoints();

  getBuffer<CPP_Type>().resize(size);
  dataset.read(getBuffer<CPP_Type>().data(), getDataType<CPP_Type>());

  dataspace.close();
  dataset.close();
  file.close();
}

CLASSNAMEINIT(XMLH5StorageManager)


/* Default constructor */
XMLH5StorageManager::XMLH5StorageManager(const FileName & filename,
    const UnsignedInteger compressionLevel)
  : XMLStorageManager(filename, compressionLevel)
{
  p_state_ = new XMLH5StorageManagerState;
  FileName filenameH5(filename);
  if (filenameH5.size() > 3)
  {
    const String extension(filenameH5.substr(filenameH5.size() - 3));
    if (extension == ".gz")
      filenameH5 = filenameH5.substr(0, filenameH5.size() - 3);
  }
  if (filenameH5.size() > 4)
  {
    const String extension(filenameH5.substr(filenameH5.size() - 4));
    if (extension == ".xml")
      filenameH5 = filenameH5.substr(0, filenameH5.size() - 4);
  }
  filenameH5 += ".h5";
  p_implementation_ = new XMLH5StorageManagerImplementation(filenameH5, compressionLevel);
}

/*
 * Virtual constructor
 */
XMLH5StorageManager * XMLH5StorageManager::clone() const
{
  return new XMLH5StorageManager(*this);
}


void XMLH5StorageManager::checkStorageManager()
{
  if (XML::GetAttributeByName( p_state_->root_, XML_STMGR::manager_attribute::Get()) !=
      "XMLH5StorageManager")
    throw StudyFileParsingException(HERE) << XML::GetAttributeByName( p_state_->root_, XML_STMGR::manager_attribute::Get())
                                          << " is used in study file. XMLH5StorageManager is expected";
}


void XMLH5StorageManager::setStorageManager()
{
  XML::SetAttribute(p_state_->root_, XML_STMGR::manager_attribute::Get(), "XMLH5StorageManager");
}


void XMLH5StorageManager::addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Scalar value)
{
  p_implementation_->addIndexedValue(p_obj, index, value);
}

void XMLH5StorageManager::addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, UnsignedInteger value)
{
  p_implementation_->addIndexedValue(p_obj, index, value);
}

void XMLH5StorageManager::readIndexedValue(Pointer<StorageManager::InternalObject> & p_obj, UnsignedInteger index, UnsignedInteger & value)
{
  // we started to store integers into h5 in 1.17
  if (getStudyVersion() >= 101700)
    p_implementation_->readIndexedValue(p_obj, index, value);
  else
    XMLStorageManager::readIndexedValue(p_obj, index, value);
}

void XMLH5StorageManager::readIndexedValue(Pointer<StorageManager::InternalObject> & p_obj, UnsignedInteger index, Scalar & value)
{
  p_implementation_->readIndexedValue(p_obj, index, value);
}

END_NAMESPACE_OPENTURNS
