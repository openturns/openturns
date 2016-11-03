//                                               -*- C++ -*-
/**
 *  @brief This class provides all the treatments for wrapper file manipulation
 *
 *  Copyright 2005-2016 Airbus-EDF-IMACS-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <vector>                 // for std::vector
#include <string>                 // for std::string
#include <cstdlib>                // for getenv
#include <cstring>                // for strcpy

#ifdef WIN32
#include <fstream>                // for ofstream
#include "openturns/OTwindows.h"            // for GetTempFileName, GetModuleFileName
#ifdef _MSC_VER
# include <direct.h>
#define mkdir(p)  _mkdir(p)
#endif /* _MSC_VER */
#endif /* WIN32 */

// Include OTConfig that defines OPENTURNS_HAVE_XXX
// It also defines INSTALL_PATH, SYSCONFIG_PATH, DATA_PATH, OPENTURNS_HOME_ENV_VAR
#include "openturns/OTconfig.hxx"

#include "openturns/OTthread.hxx"
#include "openturns/OSS.hxx"
#include "openturns/Path.hxx"
#include "openturns/Exception.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Os.hxx"
#include "openturns/Log.hxx"
#ifdef OPENTURNS_HAVE_LIBGEN_H
#include <libgen.h>               // for dirname
#endif

#ifdef OPENTURNS_HAVE_UNISTD_H
#include <unistd.h>               // for getpid, readlink, close
#endif

#ifndef INSTALL_PATH
#error "INSTALL_PATH is NOT defined. Check configuration."
#endif

#ifdef OPENTURNS_HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifndef SYSCONFIG_PATH
#error "SYSCONFIG_PATH is NOT defined. Check configuration."
#endif

#ifndef DATA_PATH
#error "DATA_PATH is NOT defined. Check configuration."
#endif

#ifndef OPENTURNS_HOME_ENV_VAR
#error "OPENTURNS_HOME_ENV_VAR is NOT defined. Check configuration."
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
    directory = String(INSTALL_PATH);
  }
  return directory;
}


FileName Path::GetExecutableDirectory()
{
  // get executable absolute path
#ifdef _WIN32
  TCHAR szPath[MAX_PATH];
  GetModuleFileName(NULL, szPath, MAX_PATH);
#else
  const UnsignedInteger MAX_PATH = 512;
  char path[MAX_PATH];
  char szPath[MAX_PATH];
  pid_t pid = getpid();
  sprintf(path, "/proc/%d/exe", pid);
  if (readlink(path, szPath, MAX_PATH) == -1)
    perror("readlink");
#endif

  // get parent dir
#ifdef OPENTURNS_HAVE_LIBGEN_H
  String pythonDir(dirname(szPath));
#else
  String pythonDir(szPath);
  if (pythonDir.empty()) return pythonDir;
  for(SignedInteger i = pythonDir.size() - 1; i >= 0; -- i)
  {
    /* We do not care about escaped backslashes */
    if(pythonDir.at(i) == '\\')
    {
      pythonDir.resize(i);
      break;
    }
  }
#endif

  return pythonDir;
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
#ifndef WIN32
    directory += "/etc";
    directory += PrefixConfigSubdirectory_;
#endif
    dirExists = Os::IsDirectory(directory);
  }
  if (!dirExists)
  {
    directory = String(SYSCONFIG_PATH) + PrefixConfigSubdirectory_;
  }
  directoryList.push_back(directory);

  // When the compile-time prefix is not the actual installation prefix,
  // guess from the Python location, happens with the Windows installer
#ifdef _WIN32
  directoryList.push_back(GetExecutableDirectory() + "\\Lib\\site-packages\\openturns");
#endif

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
#ifdef WIN32
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


#ifdef WIN32

/*
 * Convert slash to antislash.
 * ex: if filename = C:/windows/temp, return C:/windows/temp
 */
void Path::AntislashFileName(FileName & filename)
{
  for (UnsignedInteger i = 0; i < filename.size(); ++ i)
    if (filename.at(i) == '/')
      filename.at(i) = '\\';
}

/*
 * add windows backslash to filename (e.g. for compatibility with R)
 * ex: if filename = C:\windows\temp, return C:\\windows\\temp
 */
void Path::DoubleslashFileName(FileName & filename)
{
  String::size_type loc = filename.find(Os::GetDirectorySeparator());
  while(loc != String::npos)
  {
    // "\" at the last pos
    if(loc == filename.size() - 1)
    {
      filename.insert(loc, Os::GetDirectorySeparator());
      break;
    }
    // "\" in the middle
    if(filename.at(loc + 1) != Os::GetDirectorySeparator()[0])
      filename.insert(loc, Os::GetDirectorySeparator());
    // else: no "\", or "\\" in the middle
    loc = filename.find(Os::GetDirectorySeparator(), loc + 2);
  }
}

#endif


FileName Path::GetTemporaryDirectory()
{
  FileName tempDirectory;

  String tempStr(ResourceMap::Get("temporary-directory"));
#ifndef WIN32
  tempDirectory = tempStr;
#else
  const char * tempEnv = getenv(tempStr.c_str());
  if (tempEnv)
  {
    // if temporary-directory is an env var, return the content of the env var.
    tempDirectory = String(tempEnv);
  }
  else
  {
    // if not, just return the content of temporary-directory
    tempDirectory = tempStr;
  }
#endif

  return tempDirectory;
}


/* Build a temporary file name given a pattern */
FileName Path::BuildTemporaryFileName(const FileName & pattern)
{
#ifndef WIN32
  String fullPattern(GetTemporaryDirectory() + String(Os::GetDirectorySeparator()) + pattern);
  UnsignedInteger size = fullPattern.size();
  char temporaryFileName[size + 1];
  strcpy(temporaryFileName, fullPattern.c_str());
  temporaryFileName[size] = 0;
  int fileDescriptor(mkstemp(temporaryFileName));
  close(fileDescriptor);
  return temporaryFileName;
#else
  // get uniq name
  char temporaryFileName[MAX_PATH];
  GetTempFileName(GetTemporaryDirectory().c_str(), // directory for tmp files
                  TEXT(pattern.c_str()), // temp file name prefix
                  0,                     // create unique name
                  temporaryFileName);    // buffer for name
  // check temporary filename
  if (!Os::IsFile(String(temporaryFileName)))
    LOGERROR(OSS() << "Temporary file name " << temporaryFileName << " does NOT exists. Check your temporary directory.");
  // add "/" to the directory
  String slashedTemporaryFileName(temporaryFileName);
  DoubleslashFileName(slashedTemporaryFileName);
  return slashedTemporaryFileName;
#endif
}

/* Create a temporary directory.
 */
String Path::CreateTemporaryDirectory (const FileName & directoryPrefix)
{
  if (directoryPrefix.size() == 0) throw InvalidArgumentException(HERE) << "No prefix defined to create temporary directory";

#ifndef WIN32
  String tempDir(GetTemporaryDirectory());
  tempDir += Os::GetDirectorySeparator();
  tempDir += directoryPrefix;
  tempDir += "_XXXXXX";

  char * tempDirName = (char *) calloc(tempDir.size() + 1, sizeof (char));
  strncpy(tempDirName, tempDir.c_str(), tempDir.size() + 1);
  char *tempDirName_p = mkdtemp(tempDirName);
  if ( ! tempDirName_p ) throw FileOpenException(HERE) << "Could not create temporary directory from template " << tempDir;
  const String finalTempDirName(tempDirName);
  free(tempDirName);
#else
  char temporaryDirName[MAX_PATH];
  int ret = 0;
  for (int retry = 10000; retry >= 0; --retry)
  {
    ret = GetTempFileName(GetTemporaryDirectory().c_str(),       // directory for tmp files
                          TEXT((directoryPrefix + "abc").c_str()),     // temp file name prefix (only 3 characters are used)
                          0,                                           // create unique name
                          temporaryDirName);                           // buffer for name
    if (0 == ret)
    {
      ret = -1;
      continue;
    }
    DeleteFile(temporaryDirName);
    ret = mkdir(temporaryDirName);
    if (0 == ret) break;
  }
  if (0 != ret)
  {
    LOGERROR(OSS() << "Can't create temporary directory.");
    temporaryDirName[0] = '\0';
  }

  const String finalTempDirName(temporaryDirName);

#endif

  return finalTempDirName;
}


/* Delete a temporary directory.
 */
void Path::DeleteTemporaryDirectory (const FileName & directoryName)
{
  if (Os::DeleteDirectory(directoryName))
    LOGWARN(OSS() << "Can't remove temporary directory  " << directoryName);
}



END_NAMESPACE_OPENTURNS
