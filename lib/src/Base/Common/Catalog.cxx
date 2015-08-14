//                                               -*- C++ -*-
/**
 *  @brief Catalog records all user information to a file or tty
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <errno.h>
#include "OSS.hxx"
#include "Catalog.hxx"
#include "Exception.hxx"
#include "Log.hxx"
#include "MutexLock.hxx"
#include "PersistentObjectFactory.hxx"

BEGIN_NAMESPACE_OPENTURNS




static pthread_mutex_t Catalog_InstanceMutex_;
static Catalog * Catalog_P_instance_ = 0;
static pthread_once_t Catalog_InstanceMutex_once = PTHREAD_ONCE_INIT;

Catalog_init::Catalog_init()
{
  int rc = pthread_once( &Catalog_InstanceMutex_once, Catalog::Initialize );
  if (rc != 0)
  {
    perror("Catalog_init::Log_init once Initialization failed");
    exit(1);
  }
}

Catalog_init::~Catalog_init()
{
  Catalog::Release();
}


/* Constructor */
Catalog::Catalog()
  : catalog_()
{
  // Registration of destructor at exit
  // std::atexit( Catalog::Release );
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


/* Return the catalog as a singleton */
Catalog & Catalog::GetInstance()
{
  if (!Catalog_P_instance_)
  {
    Catalog_P_instance_ = new Catalog;
    assert(Catalog_P_instance_);
  }
  return *Catalog_P_instance_;
}


void Catalog::Initialize()
{
#ifndef OT_MUTEXINIT_NOCHECK
  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_NORMAL );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
  int rc = pthread_mutex_init( &Catalog_InstanceMutex_, &attr );
  if (rc != 0)
  {
    perror("Catalog::Initialize mutex initialization failed");
    exit(1);
  }
#else
  pthread_mutex_init( &Catalog_InstanceMutex_, NULL );
#endif
  Catalog_P_instance_ = new Catalog;
  assert(Catalog_P_instance_);
}



void Catalog::Release()
{
  delete Catalog_P_instance_;
  Catalog_P_instance_ = 0;
}


/* Get the factory from its name */
const PersistentObjectFactory & Catalog::Get(const String & factoryName)
{
  MutexLock lock( Catalog_InstanceMutex_ );
  return GetInstance().get( factoryName );
}

/* Get the factory from its name */
const PersistentObjectFactory & Catalog::get(const String & factoryName) const
{
  CatalogType::const_iterator it = catalog_.find( factoryName );
  if (it == catalog_.end())
    throw InvalidArgumentException(HERE) << "No factory named '" << factoryName << "' in Catalog";

  return *(it->second);
}


/* Add a new factory to the catalog */
void Catalog::Add(const String & factoryName, const PersistentObjectFactory * p_factory)
{
  MutexLock lock( Catalog_InstanceMutex_ );
  GetInstance().add( factoryName, p_factory );
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
