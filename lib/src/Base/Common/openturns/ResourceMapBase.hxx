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

#ifndef OPENTURNS_RESOURCEMAPBASE_HXX
#define OPENTURNS_RESOURCEMAPBASE_HXX

#include <map>
#include <vector>
#include "openturns/PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class ResourceMap
 * @brief Defines a catalog containing all default values used by OpenTURNS
 *
 * As OpenTURNS uses many default values for its computations, their actual values are
 * accessible and editable through this class. Some default values are hardcoded in this class,
 * some other are read in an configuration file.
 */

class OT_API ResourceMapBase
: public PersistentObject
{
  
  CLASSNAME
  
public:
    
  /** Default constructor */
  ResourceMapBase();

  virtual ResourceMapBase * clone() const;

  /** @copydoc Object::__repr__() const */
  String __repr__() const;

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
  void setAsBool(const String & key, Bool value);

  /** Method for setting information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value as an integer
   */
  void setAsUnsignedInteger(const String & key, UnsignedInteger value);

  /** Method for setting information into the resource map
   * @param key The name under which the value is stored in the ResourceMap
   * @param value The value as a double
   */
  void setAsScalar(const String & key, Scalar value);

  /** Is the specific key present ? */
  Bool hasKey(const String & key) const;
  
  /** Remove a key from the resource map */
  void removeKey(const String & key);

private:
  /** The actual map that stores the key/value pairs */
  typedef std::map< String, String > MapStringType;
  typedef std::map< String, Scalar > MapScalarType;
  typedef std::map< String, UnsignedInteger > MapUnsignedIntegerType;
  typedef std::map< String, Bool > MapBoolType;

  MapStringType mapString_;
  MapScalarType mapScalar_;
  MapUnsignedIntegerType mapUnsignedInteger_;
  MapBoolType mapBool_;

}; /* class ResourceMap */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_RESOURCEMAPBASE_HXX */
