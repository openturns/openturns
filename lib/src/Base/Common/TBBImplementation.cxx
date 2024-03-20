//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for multithreading
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include <mutex>

#include "openturns/OTconfig.hxx"

#include "openturns/TBBImplementation.hxx"
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

static TBBImplementation * TBBImplementation_P_instance_ = 0;
static const TBB_init initializer_TBBImplementation;
UnsignedInteger TBBImplementation::ThreadsNumber_ = 1;
tbb::task_arena * TBBImplementation::P_task_arena_ = 0;

void TBBImplementation::SetThreadsNumber(const UnsignedInteger threadNumber)
{
  if (!threadNumber)
    throw InvalidArgumentException(HERE) << "Number of threads must be positive";
  ThreadsNumber_ = threadNumber;
  delete P_task_arena_;
  P_task_arena_ = new tbb::task_arena(ThreadsNumber_);
}

UnsignedInteger TBBImplementation::GetThreadsNumber()
{
  return ThreadsNumber_;
}


UnsignedInteger TBBImplementation::GetThreadIndex()
{
#ifdef OPENTURNS_HAVE_TBB
  return tbb::this_task_arena::current_thread_index();
#else
  return 0;
#endif
}


void TBBImplementation::Enable()
{
  const UnsignedInteger nbThreads = ResourceMap::GetAsUnsignedInteger("TBB-ThreadsNumber");
  SetThreadsNumber(nbThreads);
}


void TBBImplementation::Disable()
{
  SetThreadsNumber(1);
}



TBB_init::TBB_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]()
  {
    TBBImplementation_P_instance_ = new TBBImplementation;
    TBBImplementation::Enable();
  });
}

TBB_init::~TBB_init()
{
  static std::once_flag flag;
  std::call_once(flag, [&]()
  {
    delete TBBImplementation_P_instance_;
    TBBImplementation_P_instance_ = 0;
    delete TBBImplementation::P_task_arena_;
    TBBImplementation::P_task_arena_ = 0;
  });
}


TBBContext::TBBContext()
  : ompNumThreads_(0)
  , openblasNumThreads_(0)
{
#ifdef OPENTURNS_HAVE_TBB
  if (TBBImplementation::GetThreadsNumber() > 1)
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
  if (TBBImplementation::GetThreadsNumber() > 1)
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
