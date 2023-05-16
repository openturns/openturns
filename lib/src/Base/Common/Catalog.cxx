//                                               -*- C++ -*-
/**
 *  @brief Catalog records all user information to a file or tty
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <errno.h>
#include "openturns/OSS.hxx"
#include "openturns/Catalog.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Log.hxx"
#include "openturns/MutexLock.hxx"
#include "openturns/PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS


static std::mutex Catalog_InstanceMutex_;
static Catalog * Catalog_P_instance_ = 0;
static const Catalog_init static_initializer_Catalog;

Catalog_init::Catalog_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]()
  {
    Catalog_P_instance_ = new Catalog;
  });
  assert(Catalog_P_instance_);
}

Catalog_init::~Catalog_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]()
  {
    delete Catalog_P_instance_;
    Catalog_P_instance_ = 0;
  });
}


/* Constructor */
Catalog::Catalog()
  : catalog_()
{
  // Do nothing
}


/* Destructor */
Catalog::~Catalog()
{
  CatalogType::iterator it;
  for(it = catalog_.begin(); it != catalog_.end(); ++it)
  {
    delete it->second;
  }
}


template<>
MutexLockSingleton<Catalog>::MutexLockSingleton(Catalog & singleton)
  : singleton_(singleton)
  , lock_(Catalog_InstanceMutex_) {}


MutexLockSingleton<Catalog> Catalog::GetInstance()
{
  static const Catalog_init force_instantiation;
  // Catalog_InstanceMutex_ is always initialized
  return *Catalog_P_instance_;
}

/* Get the list of keys */
std::vector<String> Catalog::GetKeys()
{
  return GetInstance().lock().getKeys();
}


/* Get the factory from its name */
const PersistentObjectFactory & Catalog::Get(const String & factoryName)
{
  return GetInstance().lock().get( factoryName );
}

/* Get the factory from its name */
const PersistentObjectFactory & Catalog::get(const String & factoryName) const
{
  CatalogType::const_iterator it = catalog_.find( factoryName );
  if (it == catalog_.end())
    throw InvalidArgumentException(HERE) << "No factory named '" << factoryName << "' in Catalog";

  return *(it->second);
}


/* Get the list of keys */
std::vector<String> Catalog::getKeys() const
{
  std::vector<String> keys;
  for(CatalogType::const_iterator it = catalog_.begin(); it != catalog_.end(); ++it)
  {
    keys.push_back(it->first);
  }
  return keys;
}

/* Add a new factory to the catalog */
void Catalog::Add(const String & factoryName, const PersistentObjectFactory * p_factory)
{
  GetInstance().lock().add( factoryName, p_factory );
}

/* Add a new factory to the catalog */
void Catalog::add(const String & factoryName, const PersistentObjectFactory * p_factory)
{
  if (catalog_.find( factoryName ) == catalog_.end())
  {
    catalog_[ factoryName ] = p_factory->clone();
  }
}

/* String converter */
String Catalog::__repr__() const
{
  OSS oss;
  oss << "Catalog={";
  const char * separator = "\n  ";
  for(CatalogType::const_iterator it = catalog_.begin(); it != catalog_.end(); ++it, separator = ",\n  ")
    oss << separator
        << it->first;
  oss << "\n}";
  return oss;
}


END_NAMESPACE_OPENTURNS
