//                                               -*- C++ -*-
/**
 *  @brief ResourceMapBase defines top-most resourceMap strategies
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

#include "openturns/OSS.hxx"
#include "openturns/ResourceMapBase.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Collection.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS

CLASSNAMEINIT(ResourceMapBase)

static const Factory<ResourceMapBase> Factory_ResourceMapBase;

/* Default constructor */
ResourceMapBase::ResourceMapBase()
  : mapString_()
  , mapScalar_()
  , mapUnsignedInteger_()
  , mapBool_()
{
  // Nothing to do
}

/* Virtual constructor */
ResourceMapBase * ResourceMapBase::clone() const
{
  return new ResourceMapBase(*this);
}

/* Method for retrieving information from the resource map */
String ResourceMapBase::getType(const String & key) const
{
  // First, try to retrieve the value from the String map
  {
    Collection<String> keys;
    for (MapStringType::const_iterator it = mapString_.begin(); it != mapString_.end(); ++it)
    {
      keys.add(it->first);
    }
    MapStringType::const_iterator it = mapString_.find(key);
    if (it != mapString_.end()) return "string";
  }
  // Second, try to retrieve the value from the Scalar map
  {
    Collection<String> keys;
    for (MapScalarType::const_iterator it = mapScalar_.begin(); it != mapScalar_.end(); ++it)
    {
      keys.add(it->first);
    }
    MapScalarType::const_iterator it = mapScalar_.find(key);
    if (it != mapScalar_.end()) return "float";
  }
  // Third, try to retrieve the value from the UnsignedInteger map
  {
    Collection<String> keys;
    for (MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.begin(); it != mapUnsignedInteger_.end(); ++it)
    {
      keys.add(it->first);
    }
    MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
    if (it != mapUnsignedInteger_.end()) return "unsigned int";
  }
  // Fourth, try to retrieve the value from the Bool map
  {
    Collection<String> keys;
    for (MapBoolType::const_iterator it = mapBool_.begin(); it != mapBool_.end(); ++it)
    {
      keys.add(it->first);
    }
    MapBoolType::const_iterator it = mapBool_.find(key);
    if (it != mapBool_.end()) return "bool";
  }
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMapBase. Report Bug";
}

String ResourceMapBase::get(const String & key) const
{
  // First, try to retrieve the value from the String map
  {
    MapStringType::const_iterator it = mapString_.find(key);
    if (it != mapString_.end()) return it->second;
  }
  // Second, try to retrieve the value from the Scalar map
  {
    MapScalarType::const_iterator it = mapScalar_.find(key);
    if (it != mapScalar_.end()) return (OSS() << it->second);
  }
  // Third, try to retrieve the value from the UnsignedInteger map
  {
    MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
    if (it != mapUnsignedInteger_.end()) return (OSS() << it->second);
  }
  // Fourth, try to retrieve the value from the Bool map
  {
    MapBoolType::const_iterator it = mapBool_.find(key);
    if (it != mapBool_.end()) return (OSS() << it->second);
  }
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMapBase. Report Bug";
}

Bool ResourceMapBase::hasKey(const String & key) const
{
  return ((mapString_.find(key) != mapString_.end())
      || (mapScalar_.find(key) != mapScalar_.end())
      || (mapUnsignedInteger_.find(key) != mapUnsignedInteger_.end())
      || (mapBool_.find(key) != mapBool_.end()));
}

String ResourceMapBase::getAsString(const String & key) const
{
  MapStringType::const_iterator it = mapString_.find(key);
  if (it != mapString_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMapBase as a String. Report Bug";
}

Scalar ResourceMapBase::getAsScalar(const String & key) const
{
  MapScalarType::const_iterator it = mapScalar_.find(key);
  if (it != mapScalar_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMapBase as a Scalar. Report Bug";
}

UnsignedInteger ResourceMapBase::getAsUnsignedInteger(const String & key) const
{
  MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
  if (it != mapUnsignedInteger_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMapBase as an UnsignedInteger. Report Bug";
}

Bool ResourceMapBase::getAsBool(const String & key) const
{
  MapBoolType::const_iterator it = mapBool_.find(key);
  if (it != mapBool_.end()) return it->second;
  throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMapBase as a Bool. Report Bug";
}

/* Get the size of the map */
UnsignedInteger ResourceMapBase::getSize() const
{
  return getStringSize() + getScalarSize() + getUnsignedIntegerSize() + getBoolSize();
}

UnsignedInteger ResourceMapBase::getStringSize() const
{
  return mapString_.size();
}

UnsignedInteger ResourceMapBase::getScalarSize() const
{
  return mapScalar_.size();
}

UnsignedInteger ResourceMapBase::getUnsignedIntegerSize() const
{
  return mapUnsignedInteger_.size();
}

UnsignedInteger ResourceMapBase::getBoolSize() const
{
  return mapBool_.size();
}

/* Method for setting information into the resource map */
void ResourceMapBase::set(const String & key, const String & value)
{
  // First, check if the given key is already defined in one of the maps
  // to convert it to the correct type
  // First, try to retrieve the value from the String map
  {
    MapStringType::const_iterator it = mapString_.find(key);
    if (it != mapString_.end())
    {
      mapString_[key] = value;
      return;
    }
  }
  // Second, try to retrieve the value from the Scalar map
  {
    MapScalarType::const_iterator it = mapScalar_.find(key);
    if (it != mapScalar_.end())
    {
      Scalar scalarValue = -1.0;
      std::istringstream iss( value );
      iss >> scalarValue;
      mapScalar_[ key ] = scalarValue;
      return;
    }
  }
  // Third, try to retrieve the value from the UnsignedInteger map
  {
    MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.find(key);
    if (it != mapUnsignedInteger_.end())
    {
      UnsignedInteger unsignedIntegerValue = 0;
      std::istringstream iss( value );
      iss >> unsignedIntegerValue;
      mapUnsignedInteger_[ key ] = unsignedIntegerValue;
      return;
    }
  }
  // Fourth, try to retrieve the value from the Bool map
  {
    MapBoolType::const_iterator it = mapBool_.find(key);
    if (it != mapBool_.end())
    {
      Bool boolValue = false;
      // First, try to recover the bool value from the "true" or "false" strings
      if (value == "true") boolValue = true;
      else if (value == "false") boolValue = false;
      // Second, try to recover the bool from the litteral value
      else
      {
        std::istringstream iss( value );
        iss >> boolValue;
      }
      mapBool_[ key ] = boolValue;
    }
  }
  // Second, set the new value as a string
  mapString_[ key ] = value;
}

void ResourceMapBase::setAsString(const String & key, const String & value)
{
  mapString_[ key ] = value;
}

void ResourceMapBase::setAsBool(const String & key, Bool value)
{
  mapBool_[ key ] = value;
}

void ResourceMapBase::setAsUnsignedInteger(const String & key, UnsignedInteger value)
{
  mapUnsignedInteger_[ key ] = value;
}

void ResourceMapBase::setAsScalar(const String & key, Scalar value)
{
  mapScalar_[ key ] = value;
}

/* String converter */
String ResourceMapBase::__repr__() const
{
  // First, build a global map to sort keys
  MapStringType globalMap(mapString_);
  for (MapScalarType::const_iterator it = mapScalar_.begin(); it != mapScalar_.end(); ++it)
    globalMap[it->first] = get(it->first);
  for (MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.begin(); it != mapUnsignedInteger_.end(); ++it)
    globalMap[it->first] = get(it->first);
  for (MapBoolType::const_iterator it = mapBool_.begin(); it != mapBool_.end(); ++it)
    globalMap[it->first] = get(it->first);
  OSS oss;
  oss << "ResourceMap={";
  const char * separator = "\n  ";
  for(MapStringType::const_iterator it = globalMap.begin(); it != globalMap.end(); ++it, separator = ",\n  ")
    oss << separator
        << it->first << " => " << it->second;
  oss << "\n}";
  return oss;
}

std::vector<String> ResourceMapBase::getKeys() const
{
  // Initialize with the string keys
  std::vector<String> keys(getStringKeys());
  // Append the scalar keys
  {
    std::vector<String> scalarKeys(getScalarKeys());
    keys.insert(keys.end(), scalarKeys.begin(), scalarKeys.end());
  }
  // Append the unsigned integer keys
  {
    std::vector<String> unsignedIntegerKeys(getUnsignedIntegerKeys());
    keys.insert(keys.end(), unsignedIntegerKeys.begin(), unsignedIntegerKeys.end());
  }
  // Append the bool keys
  {
    std::vector<String> boolKeys(getBoolKeys());
    keys.insert(keys.end(), boolKeys.begin(), boolKeys.end());
  }
  // Then sort the keys
  std::sort(keys.begin(), keys.end());
  return keys;
}

std::vector<String> ResourceMapBase::getStringKeys() const
{
  std::vector<String> keys;
  for(MapStringType::const_iterator it = mapString_.begin(); it != mapString_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

std::vector<String> ResourceMapBase::getBoolKeys() const
{
  std::vector<String> keys;
  for(MapBoolType::const_iterator it = mapBool_.begin(); it != mapBool_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

std::vector<String> ResourceMapBase::getScalarKeys() const
{
  std::vector<String> keys;
  for(MapScalarType::const_iterator it = mapScalar_.begin(); it != mapScalar_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

std::vector<String> ResourceMapBase::getUnsignedIntegerKeys() const
{
  std::vector<String> keys;
  for(MapUnsignedIntegerType::const_iterator it = mapUnsignedInteger_.begin(); it != mapUnsignedInteger_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

void ResourceMapBase::removeKey(const String & key)
{
  if (!hasKey(key))
    throw InternalException(HERE) << "Key '" << key << "' is missing in ResourceMap";
  
  String keyType(getType(key));
  if (keyType == "string")
    mapString_.erase(mapString_.find(key));
  if (keyType == "float")
    mapScalar_.erase(mapScalar_.find(key));
  if (keyType == "unsigned int")
    mapUnsignedInteger_.erase(mapUnsignedInteger_.find(key));
  if (keyType == "bool")
    mapBool_.erase(mapBool_.find(key));
}

END_NAMESPACE_OPENTURNS
