//                                               -*- C++ -*-
/**
 *  @brief This class provides all the treatments for wrapper file manipulation
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
#include <vector>                 // for std::vector
#include <string>                 // for std::string
#include <cstdlib>                // for getenv
#include <cstring>                // for strdup

#ifdef _WIN32
#include "openturns/OTwindows.h"  // for GetModuleFileName
#endif

// Include OTConfig that defines OPENTURNS_HAVE_XXX
// It also defines INSTALL_PATH, SYSCONFIG_PATH, OPENTURNS_HOME_ENV_VAR
#include "openturns/OTconfig.hxx"

#include "openturns/OSS.hxx"
#include "openturns/Path.hxx"
#include "openturns/Exception.hxx"
#include "openturns/Os.hxx"
#include "openturns/Log.hxx"

#include <filesystem>

#ifndef INSTALL_PATH
#error "INSTALL_PATH is NOT defined. Check configuration."
#endif

#ifndef SYSCONFIG_PATH
#error "SYSCONFIG_PATH is NOT defined. Check configuration."
#endif

#ifndef OPENTURNS_HOME_ENV_VAR
#error "OPENTURNS_HOME_ENV_VAR is NOT defined. Check configuration."
#endif

#ifdef OPENTURNS_HAVE_DLFCN_H
#include <dlfcn.h>                // for dladdr
#endif

BEGIN_NAMESPACE_OPENTURNS

/* The environment variable name */
const char * Path::OpenturnsConfigPathVariableName_ = "OPENTURNS_CONFIG_PATH";

/* The HOME subdirectory path */
const char * Path::HomeConfigSubdirectory_ = "/openturns/etc";

/* The 'openturns' configuration subdirectory path */
const char * Path::PrefixConfigSubdirectory_ = "/openturns";

/*
 * Default constructor
 */
Path::Path()
{
  // Nothing to do
}

/**
 * Return the OpenTURNS prefix
 *
 * The search rule is: check the following paths, in that order:
 *    + if the env var OPENTURNS_HOME exists, return ${OPENTURNS_HOME} if it exists and is a
 *      directory,
 *    + otherwise return the installation path ${prefix}, where 'prefix' is the installation path
 *      of the platform as provided at configuration time.
 */
FileName Path::GetInstallationDirectory()
{
  FileName directory;
  bool dirExists = false;
  const char * otHome = getenv(OPENTURNS_HOME_ENV_VAR);
  if (otHome)
  {
    directory = String(otHome);
    dirExists = Os::IsDirectory(directory);
  }
  if (!dirExists)
  {
    // NOTE: When installing with conda, INSTALL_PATH is substituted during
    // package installation.  On OSX, String(INSTALL_PATH) creates a
    // string with new content but original size; thus we use a C string
    // to update string length
    directory = String(INSTALL_PATH).c_str();
  }
  return directory;
}


FileName Path::GetParentDirectory(const FileName & fileName)
{
  return std::filesystem::u8path(fileName).parent_path().string();
}


FileName Path::GetLibraryDirectory()
{
  FileName libraryLocation;
#ifdef OPENTURNS_HAVE_DLADDR
  Dl_info di;
  const int ret = dladdr(reinterpret_cast<const void *>(&GetInstallationDirectory), &di);
  if (ret == 0 || di.dli_fname == 0)
    throw InternalException(HERE) << "call to dladdr failed";
  libraryLocation = FileName(di.dli_fname);
#elif defined(_WIN32)
  MEMORY_BASIC_INFORMATION mbi;
  if (!VirtualQuery(reinterpret_cast<const void *>(&GetInstallationDirectory), &mbi, sizeof(mbi)))
    throw InternalException(HERE) << "call to VirtualQuery failed";
  char pathBuf[16384];
  if (!GetModuleFileName(static_cast<HMODULE>(mbi.AllocationBase), pathBuf, sizeof(pathBuf)))
    throw InternalException(HERE) << "call to GetModuleFileName failed";
  libraryLocation = FileName(pathBuf);
#endif
  return GetParentDirectory(libraryLocation);
}


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
Path::DirectoryList Path::GetConfigDirectoryList()
{
  // Create an empty directory list
  DirectoryList directoryList;

  // ... search in ${OPENTURNS_CONFIG_PATH}
  // Because OPENTURNS_CONFIG_PATH is a path, we have to split it
  char * openturnsConfigDirectory = getenv(Path::OpenturnsConfigPathVariableName_);
  if (openturnsConfigDirectory)
  {
    std::string  pathToSplit = openturnsConfigDirectory;
    String::size_type lastColonPosition = 0;
    String::size_type currentColonPosition = 0;
    while ( ((currentColonPosition = pathToSplit.find(Os::GetDirectoryListSeparator(), lastColonPosition)) != String::npos) &&
            ( currentColonPosition < pathToSplit.size() ) )
    {
      FileName directory(pathToSplit, lastColonPosition, currentColonPosition - lastColonPosition);
      if (directory.size() == 0) directory = ".";
      directoryList.push_back(directory);
      lastColonPosition = currentColonPosition + 1;
    } /* end while */
    FileName directory(pathToSplit, lastColonPosition, pathToSplit.size() - lastColonPosition);
    if (directory.size() == 0) directory = ".";
    directoryList.push_back(directory);
  }

  // ... search in ${HOME}/openturns/etc
  char * homeDirectory = getenv("HOME");
  if (homeDirectory)
  {
    FileName directory = homeDirectory;
    directory += Path::HomeConfigSubdirectory_;
    directoryList.push_back(directory);
  }

  // ... search in standard config directory
  // (${OPENTURNS_HOME}/etc/openturns or ${prefix}/etc/openturns)
  FileName directory;
  bool dirExists = false;
  const char * otHome = getenv(OPENTURNS_HOME_ENV_VAR);
  if (otHome)
  {
    directory = String(otHome);
#ifndef _WIN32
    directory += "/etc";
    directory += PrefixConfigSubdirectory_;
#endif
    dirExists = Os::IsDirectory(directory);
  }
  if (!dirExists)
  {
    // See in NOTE above why we use c_str() here
    directory = String() + String(SYSCONFIG_PATH).c_str() + PrefixConfigSubdirectory_;
  }
  directoryList.push_back(directory);

  // When the compile-time prefix is not the actual installation prefix,
  // fallback to the location of the shared lib (windows, wheels, ...)
  try
  {
    directoryList.push_back(GetLibraryDirectory());
  }
  catch (const InternalException & ex)
  {
    // we cannot throw here as this method is called in the initialization
    LOGWARN(OSS() << "Could not get library directory: " << ex.what());
  }
  return directoryList;
}


/*
 * Find a file named 'name' located in one of the directories
 * listed in 'dirList'. The function returns the full path (directory/name)
 * of the first match.
 */
FileName Path::FindFileByNameInDirectoryList(const FileName & name,
    const DirectoryList & dirList)
{
  if (name.size() == 0) throw FileNotFoundException(HERE) << "Can NOT find a file with no name";

  // If the name starts with a slash then it is an absolute path
  // and it did not need to be searched in the directory list
  // so we return it as is.
  if (name[0] == '/') return name;
#ifdef _WIN32
  if ((name.size() > 1) && (name[1] == ':')) return name;
#endif

  // We create the full path name of the file with each directory
  // of the list in turn, and then we check if this file exists
  DirectoryList::const_iterator currentDirectory;
  for(currentDirectory  = dirList.begin();
      currentDirectory != dirList.end();
      ++ currentDirectory)
  {
    LOGDEBUG(OSS() << "Searching '" << name << "' in directory : " << *currentDirectory);

    FileName fullPathForFile(*currentDirectory + Os::GetDirectorySeparator() + name);
    if (Os::IsFile(fullPathForFile))
      return fullPathForFile;
  }

  // Hmm ! Seems we have a problem...
  // No file was found is the search path so we throw an exception

  OSS errorMessage;
  errorMessage << "No file named '" << name
               << "' was found in any of those directories :";
  for(currentDirectory  = dirList.begin();
      currentDirectory != dirList.end();
      ++ currentDirectory)
  {
    errorMessage << " " << *currentDirectory;
  }
  throw FileNotFoundException(HERE) << String(errorMessage);

} /* end findFileByNameInDirectoryList */


END_NAMESPACE_OPENTURNS
