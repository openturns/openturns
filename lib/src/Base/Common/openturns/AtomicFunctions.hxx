//                                               -*- C++ -*-
/**
 *  @brief This file supplies some atomic functions to support multithreading
 *
 *  Copyright 2005-2019 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_ATOMICFUNCTIONS_HXX
#define OPENTURNS_ATOMICFUNCTIONS_HXX

#include <cassert>
#include "openturns/OTconfig.hxx"
#include "openturns/MutexLock.hxx"

#if __cplusplus > 199711L
#include <atomic>
#elif defined(OPENTURNS_HAVE_TBB)
#include <tbb/tbb.h>
#include "openturns/OTwindows.h"
#endif


BEGIN_NAMESPACE_OPENTURNS

struct OT_API Atomic
{
#if !defined(OPENTURNS_HAVE_SYNC_BUILTINS)
  static pthread_mutex_t Atomic_Mutex_;
#endif


  static inline
  int FetchAndAdd( int * p, int d )
  {
#if defined(OPENTURNS_HAVE_SYNC_BUILTINS)
    return __sync_fetch_and_add( p, d );
#elif defined(_WIN32)
    return InterlockedExchangeAdd( (LONG *)p, d );
#elif defined(__i386__)
    int result;
    __asm__ __volatile__ (
      "lock\n\t"
      "xadd %1, %0"
      : "=m" (*p), "=r" (result)
      : "m"  (*p), "1"  (d)
      : "memory", "cc"
    );
    return result;
#else
    MutexLock lock( Atomic_Mutex_ );
    int result( *p );
    *p += d;
    return result;
#endif
  }


  static inline
  void Increment( int * p )
  {
#if defined(OPENTURNS_HAVE_SYNC_BUILTINS)
    __sync_fetch_and_add( p, 1 );
#elif defined(_WIN32)
    InterlockedIncrement( (LONG *)p );
#elif defined(__i386__)
    __asm__ (
      "lock\n\t"
      "incl %0"
      : "=m" (*p)
      : "m"  (*p)
      : "cc"
    );
#else
    MutexLock lock( Atomic_Mutex_ );
    ++ *p;
#endif
  }


  static inline
  void Decrement( int * p )
  {
#if defined(OPENTURNS_HAVE_SYNC_BUILTINS)
    __sync_fetch_and_sub( p, 1 );
#elif defined(_WIN32)
    InterlockedDecrement( (LONG *)p );
#elif defined(__i386__)
    __asm__ (
      "lock\n\t"
      "decl %0"
      : "=m" (*p)
      : "m"  (*p)
      : "cc"
    );
#else
    MutexLock lock( Atomic_Mutex_ );
    -- *p;
#endif
  }


  static inline
  int OrAndFetch( int * p, int d )
  {
#if defined(OPENTURNS_HAVE_SYNC_BUILTINS)
    return __sync_or_and_fetch( p, d );
#else // TODO: windows ? i386 ?
    MutexLock lock( Atomic_Mutex_ );
    *p |= d;
    int result( *p );
    return result;
#endif
  }


  static inline
  void Reset( int * p )
  {
#if defined(OPENTURNS_HAVE_SYNC_BUILTINS)
    __sync_and_and_fetch( p, 0x00 );
#elif defined(_WIN32)
    InterlockedExchange ( (LONG *)p, 0x00 );
#else // TODO: i386 ?
    MutexLock lock( Atomic_Mutex_ );
    *p = 0;
#endif
  }

}; /* end struct Atomic */


#if __cplusplus > 199711L

class OT_API AtomicInt
{

  typedef std::atomic<int> Integer;
  Integer val_;

public:

  AtomicInt(int v = 0) : val_(v)
  {}

  // marked as delete in std::atomic
  AtomicInt(const AtomicInt & other) : val_(other.val_.load())
  {}

  inline
  AtomicInt & operator = (int v)
  {
    val_ = v;
    return *this;
  }

  // marked as delete in std::atomic
  inline
  AtomicInt & operator = (const AtomicInt & other)
  {
    if (this != &other)
      val_ = other.val_.load();
    return *this;
  }

  // Get p value, increment it by d and return the old value
  inline
  int fetchAndAdd( int d )
  {
    return val_.fetch_add(d);
  }

  inline
  void increment()
  {
    ++ val_;
  }

  inline
  void decrement()
  {
    -- val_;
  }

  inline
  int get() const
  {
    return val_.load();
  }

}; // AtomicInt

#elif defined(OPENTURNS_HAVE_TBB)

class OT_API AtomicInt
{

  typedef tbb::atomic<int> Integer;
  Integer val_;

public:

  AtomicInt(int v = 0) : val_()
  {
    val_ = v;
  }

  inline
  AtomicInt & operator = ( int v )
  {
    val_ = v;
    return *this;
  }

  // Get p value, increment it by d and return the old value
  inline
  int fetchAndAdd( int d )
  {
    return val_.fetch_and_add( d );
  }

  inline
  void increment()
  {
    val_.fetch_and_add( 1 );
  }

  inline
  void decrement()
  {
    val_.fetch_and_add( -1 );
  }

  inline
  int get() const
  {
    return val_;
  }

}; /* end class AtomicInt */

#else

class OT_API AtomicInt
{
  typedef int Integer;
  Integer val_;

public:

  AtomicInt(int v = 0)
  {
    Atomic::Reset( & val_ );
    Atomic::FetchAndAdd( & val_, v );
  }

  inline
  AtomicInt & operator = ( int v )
  {
    Atomic::Reset( & val_ );
    Atomic::FetchAndAdd( & val_, v );
    return *this;
  }

  // Get p value, increment it by d and return the old value
  inline
  int fetchAndAdd( int d )
  {
    return Atomic::FetchAndAdd( & val_, d );
  }

  inline
  void increment()
  {
    Atomic::Increment( & val_ );
  }

  inline
  void decrement()
  {
    Atomic::Decrement( & val_ );
  }

  inline
  int get() const
  {
    return val_;
  }

}; /* end class AtomicInt */

#endif


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_ATOMICFUNCTIONS_HXX */
