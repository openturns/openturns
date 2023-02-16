//                                               -*- C++ -*-
/**
 *  @brief The class MutexLock manages the lock/unlock of Pthread mutexes
 *         This file is intended to be only include in .cxx files (avoid .hxx)
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
#ifndef OPENTURNS_MUTEXLOCK_HXX
#define OPENTURNS_MUTEXLOCK_HXX

#include <cerrno>  // for errno(3)
#include <cstdio>  // for perror(3)
#include <cstdlib> // for exit(3)
#include <cstring> // for strerror(3)
#include "openturns/OTprivate.hxx"
#include "openturns/OTthread.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API MutexLock
{
  pthread_mutex_t & mtx_;
public:
  MutexLock( pthread_mutex_t & mtx )
    : mtx_(mtx)
  {
    int rc = pthread_mutex_lock( &mtx_ );
    if (rc != 0)
    {
      fprintf( stderr, "(MutexLock ctor) rc=%d : %s\n", rc, strerror(rc) );
      exit(1);
    }
  }

  MutexLock( const MutexLock & other )
    : mtx_(other.mtx_)
  {
    int rc = pthread_mutex_lock( &mtx_ );
    if (rc != 0)
    {
      fprintf( stderr, "(MutexLock copy ctor) rc=%d : %s\n", rc, strerror(rc) );
      exit(1);
    }
  }

  virtual ~MutexLock()
  {
    int rc = pthread_mutex_unlock( &mtx_ );
    if (rc != 0)
    {
      fprintf( stderr, "(MutexLock dtor) rc=%d : %s\n", rc, strerror(rc) );
      exit(1);
    }
  }

}; /* class MutexLock */

template<class T>
class MutexLockSingleton
{
  T & singleton_;
  MutexLock lock_;

public:
  // Default constructor, defined by client classes
  MutexLockSingleton(T & singleton);
  // Default copy-constructor
  MutexLockSingleton( const MutexLockSingleton<T> & other ) : singleton_(other.singleton_), lock_(other.lock_) {}

private:
  // Disable copy-assignment
  MutexLockSingleton& operator=( const MutexLockSingleton<T> & other );

public:
  /** @copydoc Object::__repr__() const */
  String __repr__() const
  {
    return singleton_.__repr__();
  }

#ifndef SWIG
  T & lock()
  {
    return singleton_;
  }
  const T & lock() const
  {
    return singleton_;
  }
#endif

}; /* class MutexLockSingleton */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_MUTEXLOCK_HXX */
