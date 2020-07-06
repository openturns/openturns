//                                               -*- C++ -*-
/**
 *  @brief H5StorageManager provides an interface for different
 *  HDF5-base storage classes
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
#ifndef OPENTURNS_XMLH5STORAGEMANAGER_HXX
#define OPENTURNS_XMLH5STORAGEMANAGER_HXX

#include "openturns/XMLStorageManager.hxx"
#include "openturns/XMLToolbox.hxx"

BEGIN_NAMESPACE_OPENTURNS

#ifndef SWIG

struct XMLH5StorageManagerState : public XMLStorageManagerState
{
  Bool reachedEnd_;
  XMLH5StorageManagerState() : XMLStorageManagerState(), reachedEnd_(false) {}
  ~XMLH5StorageManagerState()  {}
  virtual XMLH5StorageManagerState * clone() const override
  {
    return new XMLH5StorageManagerState(*this);
  }
  void next() override
  {
    // TODO: find a cleaner pattern for handling StorageManagerStates increment
    if (index_ == (UnsignedInteger)std::stoi(XML::GetAttributeByName(current_->parent, "size"))-1
        || std::stoi(XML::GetAttributeByName(current_->parent, "size")) == 0) {
      current_ = XML::GetNextNode(current_);
      index_ = 0;
    }
  }
  String __repr__() const override
  {
    return OSS(true) << "XMLH5StorageManagerState { root = <" << root_
           << ">, current_ = <" << current_ << ">}";
  }
};

#endif /* SWIG */

class OT_API XMLH5StorageManager : public XMLStorageManager
{
  CLASSNAME
public:
  explicit XMLH5StorageManager(const FileName & filename,
                               const UnsignedInteger compressionLevel = ResourceMap::GetAsUnsignedInteger("XMLStorageManager-DefaultCompressionLevel"));

  virtual XMLH5StorageManager * clone() const override;

#ifndef SWIG
  //using XMLStorageManager::addIndexedValue;
  void addIndexedValue(Pointer<InternalObject> & p_obj,
                       UnsignedInteger index,
                       Scalar value) override;

  //using XMLStorageManager::readIndexedValue;
  void readIndexedValue(Pointer<InternalObject> & p_obj,
                        UnsignedInteger index,
                        Scalar & value) override;
#endif /* SWIG */

protected:
  void writeToH5(const OT::String & dataSetName);
  void readFromH5(const String & dataSetName);

private:
  FileName h5FileName_;
  std::vector<OT::Scalar> valBuf_;
  OT::Bool isFirstDS_;
  OT::Bool isChunked_;

}; /* class H5StorageManager */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_XMLH5STORAGEMANAGER_HXX */
