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

#include "openturns/OTwindows.h" // For CreateProcess
#include "openturns/Os.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/ResourceMap.hxx" // For ResourceMap

// include OTConfig that defines OPENTURNS_HAVE_XXX
#include "openturns/OTconfig.hxx"

#ifdef OPENTURNS_ENABLE_CXX17
#include <filesystem>
#else

#ifdef OPENTURNS_HAVE_SYS_TYPES_H
# include <sys/types.h>            // for stat
#endif

#ifdef OPENTURNS_HAVE_SYS_STAT_H
# include <sys/stat.h>             // for stat
#endif

#include <fcntl.h>
#ifndef _WIN32
#include <ftw.h>       // for stat(2)
#endif

#ifdef _MSC_VER
# include <direct.h>
# if !defined(S_ISDIR)
#  define S_ISDIR(mode) (((mode) & S_IFDIR) != 0)
# endif
# if !defined(S_ISREG)
#  define S_ISREG(mode) (((mode) & S_IFREG) != 0)
# endif
#endif

#endif

BEGIN_NAMESPACE_OPENTURNS

const char * Os::GetDirectorySeparator()
{
#ifndef _WIN32
  return "/";
#else
  return "\\";
#endif
}

const char * Os::GetDirectoryListSeparator()
{
#ifndef _WIN32
  return ":";
#else
  return ";";
#endif
}

void Os::Remove(const String& fileName)
{
  if (!ResourceMap::GetAsBool("Os-RemoveFiles")) return;
#ifdef OPENTURNS_ENABLE_CXX17
  if (!std::filesystem::remove(std::filesystem::u8path(fileName)))
#else
  if (remove(fileName.c_str()) == -1)
#endif
  {
    Log::Warn(OSS() << "Warning: cannot remove file " << fileName);
  }
}

Bool Os::IsDirectory(const String & fileName)
{
#ifdef OPENTURNS_ENABLE_CXX17
  return std::filesystem::is_directory(std::filesystem::u8path(fileName));
#else
  struct stat dir_stat;
  if(stat(fileName.c_str(), &dir_stat) != 0) return false;
  return S_ISDIR(dir_stat.st_mode);
#endif
}

Bool Os::IsFile(const String & fileName)
{
#ifdef OPENTURNS_ENABLE_CXX17
  return std::filesystem::is_regular_file(std::filesystem::u8path(fileName));
#else
  struct stat dir_stat;
  if(stat(fileName.c_str(), &dir_stat) != 0) return false;
  return S_ISREG(dir_stat.st_mode);
#endif
}

END_NAMESPACE_OPENTURNS
