//                                               -*- C++ -*-
/**
 *  @brief The test file of class AtomicInt for standard methods
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
#include <unistd.h> // sysconf
#include <stdlib.h> // getenv
#ifndef _MSC_VER
# include <signal.h>
# include <pthread.h>
#endif

#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

#define NB_ITER 1000000

void* threadFunc(void* arg)
{
  //
  AtomicInt * atom = (AtomicInt*) arg;

  int i;
  for( i = 0; i < NB_ITER; i++ )
  {
    atom->increment();
    atom->decrement();
    atom->fetchAndAdd(1);
  }

  return NULL;
}


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    AtomicInt atom;

#ifndef WIN32
    unsigned int nbThreads = sysconf(_SC_NPROCESSORS_CONF);
#else
    std::istringstream converter(getenv("NUMBER_OF_PROCESSORS"));
    unsigned int nbThreads;
    if (!(converter >> nbThreads))
    {
      OSS errorMessage;
      errorMessage << "OT::AtomicInt wrong nb of thread!";
      throw TestFailed(errorMessage);
    }
#endif
    nbThreads++;
    pthread_t *threadsIds = new pthread_t[nbThreads];

    unsigned int i;
    for( i = 0; i < nbThreads; ++i )
    {
      int rc = pthread_create( threadsIds + i, NULL,
                               threadFunc,
                               (void*) &atom );
      if (rc != 0)
      {
#ifndef _MSC_VER
        int j;
        for( j = i - 1 ; j >= 0 ; --j )
          pthread_kill( threadsIds[j], SIGTERM );
#endif
        OSS errorMessage;
        errorMessage << "OT::AtomicInt Threads creation failed!";
        throw TestFailed(errorMessage);
      }
    } /* end for */

    for( i = 0; i < nbThreads; ++i )
    {
      int rc = pthread_join( threadsIds[i], NULL );
      if (rc != 0)
      {
        OSS errorMessage;
        errorMessage << "OT::AtomicInt Threads join failed!";
        throw TestFailed(errorMessage);
      }
    } /* end for */
    delete [] threadsIds;

    unsigned int shaked = atom.fetchAndAdd(10);
    if (shaked != (NB_ITER * nbThreads))
    {
      OSS errorMessage;
      errorMessage << "OT::AtomicInt does NOT return the correct value (value="
                   << shaked << " should be " << NB_ITER * nbThreads << ").";
      throw TestFailed(errorMessage);
    }

  }
  catch (TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }


  return ExitCode::Success;
}
