//                                               -*- C++ -*-
/**
 *  @brief The main program. The entry point of the project...
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

#include "openturns/OT.hxx"
extern "C" int openturns_library_ok();

/*
 * @fn     int main (int argc, char *argv[])
 * @brief  The main program. The entry point of the project...
 * @return The return code (null if correct)
 * @param  argc The number or arguments in argv
 * @param  argv The array of arguments from the calling program
 */
int main(int, char *[])
{
  return openturns_library_ok() ? 0 : 1 ;
}
