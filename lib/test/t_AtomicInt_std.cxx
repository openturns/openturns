//                                               -*- C++ -*-
/**
 *  @brief The test file of class AtomicInt for standard methods
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
#ifndef _WIN32
#include <unistd.h> // sysconf
#endif
#include <stdlib.h> // getenv
#include <thread>
#include "openturns/OT.hxx"
#include "openturns/OTtestcode.hxx"

using namespace OT;
using namespace OT::Test;

#define NB_ITER 1000000

void threadFunc(void* arg)
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
}


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  try
  {
    AtomicInt atom;

#ifndef _WIN32
    unsigned int nbThreads = sysconf(_SC_NPROCESSORS_CONF);
#else
    std::istringstream converter(getenv("NUMBER_OF_PROCESSORS"));
    unsigned int nbThreads;
    if (!(converter >> nbThreads))
    {
      throw TestFailed("OT::AtomicInt wrong nb of thread!");
    }
#endif
    ++ nbThreads;
    std::thread * threads = new std::thread[nbThreads];

    for (unsigned int i = 0; i < nbThreads; ++ i)
    {
      try
      {
        threads[i] = std::thread(threadFunc, (void*) &atom);
      }
      catch (const std::exception &)
      {
        throw TestFailed("OT::AtomicInt Threads creation failed!");
      }
    }

    for (unsigned int i = 0; i < nbThreads; ++ i)
    {
      try
      {
        threads[i].join();
      }
      catch (const std::exception &)
      {
        throw TestFailed("OT::AtomicInt Threads join failed!");
      }

    }
    delete [] threads;
    unsigned int shaked = atom.fetchAndAdd(10);
    if (shaked != (NB_ITER * nbThreads))
    {
      OSS errorMessage;
      errorMessage << "OT::AtomicInt does NOT return the correct value (value="
                   << shaked << " should be " << NB_ITER * nbThreads << ").";
      throw TestFailed(errorMessage);
    }

  }
  catch (const TestFailed & ex)
  {
    std::cerr << ex << std::endl;
    return ExitCode::Error;
  }
  return ExitCode::Success;
}
