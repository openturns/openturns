//                                               -*- C++ -*-
/**
 *  @brief This class provides operating system specific variables
 *
 *  Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
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

#include "OTwindows.h" // For CreateProcess
#include "Os.hxx"
#include "Log.hxx"
#include "OSS.hxx"
#include "ResourceMap.hxx" // For ResourceMap

#ifdef OPENTURNS_HAVE_UNISTD_H
# include <unistd.h>  // for rmdir, unlink
#endif

#include <cstdlib>   // for system(3)
#include <sys/types.h>            // for stat
#include <sys/stat.h>             // for stat
#include <fcntl.h>
#ifndef WIN32
#include <ftw.h>       // for stat(2)
#endif

#ifdef _MSC_VER
# include <direct.h>
# define MKDIR(p, mode)  _mkdir(p)
# if !defined(S_ISDIR)
#  define S_ISDIR(mode) (((mode) & S_IFDIR) != 0)
# endif
#elif defined(WIN32)
# define MKDIR(p, mode)  mkdir(p)
#else
# define MKDIR(p, mode)  mkdir(p, mode)
#endif

BEGIN_NAMESPACE_OPENTURNS

const char * Os::GetDirectorySeparator()
{
#ifndef WIN32
  return "/";
#else
  return "\\";
#endif
}

const char * Os::GetDirectoryListSeparator()
{
#ifndef WIN32
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
#ifndef WIN32
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
#ifdef WIN32
  if ( ResourceMap::GetAsUnsignedInteger("Os-create-process") == 1 )
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
  if (ResourceMap::GetAsUnsignedInteger("Os-RemoveFiles") == 0) return;
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
#endif
}

// Function helper for Os::MakeDirectory
static bool
is_directory(const char * name)
{
  struct stat dir_stat;
  if(stat(name, &dir_stat) != 0) return false;
  return S_ISDIR(dir_stat.st_mode);
}

// Returns 0 if no error
int Os::MakeDirectory(const String & path)
{
  if (path.empty()) return 1;
  if (is_directory(path.c_str())) return 0;

  String slashPath(path);
  convert_backslashes(slashPath);

  String::size_type pos = 0;
  while((pos = slashPath.find('/', pos)) != String::npos)
  {
    String current_dir(path.substr(0, pos));
    const char * cpath = current_dir.c_str();
    if (!is_directory(cpath) && (0 != MKDIR(cpath, 0777))) return 1;
    pos++;
  }

  return 0;
}

#ifndef WIN32
static int deleteRegularFileOrDirectory(const char * path,
                                        const struct stat * p_sb,
                                        int typeflag,
                                        struct FTW * ftwbuf)
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
  if (!is_directory(path.c_str())) return 1;

  // Refuse to delete root directory (/) and current directory (.)
  if (path == "/" || path == ".") return 1;

  const char * directory = path.c_str();
#ifdef WIN32
  if ( ((strlen( directory ) == 3) && (directory[1] == ':') && (directory[2] == '\\' || directory[2] == '/')) ||
       ((strlen( directory ) == 2) && (directory[1] == ':')) )
  {
    // do not delete root directory
    return 1;
  }
#endif

  struct stat file_stat;
  int rc = 0;
  rc = stat( directory, &file_stat );
  if (rc == 0)
  {
    if (! S_ISDIR(file_stat.st_mode))
    {
      // Not a directory
      return 1;
    }
  }

#ifndef WIN32

  rc = nftw( directory, deleteRegularFileOrDirectory, 20, FTW_DEPTH );
  if ( rc != 0 ) return 1;

#else /* WIN32 */

  size_t timeout = ResourceMap::GetAsUnsignedInteger("output-files-timeout");
  size_t countdown = timeout;
  OT::String rmdirCmd = OT::String("rmdir /Q /S \"") + directory + "\"";
  int directoryExists;

  do
  {
    rc = system( (rmdirCmd + " > NUL 2>&1").c_str() );

    // check if directory still there (rmdir dos command always return 0)
    struct stat dir_stat;
    directoryExists = stat( directory, &dir_stat );
    if( directoryExists == 0 )
    {
      if  ( countdown <= 0 ) return 1;
      --countdown;
    }
    Sleep( 1000 );
  }
  while( directoryExists == 0 );

#endif /* WIN32 */

  return 0;
}


END_NAMESPACE_OPENTURNS
