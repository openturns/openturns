//                                               -*- C++ -*-
/**
 *  @brief This class provides operating system specific variables
 *
 *  Copyright 2005-2021 Airbus-EDF-IMACS-ONERA-Phimeca
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

#ifdef OPENTURNS_HAVE_UNISTD_H
# include <unistd.h>  // for rmdir, unlink
#endif

#include <cstdlib>   // for system(3)

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
# define MKDIR(p, mode)  _mkdir(p)
# if !defined(S_ISDIR)
#  define S_ISDIR(mode) (((mode) & S_IFDIR) != 0)
# endif
# if !defined(S_ISREG)
#  define S_ISREG(mode) (((mode) & S_IFREG) != 0)
# endif
#elif defined(_WIN32)
# define MKDIR(p, mode)  mkdir(p)
#else
# define MKDIR(p, mode)  mkdir(p, mode)
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

const char * Os::GetEndOfLine()
{
#ifndef __MINGW32__
  return "\n";
#else
  return "\r\n";
#endif
}

String Os::GetDeleteCommandOutput()
{
#ifndef _WIN32
  return " > /dev/null 2>&1";
#else
  return " > NUL";
#endif
}

// Returns 0 if no error
int Os::ExecuteCommand(const String & command)
{
  int rc = -1;
  LOGINFO( OSS() << "Execute command=" << command );
#ifdef _WIN32
  if ( ResourceMap::GetAsBool("Os-create-process"))
  {
    // Startup information
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    // Process information
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    // Create the process
    DWORD dwProcessFlags = 0;
    char * cmd = strdup(command.c_str());
    const Bool processOk = CreateProcess(NULL, cmd, NULL, NULL, true, dwProcessFlags, NULL, NULL, &si, &pi) != 0;
    free(cmd);
    if ( processOk )
    {
      // Wait for the external application to finish
      DWORD waitRc = WaitForSingleObject(pi.hProcess, INFINITE);
      if ( waitRc != WAIT_FAILED )
      {
        DWORD exit_code = 0;
        const Bool codeOk = GetExitCodeProcess(pi.hProcess, &exit_code) != 0;
        if (codeOk)
        {
          rc = exit_code;
        }
        else
        {
          rc = GetLastError();
        }
      }

      // Close everything
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
    }
    else
    {
      rc = GetLastError();
    }
  } // use create process
  else
#endif
  {
    rc = system(command.c_str());
  }
  LOGINFO( OSS() << "Return code=" << rc << " for command=" << command );
  return rc;
}


void Os::Remove(const String& fileName)
{
  if (!ResourceMap::GetAsBool("Os-RemoveFiles")) return;
  if (remove(fileName.c_str()) == -1)
  {
    Log::Warn(OSS() << "Warning: cannot remove file " << fileName);
  }
}

// Function helper for Os::MakeDirectory: replace backslash by slash
static void
convert_backslashes(String & path)
{
#ifdef _WIN32
  const char* current_char = path.c_str();
  String::size_type pos = 0;
  // On Windows, leading \\ is for network paths and must not be stripped
  if (*current_char == '\\' && *(current_char + 1) == '\\')
  {
    pos = 2;
    current_char += pos;
  }
  for ( ; *current_char != '\0'; ++pos, ++current_char )
  {
    if (*current_char == '\\') path[pos] = '/';
  }
#else
  (void) path;
#endif
}

Bool Os::IsDirectory(const String & fileName)
{
  struct stat dir_stat;
  if(stat(fileName.c_str(), &dir_stat) != 0) return false;
  return S_ISDIR(dir_stat.st_mode);
}

Bool Os::IsFile(const String & fileName)
{
  struct stat dir_stat;
  if(stat(fileName.c_str(), &dir_stat) != 0) return false;
  return S_ISREG(dir_stat.st_mode);
}

// Returns 0 if no error
int Os::MakeDirectory(const String & path)
{
  if (path.empty()) return 1;
  if (IsDirectory(path)) return 0;

  String slashPath(path);
  convert_backslashes(slashPath);

  String::size_type pos = 0;
  while((pos = slashPath.find('/', pos)) != String::npos)
  {
    String current_dir(path.substr(0, pos));
    const char * cpath = current_dir.c_str();
    if (!IsDirectory(current_dir) && (0 != MKDIR(cpath, 0777))) return 1;
    pos++;
  }

  return 0;
}

#ifndef _WIN32
static int deleteRegularFileOrDirectory(const char * path,
                                        const struct stat *,
                                        int typeflag,
                                        struct FTW * )
{
  int rc;

  switch (typeflag)
  {
    case FTW_DP:
      rc = rmdir( path );
      if ( rc < 0 ) return 1;
      break;

    case FTW_SL:
    case FTW_SLN:
    case FTW_F:
      rc = unlink( path );
      if ( rc < 0 ) return 1;
      break;

  } /* end switch */

  return 0;
}
#endif /* !WIN32 */



// Delete a directory and its contents recursively. Returns 0 if no error
int Os::DeleteDirectory(const String & path)
{
  if (path.empty()) return 1;
  if (!IsDirectory(path)) return 1;

  // Refuse to delete root directory (/) and current directory (.)
  if (path == "/" || path == ".") return 1;

  const char * directory = path.c_str();
#ifdef _WIN32
  if ( ((strlen( directory ) == 3) && (directory[1] == ':') && (directory[2] == '\\' || directory[2] == '/')) ||
       ((strlen( directory ) == 2) && (directory[1] == ':')) )
  {
    // do not delete root directory
    return 1;
  }
#endif

  int rc = 0;

#ifndef _WIN32

  rc = nftw(directory, deleteRegularFileOrDirectory, 20, FTW_DEPTH);

#else /* WIN32 */

  UnsignedInteger countdown = ResourceMap::GetAsUnsignedInteger("output-files-timeout");
  const String rmdirCmd("rmdir /Q /S \"" + path + "\"" + " > NUL 2>&1");
  Bool directoryExists = true;

  do
  {
    rc = system(rmdirCmd.c_str());

    // check if directory still there (rmdir dos command always return 0)
    directoryExists = IsDirectory(path);
    if (directoryExists)
    {
      if (countdown == 0) return 1;
      -- countdown;
    }
    Sleep(1000);
  }
  while (directoryExists);

#endif /* WIN32 */

  return rc;
}


END_NAMESPACE_OPENTURNS
