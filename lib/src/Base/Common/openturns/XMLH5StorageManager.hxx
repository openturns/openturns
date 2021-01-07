//                                               -*- C++ -*-
/**
 *  @brief XMLH5StorageManager implements xml/h5 storage
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

  XMLH5StorageManagerState * clone() const override
  {
    return new XMLH5StorageManagerState(*this);
  }
  void next() override
  {
    // TODO: find a cleaner pattern for handling StorageManagerStates increment
    if (reachedEnd_)
    {
      current_ = XML::GetNextNode(current_);
    }
  }
  String __repr__() const override
  {
    return OSS(true) << "XMLH5StorageManagerState { root = <" << root_
           << ">, current_ = <" << current_ << ">}";
  }
};

class XMLH5StorageManagerImplementation;

#endif /* SWIG */

class OT_API XMLH5StorageManager : public XMLStorageManager
{
  CLASSNAME
public:
  explicit XMLH5StorageManager(const FileName & filename,
                               const UnsignedInteger compressionLevel = ResourceMap::GetAsUnsignedInteger("XMLStorageManager-DefaultCompressionLevel"));

  XMLH5StorageManager * clone() const override;

#ifndef SWIG
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, UnsignedInteger value) override;
  void addIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Scalar value) override;

  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, UnsignedInteger & value) override;
  void readIndexedValue(Pointer<InternalObject> & p_obj, UnsignedInteger index, Scalar & value) override;
#endif /* SWIG */

private:
  Pointer<XMLH5StorageManagerImplementation> p_implementation_;

}; /* class H5StorageManager */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_XMLH5STORAGEMANAGER_HXX */
