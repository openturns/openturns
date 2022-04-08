//                                               -*- C++ -*-
/**
 *  @brief Study keeps all PersistentObjects in a file
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
#ifndef OPENTURNS_STUDY_HXX
#define OPENTURNS_STUDY_HXX

#include <iostream>              // for std::ostream
#include <map>                   // for std::map
#include "openturns/Object.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/StorageManager.hxx"
#include "openturns/ResourceMap.hxx"

BEGIN_NAMESPACE_OPENTURNS

/* Forward declarations */
class InterfaceObject;
class StorageManager;

/**
 * @class Study
 * @brief Keeps all PersistentObjects in a file
 *
 * The study allows the user to save all its data to a structure looking like a map.
 * Tha data are copied verbatim to the study. This is not a link so future modification
 * of the original won't affect the data saved in the study. To update the data saved
 * in the study, the user has to explicitly save it again.
 * Study allows the user to retrieve previously saved objects either by their name if
 * a name was defined for the object or by their Id.
 * @see PersistentObject
 * @see StorageManager
 */

class OT_API Study
  : public Object
{
  CLASSNAME

  friend class StorageManager;

public:

  /* Some typedefs for easy reading */
  typedef Pointer<PersistentObject> MapElement;
  typedef std::map<Id, MapElement>  Map;
  typedef std::map<String, Id>      LabelMap;
  typedef Pointer<StorageManager>   StorageManagerImplementation;

  /**
   * Default constructor
   */
  Study();

  /**
   * Parameter constructor
   */
  Study(const String & fileName,
        const UnsignedInteger compressionLevel = ResourceMap::GetAsUnsignedInteger("XMLStorageManager-DefaultCompressionLevel"));

  /** @copydoc Object::__repr__() const */
  String __repr__() const override;

  /** @copydoc Object::__str__() const */
  String __str__(const String & offset = "") const override;


  /** This method saves the study through the storage manager */
  void save();

  /** This method reloads the study from the storage manager */
  void load();



  /** Defines which storage manager must be used to save and reload data */
  void setStorageManager(const StorageManager & smgr);

  /** Get the storage manager used by the study */
  StorageManagerImplementation getStorageManager() const;

  /** Add a PersistentObject to the study */
  void add(const InterfaceObject & io);

  /** Add a PersistentObject to the study */
  void add(const String & label, const InterfaceObject & io, Bool force = false);

  /** Add a PersistentObject to the study */
  void add(const PersistentObject & po);

  /** Add a PersistentObject to the study */
  void add(const String & label, const PersistentObject & po, Bool force = false);
#ifndef SWIG
  /** Add a PersistentObject to the study
   * @throw InvalidArgumentException
   */
  void add(const PersistentObject * po);

  /** Add a PersistentObject to the study
   * @throw InvalidArgumentException
   */
  void add(const String & label, const PersistentObject * po, Bool force = false);
#endif
  /** Remove a PersistentObject from the study */
  void remove(const InterfaceObject & io);

  /** Remove a PersistentObject from the study */
  void remove(const String & label);

  /** Query if object is stored in study
   * @param id The Id of the object
   * @return True if the object is stored in the study
   */
  Bool hasObject(Id id) const;

  /** Query if object is stored in study
   * @param label The label of the object
   * @return True if the object is stored in the study
   */
  Bool hasObject(const String & label) const;


  /** Get object whose id is given
   * @param id The Id of the object
   * @return The object saved in the study
   */
  MapElement getObject(Id id) const;

  /** Get object whose id is given
   * @param label The label of the object
   * @return The object saved in the study
   */
  MapElement getObject(const String & label) const;


  /** Fill an object with one got from study
   * @param po An object to be refilled (may be empty, ie default constructed)
   * @param name The name of the object stored in the study
   * @throw InvalidArgumentException
   */
  void fillObjectByName(PersistentObject & po, const String & name) const;

  /** Fill an object with one got from study
   * @param io An object to be refilled (may be empty, ie default constructed)
   * @param name The name of the object stored in the study
   * @throw InvalidArgumentException
   */
  void fillObjectByName(InterfaceObject  & io, const String & name) const;

  /** Fill an object with one got from study
   * @param id The Id of the object stored in the study
   * @param po An object to be refilled (may be empty, ie default constructed)
   * @throw InvalidArgumentException
   */
  void fillObject(Id id, PersistentObject & po) const;

  /** Fill an object with one got from study
   * @param id The Id of the object stored in the study
   * @param io An object to be refilled (may be empty, ie default constructed)
   * @throw InvalidArgumentException
   */
  void fillObject(Id id, InterfaceObject  & io) const;

  /** Fill an object with one got from study
   * @param label The label of the object stored in the study
   * @param po An object to be refilled (may be empty, ie default constructed)
   * @throw InvalidArgumentException
   */
  void fillObject(const String & name, PersistentObject & po) const;

  /** Fill an object with one got from study
   * @param label The label of the object stored in the study
   * @param io An object to be refilled (may be empty, ie default constructed)
   * @throw InvalidArgumentException
   */
  void fillObject(const String & name, InterfaceObject  & io) const;

  /** Print all the labels in the study */
  String printLabels() const;

protected:

  /** Define a label for an object */
  void defineLabel(Id id, const String & label);

  /** Define the visibility of an object */
  void defineVisibility(Id id, Bool visible);

  /** Get object whose class and name are given */
  MapElement getObjectByName(const String & className, const String & name) const;

  /** This method purges the study from the reloaded objects that are tagged unvisible */
  void cleanUnvisibleObject();

  /** This method performs the translation of ids after a study load */
  void translateId();


private:

  /** The map storing the saved objects */
  Map map_;

  /** The map storing the saved objects */
  LabelMap labelMap_;

  /** The StorageManager that drives the load/store operations */
  StorageManagerImplementation p_storageManager_;

}; /* class Study */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_STUDY_HXX */
