//                                               -*- C++ -*-
/**
 *  @brief ResourceMap defines a resource catalog
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-Phimeca
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

#ifndef OPENTURNS_RESOURCEMAP_HXX
#define OPENTURNS_RESOURCEMAP_HXX

#include <map>

#include "openturns/Log.hxx" // ensures correct static initialization order
#include "openturns/OStream.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Path.hxx"
#include "openturns/MutexLock.hxx"
#include "openturns/ResourceMapBase.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ResourceMap
 * @brief Defines a catalog containing all default values used by OpenTURNS
 *
 * As OpenTURNS uses many default values for its computations, their actual values are
 * accessible and editable through this class. Some default values are hardcoded in this class,
 * some other are read in an configuration file.
 */

class OT_API ResourceMap
{
public:
#ifndef SWIG
  /** GetInstance gives a locked access to the singleton */
  static MutexLockSingleton<ResourceMap> GetInstance();
#endif

  /** Get a value in the maps */
  static String GetType(const String & key);
  static String Get(const String & key);
  static String GetAsString(const String & key);
  static Bool GetAsBool(const String & key);
  static UnsignedInteger GetAsUnsignedInteger(const String & key);
  static Scalar GetAsScalar(const String & key);

  /** Set a value in the maps */
  static void Set(const String & key, const String & value);
  static void SetAsString(const String & key, const String & value);
  static void SetAsBool(const String & key, Bool value);
  static void SetAsUnsignedInteger(const String & key, UnsignedInteger value);
  static void SetAsScalar(const String & key, Scalar value);

  /** Get the total size of the maps */
  static UnsignedInteger GetSize();
  static UnsignedInteger GetStringSize();
  static UnsignedInteger GetScalarSize();
  static UnsignedInteger GetUnsignedIntegerSize();
  static UnsignedInteger GetBoolSize();

  /** Get the list of keys in the maps */
  static std::vector<String> GetKeys();
  static std::vector<String> GetStringKeys();
  static std::vector<String> GetScalarKeys();
  static std::vector<String> GetUnsignedIntegerKeys();
  static std::vector<String> GetBoolKeys();

  /** Is the specific key present ? */
  static Bool HasKey(const String & key);
  
  /** Remove a key */
  static void RemoveKey(const String & key);

  /** Reload configuration */
  static void Reload();

  /** @copydoc Object::__repr__() const */
  String __repr__() const;

  /** Update the ResourceMap with information from the configuration file */
  void readConfigurationFile(const FileName & configurationFile);

  /** Find the configuration file in specific path (see Path class for algorithm) */
  FileName findConfigurationFile() const;

  /** Load the configuration file */
  void loadConfigurationFile();

  /** Load the configuration defined at installation time */
  void loadDefaultConfiguration();

  /** Reload configuration */
  void reload();
private:

  /** Default constructor */
  ResourceMap();

  /** Default constructor */
  ResourceMap(const ResourceMap & other)
    : map_(other.map_)
  {}

  /* Actual data storage object */
  ResourceMapBase map_;

  friend struct ResourceMap_init;
}; /* class ResourceMap */

/** This struct initializes all static members of ResourceMap */
struct OT_API ResourceMap_init
{
  ResourceMap_init();
  ~ResourceMap_init();
};

/**
 * @fn std::ostream & operator <<(std::ostream & os, const ResourceMap & obj)
 * @brief Output stream converter
 * @param os A STL output stream resourceMap
 * @param obj The resourceMap read by \em os
 * @return A reference to \em os
 *
 * Operator << converts the ResourceMap object to an output stream
 * so it is easy to show the content of the resourceMap.
 */
#ifndef SWIG
OT_API std::ostream & operator <<(std::ostream & os, const MutexLockSingleton<ResourceMap> & obj);
OT_API OStream & operator <<(OStream & OS, const MutexLockSingleton<ResourceMap> & obj);
#endif


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RESOURCEMAP_HXX */
