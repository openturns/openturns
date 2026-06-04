//                                               -*- C++ -*-
/**
 *  @brief This class provides operating system specific variables
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

#include "openturns/Os.hxx"
#include "openturns/Log.hxx"
#include "openturns/OSS.hxx"
#include "openturns/ResourceMap.hxx" // For ResourceMap

#include <filesystem>

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
#if defined(__cplusplus) && (__cplusplus >= 202002L)
  const std::u8string u8FileName(reinterpret_cast<const char8_t*>(fileName.data()),
                                 reinterpret_cast<const char8_t*>(fileName.data() + fileName.size()));
  if (!std::filesystem::remove(std::filesystem::path{u8FileName}))
#else
  if (!std::filesystem::remove(std::filesystem::u8path(fileName)))
#endif
  {
    LOGWARN(OSS() << "Os: cannot remove file " << fileName);
  }
}

Bool Os::IsDirectory(const String & fileName)
{
#if defined(__cplusplus) && (__cplusplus >= 202002L)
  const std::u8string u8FileName(reinterpret_cast<const char8_t*>(fileName.data()),
                                 reinterpret_cast<const char8_t*>(fileName.data() + fileName.size()));
  return std::filesystem::is_directory(std::filesystem::path{u8FileName});
#else
  return std::filesystem::is_directory(std::filesystem::u8path(fileName));
#endif
}

Bool Os::IsFile(const String & fileName)
{
#if defined(__cplusplus) && (__cplusplus >= 202002L)
  const std::u8string u8FileName(reinterpret_cast<const char8_t*>(fileName.data()),
                                 reinterpret_cast<const char8_t*>(fileName.data() + fileName.size()));
  return std::filesystem::is_regular_file(std::filesystem::path{u8FileName});
#else
  return std::filesystem::is_regular_file(std::filesystem::u8path(fileName));
#endif
}

END_NAMESPACE_OPENTURNS
