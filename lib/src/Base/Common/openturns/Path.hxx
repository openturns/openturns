//                                               -*- C++ -*-
/**
 *  @brief This class provides all the mechanism of path manipulation
 *
 *  Copyright 2005-2026 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PATH_HXX
#define OPENTURNS_PATH_HXX

#include <vector>                // for std::vector
#include "openturns/OTprivate.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Path
 * @brief Provides mechanism for path manipulation
 *
 * The class defines the search algorithms used to find a file or a directory
 * through the filesystem. It also defines the standard hardcoded paths and the
 * way temporary files are built.
 */

class OT_API Path
{
public:

  typedef std::vector<FileName> DirectoryList;

  /**
   * Return the OpenTURNS prefix
   *
   * The search rule is: check the following paths, in that order:
   *    + if the env var OPENTURNS_HOME exists, return ${OPENTURNS_HOME} if it exists and is a
   *      directory,
   *    + otherwise return the installation path ${prefix}, where 'prefix' is the installation path
   *      of the platform as provided at configuration time.
   */
  static FileName GetInstallationDirectory();

  /**
   * Analyse the process environment
   * and return a list of directories to search in for configuration files.
   *
   * The search rule is :look for the file in the following directories, in that order :
   *    + if the env var OPENTURNS_CONFIG_PATH exists, in directories listed in
   *      ${OPENTURNS_CONFIG_PATH} (see openturnsConfigPathVariableName_)
   *    + in directory ${HOME}/openturns/etc (see homeSubdirectory_)
   *    + in standard config directory (either ${OPENTURNS_HOME}/etc/openturns if OPENTURNS_HOME
   *      is defined and if this path is a directory, or the path ${prefix}/etc/openturns
   *      where 'prefix' is the installation path of the platform as provided at configuration time.
   */
  static DirectoryList GetConfigDirectoryList();

  /**
   * Find a file named \em name located in one of the directories
   * listed in \em dirList. The function returns the full path (directory/name)
   * of the first match.
   */
  static FileName FindFileByNameInDirectoryList(const FileName & name,
      const DirectoryList & dirList);

  /** Get the location of the OT shared library (at runtime) */
  static FileName GetLibraryDirectory();

protected:

  /**
   * @var static const char * Path::OpenturnsConfigPathVariableName_
   *
   * The environment variable name
   */
  static const char * OpenturnsConfigPathVariableName_;

  /**
   * @var static const char * Path::HomeConfigSubdirectory_
   *
   * The HOME subdirectory path
   */
  static const char * HomeConfigSubdirectory_;

private:
  /** Get the parent directory */
  static FileName GetParentDirectory(const FileName & fileName);

  /** Default constructor */
  Path();

}; /* class Path */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PATH_HXX */
