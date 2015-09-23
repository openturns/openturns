//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for multithreading
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
#include <cassert>
#include <cstdlib>
#include <errno.h>

#include "OTconfig.hxx"
#include "OTthread.hxx"

#include "TBB.hxx"
#include "ResourceMap.hxx"


BEGIN_NAMESPACE_OPENTURNS

static pthread_mutex_t TBB_InstanceMutex_;
static TBB * TBB_P_instance_ = 0;
static const TBB_init initializer_TBB;

#ifdef OPENTURNS_HAVE_TBB
tbb::task_scheduler_init * TBB_P_scheduler_ = 0;
#endif /* OPENTURNS_HAVE_TBB */


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
#ifdef OPENTURNS_HAVE_TBB
  UnsignedInteger nbThreads = ResourceMap::GetAsUnsignedInteger( "parallel-threads" );
  TBB_P_scheduler_ = new tbb::task_scheduler_init( nbThreads );
#endif /* OPENTURNS_HAVE_TBB */
}

TBB_init::~TBB_init()
{
  delete TBB_P_instance_;
  TBB_P_instance_ = 0;
  pthread_mutex_destroy( &TBB_InstanceMutex_);
#ifdef OPENTURNS_HAVE_TBB
  delete TBB_P_scheduler_;
  TBB_P_scheduler_ = 0;
#endif /* OPENTURNS_HAVE_TBB */
}


END_NAMESPACE_OPENTURNS
