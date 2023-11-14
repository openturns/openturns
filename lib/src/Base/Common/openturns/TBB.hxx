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
#ifndef OPENTURNS_TBB_HXX
#define OPENTURNS_TBB_HXX

#include "openturns/Exception.hxx"

BEGIN_NAMESPACE_OPENTURNS


class OT_API TBB
{
public:
  /* Enable/disable */
  static void Enable();
  static void Disable();

  /* Accessor to the maximum number of threads */
  static void SetThreadsNumber(const UnsignedInteger threadsNumber);
  static UnsignedInteger GetThreadsNumber();

};

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_TBB_HXX */
