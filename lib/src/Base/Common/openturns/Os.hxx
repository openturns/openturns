//                                               -*- C++ -*-
/**
 *  @brief This class provides operating system specific variables
 *
 *  Copyright 2005-2025 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifndef OPENTURNS_OS_HXX
#define OPENTURNS_OS_HXX

#include "openturns/OTtypes.hxx"
#include "openturns/OTdebug.h"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class Os
 *
 * This class provides operating system specific.
 */
class OT_API Os
{

public:

  /**
   * Return the path separator of the target OS.
   */
  static const char * GetDirectorySeparator();

  /**
   * Return the separator to split directory list.
   */
  static const char * GetDirectoryListSeparator();

  /**
   * Remove a file.
   */
  static void Remove(const String & fileName);

  static Bool IsDirectory(const String & fileName);

  static Bool IsFile(const String & fileName);

private:

  /** Default constructor */
  Os() {};

}; /* class Os */


END_NAMESPACE_OPENTURNS


#endif /* OPENTURNS_OS_HXX */


