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

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <errno.h>

#include "openturns/OTconfig.hxx"
#include "openturns/OTthread.hxx"

#include "openturns/TBB.hxx"
#include "openturns/ResourceMap.hxx"


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
#else
  (void)numberOfThreads;
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
