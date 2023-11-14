//                                               -*- C -*-
/**
 *  @brief This file contains a single C function to allow m4 easy detection
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
#include "openturns/OTdebug.h"

/*
 * @fn     int openturns_library_ok()
 * @brief  A function that return 1 on success
 * @return The return code
 */
int OT_API openturns_library_ok(void);

int OT_API openturns_library_ok(void)
{
  return 1;
}
