//                                               -*- C++ -*-
/**
 *  @brief The class MutexLock manages the lock/unlock of thread mutexes
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

#include <mutex>
#include <cstdlib> // for exit(3)
#include <iostream>
#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS

class OT_API MutexLock
{
public:
  MutexLock (std::mutex & mtx)
  : mtx_(mtx)
  {
    try
    {
      lock_ = new std::unique_lock<std::mutex>(mtx, std::defer_lock);
    }
    catch (const std::exception & ex)
    {
      std::cerr << "(MutexLock ctor) err=" << ex.what() << std::endl;
      exit(1);
    }
  }

  MutexLock (const MutexLock & other)
  : mtx_(other.mtx_)
  {
    try
    {
      lock_ = new std::unique_lock<std::mutex>(mtx_, std::defer_lock);
    }
    catch (const std::exception & ex)
    {
      std::cerr << "(MutexLock copy ctor) err=" << ex.what() << std::endl;
      exit(1);
    }
  }

  MutexLock & operator=(const MutexLock & other) = delete;

  virtual ~MutexLock()
  {
    try
    {
      delete lock_;
    }
    catch (const std::exception & ex)
    {
      std::cerr << "(MutexLock dtor) err=" << ex.what() << std::endl;
      exit(1);
    }
  }

private:
  std::mutex & mtx_;
  std::unique_lock<std::mutex> * lock_ = nullptr;

}; /* class MutexLock */

template<class T>
class MutexLockSingleton
{
  T & singleton_;
  MutexLock lock_;

public:
  // Default constructor, defined by client classes
  MutexLockSingleton (T & singleton);

  // Default copy-constructor
  MutexLockSingleton (const MutexLockSingleton<T> & other)
  : singleton_(other.singleton_), lock_(other.lock_) {}

private:
  // Disable copy-assignment
  MutexLockSingleton& operator=(const MutexLockSingleton<T> & other);

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
