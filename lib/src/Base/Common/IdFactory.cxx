//                                               -*- C++ -*-
/**
 *  @brief The class IdFactory builds unique Ids for all Objects
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
#include <cstdlib>
#include "openturns/IdFactory.hxx"
#include "openturns/OSS.hxx"
#include "openturns/OTthread.hxx"
#include "openturns/MutexLock.hxx"

BEGIN_NAMESPACE_OPENTURNS


/* Id accessor */
Id IdFactory::BuildId()
{
  static AtomicInt IdFactory_NextId;
  return IdFactory_NextId.fetchAndAdd( 1 );
}

END_NAMESPACE_OPENTURNS
