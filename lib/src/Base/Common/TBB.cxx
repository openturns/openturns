//                                               -*- C++ -*-
/**
 *  @brief This file supplies support for multithreading
 *
 *  Copyright 2005-2023 Airbus-EDF-IMACS-ONERA-Phimeca
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

#include "openturns/TBB.hxx"
#include "openturns/TBBImplementation.hxx"

BEGIN_NAMESPACE_OPENTURNS

void TBB::SetThreadsNumber(const UnsignedInteger threadNumber)
{
  TBBImplementation::SetThreadsNumber(threadNumber);
}

UnsignedInteger TBB::GetThreadsNumber()
{
  return TBBImplementation::GetThreadsNumber();
}

void TBB::Enable()
{
  TBBImplementation::Enable();
}


void TBB::Disable()
{
  TBBImplementation::Disable();
}

END_NAMESPACE_OPENTURNS
