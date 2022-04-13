//                                               -*- C++ -*-
/**
 *  @brief The test file of class Log for standard methods
 *
 *  Copyright 2005-2022 Airbus-EDF-IMACS-ONERA-Phimeca
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
#include "openturns/OTtestcode.hxx"
#include "openturns/OStream.hxx"

#include "openturns/Log.hxx"

using namespace OT;
using namespace OT::Test;

const int N = 100;

void* printDebug(void*)
{
  for(int i = 0; i < N; i++) LOGDEBUG(OSS() << "Debug #" << i);
  return NULL;
}


void* printInfo(void*)
{
  for(int i = 0; i < N; i++) LOGINFO(OSS() << "Info #" << i);
  return NULL;
}


void* printUser(void*)
{
  for(int i = 0; i < N; i++) LOGUSER(OSS() << "User #" << i);
  return NULL;
}


void* printWarn(void*)
{
  for(int i = 0; i < N; i++) LOGWARN(OSS() << "Warn #" << i);
  return NULL;
}


void* printError(void*)
{
  for(int i = 0; i < N; i++) LOGERROR(OSS() << "Error #" << i);
  return NULL;
}


void* printTrace(void*)
{
  for(int i = 0; i < N; i++) LOGTRACE(OSS() << "Trace #" << i);
  return NULL;
}


int main(int, char *[])
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  ExitCodeValue rc = ExitCode::Success;

  const int nbThreads = 128;
  pthread_t Threads[nbThreads];

  const int nbFunctions = 6;
  typedef void * (*FUNC) (void *);
  FUNC Functions[nbFunctions] = { printDebug, printInfo, printUser, printWarn, printError, printTrace };

  Log::Show( Log::ALL );
  Log::SetFile("Log_check.log");

  // Create a thread for messages of every level
  for (int i = 0; i < nbThreads; ++i)
    pthread_create( Threads + i, 0, Functions[i % nbFunctions], 0 );

  // Wait for threads of messages
  for (int i = 0; i < nbThreads; ++i)
    pthread_join( Threads[i], 0 );

  // const UnsignedInteger expectedLength = 4 * N;
  // UnsignedInteger actualLength = Log::GetInstance().getSize();

  // if (actualLength != expectedLength) rc = ExitCode::Error;

  return rc;
}
