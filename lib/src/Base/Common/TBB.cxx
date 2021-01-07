//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for multithreading
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <cassert>
#include <cstdlib>
#include <errno.h>

#include "openturns/OTconfig.hxx"
#include "openturns/OTthread.hxx"

#include "openturns/TBB.hxx"
#include "openturns/ResourceMap.hxx"

#ifdef OPENTURNS_HAVE_OPENMP
#include <omp.h>
#endif

#ifdef OPENTURNS_HAVE_OPENBLAS
extern "C" {
// This function is private
  int goto_get_num_procs(void);
// This one is public but redeclare it in case regular cblas headers are used
  void openblas_set_num_threads(int num_threads);
}
#endif

BEGIN_NAMESPACE_OPENTURNS

static pthread_mutex_t TBB_InstanceMutex_;
static TBB * TBB_P_instance_ = 0;
static const TBB_init initializer_TBB;
UnsignedInteger TBB::NumberOfThreads_ = 1;
tbb::task_arena * TBB::P_task_arena_ = 0;

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
  if (!numberOfThreads)
    throw InvalidArgumentException(HERE) << "Number of threads must be positive";
  NumberOfThreads_ = numberOfThreads;
  delete P_task_arena_;
  P_task_arena_ = new tbb::task_arena(NumberOfThreads_);
}

UnsignedInteger TBB::GetNumberOfThreads()
{
  return NumberOfThreads_;
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
  delete TBB::P_task_arena_;
  TBB::P_task_arena_ = 0;
}


TBBContext::TBBContext()
  : ompNumThreads_(0)
  , openblasNumThreads_(0)
{
#ifdef OPENTURNS_HAVE_TBB
  if (TBB::GetNumberOfThreads() > 1)
  {
    // disable threading
#ifdef OPENTURNS_HAVE_OPENMP
    ompNumThreads_ = omp_get_max_threads();
    omp_set_num_threads(1);
#endif
#ifdef OPENTURNS_HAVE_OPENBLAS
    openblasNumThreads_ = goto_get_num_procs();
    openblas_set_num_threads(1);
#endif
  }
#endif
  (void) ompNumThreads_;
  (void) openblasNumThreads_;
}

TBBContext::~TBBContext()
{
#ifdef OPENTURNS_HAVE_TBB
  if (TBB::GetNumberOfThreads() > 1)
  {
    // restore threading
#ifdef OPENTURNS_HAVE_OPENMP
    omp_set_num_threads(ompNumThreads_);
#endif
#ifdef OPENTURNS_HAVE_OPENBLAS
    openblas_set_num_threads(openblasNumThreads_);
#endif
  }
#endif
}

END_NAMESPACE_OPENTURNS
