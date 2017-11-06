//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for multithreading
 *
 *  Copyright 2005-2017 Airbus-EDF-IMACS-Phimeca
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
#include <cassert>
#include <cstdlib>
#include <errno.h>

#include "openturns/OTconfig.hxx"
#include "openturns/OTthread.hxx"

#include "openturns/TBB.hxx"
#include "openturns/ResourceMap.hxx"

#if defined(HAVE_OPENBLAS_GET_NUM_THREADS) && defined(HAVE_OPENBLAS_SET_NUM_THREADS)
// CMakeLists.txt checks that these functions can be linked against, so declare them.
// We do not check for cblas.h, so we cannot include it.
extern "C" {
int  openblas_get_num_threads(void);
void  openblas_set_num_threads(int);
}
#endif

BEGIN_NAMESPACE_OPENTURNS

static pthread_mutex_t TBB_InstanceMutex_;
static TBB * TBB_P_instance_ = 0;
static const TBB_init initializer_TBB;

#ifdef OPENTURNS_HAVE_TBB
tbb::task_scheduler_init * TBB_P_scheduler_ = 0;
#endif /* OPENTURNS_HAVE_TBB */


Bool TBB::IsAvailable()
{
#ifdef OPENTURNS_HAVE_TBB
  return true;
#else
  return false;
#endif
}

void TBB::SetNumberOfThreads(const UnsignedInteger numberOfThreads)
{
#ifdef OPENTURNS_HAVE_TBB
  delete TBB_P_scheduler_;
  TBB_P_scheduler_ = new tbb::task_scheduler_init(numberOfThreads);
#endif
}

void TBB::Enable()
{
  const UnsignedInteger nbThreads = ResourceMap::GetAsUnsignedInteger("parallel-threads");
  SetNumberOfThreads(nbThreads);
}


void TBB::Disable()
{
  SetNumberOfThreads(1);
}

UnsignedInteger TBB::DisableThreadedBlas()
{
    UnsignedInteger numThreads = 1;
#if defined(HAVE_OPENBLAS_GET_NUM_THREADS) && defined(HAVE_OPENBLAS_SET_NUM_THREADS)
    numThreads = openblas_get_num_threads();
    openblas_set_num_threads(1);
#elif defined(_OPENMP) && defined(HAVE_OMP_GET_MAX_THREADS) && defined(HAVE_OMP_SET_NUM_THREADS)
    numThreads = omp_get_max_threads();
    omp_set_num_threads(1);
#endif
    return numThreads;
}

void
TBB::EnableThreadedBlas(UnsignedInteger n)
{
#if defined(HAVE_OPENBLAS_GET_NUM_THREADS) && defined(HAVE_OPENBLAS_SET_NUM_THREADS)
    openblas_set_num_threads(n);
#elif defined(_OPENMP) && defined(HAVE_OMP_GET_MAX_THREADS) && defined(HAVE_OMP_SET_NUM_THREADS)
    omp_set_num_threads(n);
#endif
    (void) n;
}

TBB_init::TBB_init()
{
  if (!TBB_P_instance_)
  {
#ifndef OT_MUTEXINIT_NOCHECK
    pthread_mutexattr_t attr;
    pthread_mutexattr_init( &attr );
    pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
    pthread_mutex_init( &TBB_InstanceMutex_, &attr );
#else
    pthread_mutex_init( &TBB_InstanceMutex_, NULL );
#endif
    TBB_P_instance_ = new TBB;
  }
  TBB::Enable();
}

TBB_init::~TBB_init()
{
  if (TBB_P_instance_)
    pthread_mutex_destroy(&TBB_InstanceMutex_);
  delete TBB_P_instance_;
  TBB_P_instance_ = 0;
#ifdef OPENTURNS_HAVE_TBB
  delete TBB_P_scheduler_;
  TBB_P_scheduler_ = 0;
#endif /* OPENTURNS_HAVE_TBB */
}


END_NAMESPACE_OPENTURNS
