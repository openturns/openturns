//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for multithreading
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
#ifndef OPENTURNS_TBB_HXX
#define OPENTURNS_TBB_HXX

#include <algorithm>
#include "openturns/OTprivate.hxx"
#include "openturns/OTconfig.hxx"

#ifdef OPENTURNS_HAVE_TBB
#ifdef OPENTURNS_TBB_NO_IMPLICIT_LINKAGE
# ifndef __TBB_NO_IMPLICIT_LINKAGE
#  define __TBB_NO_IMPLICIT_LINKAGE 1
# endif
# ifndef __TBBMALLOC_NO_IMPLICIT_LINKAGE
#  define __TBBMALLOC_NO_IMPLICIT_LINKAGE 1
# endif
#endif
#include <tbb/parallel_sort.h>
#include <tbb/parallel_reduce.h>
#include "openturns/OTwindows.h"
#else /* OPENTURNS_HAVE_TBB */
#include "openturns/Exception.hxx"
// We redefine some TBB elements to simulate TBB availability through the code
// Those redefinitions are single threaded
namespace tbb
{

template <typename T>
class blocked_range
{
public:
  typedef T value_type;
  typedef std::size_t size_type;
  blocked_range(value_type from, value_type to, size_type gs = 1)
    : from_(from), to_(to), grainSize_(gs)
  {
#ifdef DEBUG_BOUNDCHECKING
    if (from_ > to_)
      throw OT::InvalidArgumentException(HERE) << "Range is malformed (from > to) with from=" << from_
          << " and to=" << to;
    if (grainSize_ < 1)
      throw OT::InvalidArgumentException(HERE) << "Range is malformed (grainSize < 1) with grainSize=" << grainSize_;
#endif /* DEBUG_BOUNDCHECKING */
  }
  value_type begin() const
  {
    return from_;
  }
  value_type end() const
  {
    return to_;
  }
  size_type size() const
  {
    return size_t(to_ - from_);
  }
  size_type grainsize() const
  {
    return grainSize_;
  }
private:
  value_type from_;
  value_type to_;
  size_type grainSize_;
}; // end class blocked_range

template <typename RANGE, typename BODY>
void parallel_for( const RANGE & range, const BODY & body )
{
  body( range );
}

template <typename RANGE, typename BODY>
void parallel_reduce( const RANGE & range, BODY & body )
{
  body( range );
}

template <typename ITERATOR>
void parallel_sort( ITERATOR first, ITERATOR last )
{
  std::stable_sort( first, last );
}

} // namespace tbb
#endif /* OPENTURNS_HAVE_TBB */

BEGIN_NAMESPACE_OPENTURNS

// disables blas threading inside TBB parallel regions
class TBBContext
{
public:
  TBBContext();
  ~TBBContext();
private:
  int ompNumThreads_;
  int openblasNumThreads_;
};

class OT_API TBB
{
public:
#ifndef SWIG // swig fails to parse nested struct or template

#ifdef OPENTURNS_HAVE_TBB
  typedef tbb::split Split;
#else /* OPENTURNS_HAVE_TBB */
  struct Split {};
#endif /* OPENTURNS_HAVE_TBB */

  template <typename T>
  struct BlockedRange : public tbb::blocked_range<T>
  {
    typedef T value_type;
    typedef typename tbb::blocked_range<T>::size_type size_type;
    BlockedRange(value_type from, value_type to, size_type gs = 1) : tbb::blocked_range<T>(from, to, gs) {}
    BlockedRange(const tbb::blocked_range<T> & range) : tbb::blocked_range<T>(range) {}
  };
#endif // SWIG

  template <typename BODY>
  static inline
  void ParallelFor( UnsignedInteger from, UnsignedInteger to, const BODY & body, std::size_t gs = 1 )
  {
    TBBContext context;
    tbb::parallel_for( tbb::blocked_range<UnsignedInteger>( from, to, gs ), body );
  }

  template <typename BODY>
  static inline
  void ParallelReduce( UnsignedInteger from, UnsignedInteger to, BODY & body, std::size_t gs = 1)
  {
    TBBContext context;
    tbb::parallel_reduce( tbb::blocked_range<UnsignedInteger>( from, to, gs ), body );
  }

  template <typename ITERATOR>
  static inline
  void ParallelSort( ITERATOR first, ITERATOR last )
  {
    TBBContext context;
    tbb::parallel_sort( first, last );
  }

  /* Whether TBB is available */
  static Bool IsAvailable();

  /* Enable/disable */
  static void Enable();
  static void Disable();

  /* Accessor to the maximum number of threads */
  static void SetNumberOfThreads(const UnsignedInteger numberOfThreads);
  static UnsignedInteger GetNumberOfThreads();

private:
  friend struct TBB_init;

}; /* end class TBB */

/** This struct initializes all static members of TBB */
struct OT_API TBB_init
{
  TBB_init();
  ~TBB_init();
}; /* end class TBB_init */

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TBB_HXX */
