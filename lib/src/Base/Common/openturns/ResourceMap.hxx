//                                               -*- C++ -*-
/**
 *  @brief ResourceMap defines a resource catalog
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

#ifndef OPENTURNS_RESOURCEMAP_HXX
#define OPENTURNS_RESOURCEMAP_HXX

#include <map>

#include "openturns/Log.hxx" // ensures correct static initialization order
#include "openturns/OStream.hxx"
#include "openturns/Pointer.hxx"
#include "openturns/Path.hxx"
#include "openturns/MutexLock.hxx"

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
  static void SetAsBool(const String & key, const Bool value);
  static void SetAsUnsignedInteger(const String & key, const UnsignedInteger value);
  static void SetAsScalar(const String & key, const Scalar value);

  /** Add a value in the maps */
  static void AddAsString(const String & key, const String & value);
  static void AddAsBool(const String & key, const Bool value);
  static void AddAsUnsignedInteger(const String & key, const UnsignedInteger value);
  static void AddAsScalar(const String & key, const Scalar value);

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

  /** Get the list of keys associated to a class */
  static std::vector<String> FindKeys(const String & substr);

  /** String representation */
  String __repr__() const;

protected:
  /** Method for retrieving information from the resource map
   * @return The list of keys in the maps
   */
  std::vector<String> getKeys() const;

  /** Method for retrieving information from the resource map
   * @return The list of keys in the string map
   */
  std::vector<String> getStringKeys() const;

  /** Method for retrieving information from the resource map
   * @return The list of keys in the scalar map
   */
  std::vector<String> getScalarKeys() const;

  /** Method for retrieving information from the resource map
   * @return The list of keys in the unsigned integer map
   */
  std::vector<String> getUnsignedIntegerKeys() const;

  /** Method for retrieving information from the resource map
   * @return The list of keys in the bool map
   */
  std::vector<String> getBoolKeys() const;

  /** Method for retrieving information from the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @return the type of the key
   */
  String getType(const String & key) const;

  /** Method for retrieving information from the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @return The value written into a string if it is not already present in a dedicated map
   */
  String get(const String & key) const;

  /** Method for retrieving information from the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @return The value written into a string
   */
  String getAsString(const String & key) const;

  /** Method for retrieving information from the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @return The value if the value is boolean castable, false otherwise
   */
  Bool getAsBool(const String & key) const;

  /** Method for retrieving information from the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @return The value if the value is integer castable, zero otherwise
   */
  UnsignedInteger getAsUnsignedInteger(const String & key) const;

  /** Method for retrieving information from the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @return The value if the value is double castable, zero otherwise
   */
  Scalar getAsScalar(const String & key) const;

  /** Method for retrieving information from the resource map
   * @return The number of constants defined in one of the maps
   */
  UnsignedInteger getSize() const;

  /** Method for retrieving information from the resource map
   * @return The number of constants defined in the string map
   */
  UnsignedInteger getStringSize() const;

  /** Method for retrieving information from the resource map
   * @return The number of constants defined in the scalar map
   */
  UnsignedInteger getScalarSize() const;

  /** Method for retrieving information from the resource map
   * @return The number of constants defined in the unsigned integer map
   */
  UnsignedInteger getUnsignedIntegerSize() const;

  /** Method for retrieving information from the resource map
   * @return The number of constants defined in the bool map
   */
  UnsignedInteger getBoolSize() const;

  /** Method for setting information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value written to a string
   */
  void set(const String & key, const String & value);

  /** Method for setting information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value written to a string
   */
  void setAsString(const String & key, const String & value);

  /** Method for setting information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value as a boolean
   */
  void setAsBool(const String & key, const Bool value);

  /** Method for setting information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value as an integer
   */
  void setAsUnsignedInteger(const String & key, const UnsignedInteger value);

  /** Method for setting information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value as a double
   */
  void setAsScalar(const String & key, const Scalar value);

  /** Method for adding information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value written to a string
   */
  void addAsString(const String & key, const String & value);

  /** Method for adding information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value as a boolean
   */
  void addAsBool(const String & key, const Bool value);

  /** Method for adding information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value as an integer
   */
  void addAsUnsignedInteger(const String & key, const UnsignedInteger value);

  /** Method for adding information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value as a double
   */
  void addAsScalar(const String & key, const Scalar value);

  /** Is the specific key present ? */
  Bool hasKey(const String & key) const;

  /** Remove a key from the resource map */
  void removeKey(const String & key);

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

  /** Get the list of keys associated to a class */
  std::vector<String> findKeys(const String & substr);
private:

  /** Default constructor */
  ResourceMap();

  /** Default constructor */
  ResourceMap(const ResourceMap & other)
    : mapString_(other.mapString_)
    , mapScalar_(other.mapScalar_)
    , mapUnsignedInteger_(other.mapUnsignedInteger_)
    , mapBool_(other.mapBool_)
  {}

  /** The actual map that stores the key/value pairs */
  typedef std::map< String, String > MapStringType;
  typedef std::map< String, Scalar > MapScalarType;
  typedef std::map< String, UnsignedInteger > MapUnsignedIntegerType;
  typedef std::map< String, Bool > MapBoolType;

  MapStringType mapString_;
  MapScalarType mapScalar_;
  MapUnsignedIntegerType mapUnsignedInteger_;
  MapBoolType mapBool_;

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
