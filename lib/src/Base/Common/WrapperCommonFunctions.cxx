//                                               -*- C++ -*-
/**
 * @brief This file provides function tools for the wrapper library
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
#ifndef WIN32 // no strerror_r on mingw
#define _XOPEN_SOURCE 600 // for strerror_r
#endif

#include "OTwindows.h" // for Sleep

#include <stdarg.h>    // for variadic args functions
#include <cstring>
#include <cstdlib>     // for system(3)
#include <unistd.h>    // for stat(2), rmdir(2)
#include <sys/types.h> // for waitpid(2), stat(2)
#ifndef WIN32
#include <sys/wait.h>  // for waitpid(2)
#endif
#ifndef _MSC_VER
#include <libgen.h>    // for dirname(3)
#include <dirent.h>    // for scandir(3)
#endif
#include <sys/stat.h>  // for stat(2)
#ifndef WIN32
#include <ftw.h>       // for stat(2)
#endif
#include "WrapperCommonFunctions.hxx"
#include "Os.hxx"
#include "Log.hxx"
#include "ResourceMap.hxx"
#include "Exception.hxx"
#include "TTY.hxx"


#ifdef _MSC_VER
#include <BaseTsd.h>
#include <io.h>


typedef SSIZE_T ssize_t;
# define open  _open
# define close _close
# define creat _creat
# define read  _read
# define write _write
# define S_ISDIR(mode)	(((mode) & S_IFDIR) != 0)

# define va_copy(d,s) ((d) = (s))
# define snprintf c99_snprintf

static inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
  int count = -1;

  if (size != 0)
    count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
  if (count == -1)
    count = _vscprintf(format, ap);

  return count;
}

static inline int c99_snprintf(char* str, size_t size, const char* format, ...)
{
  int count;
  va_list ap;

  va_start(ap, format);
  count = c99_vsnprintf(str, size, format, ap);
  va_end(ap);

  return count;
}

#endif /* _MSC_VER */


#define SHELL_NAME "sh"
#define SHELL_PATH "/bin/sh"

#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
static const int ERRORBUF_LENGTH = 2048;
#endif

#if defined(WIN32) || defined(__APPLE__)
char * strndup(const char * str, size_t len)
{
  register size_t n;
  register char * dst;

  n = strlen(str);
  if (len < n)
    n = len;
  dst = (char *) malloc(n + 1);
  if (dst)
  {
    memcpy(dst, str, n);
    dst[n] = '\0';
  }
  return dst;
}
#endif

#if defined(__APPLE__) && defined(__DYNAMIC__)
#include <crt_externs.h>
static char **environ;
#else
extern char ** environ; // Standard environment definition
#endif

pthread_mutex_t FileSystemMutex;



#ifdef SLOW_FILESYSTEM
static pthread_once_t FileSystemMutex_once = PTHREAD_ONCE_INIT;

FileSystemMutex_init::FileSystemMutex_init()
{
  int rc = pthread_once( &FileSystemMutex_once, FileSystemMutex_init::Initialization );
  if (rc != 0)
  {
    perror("FileSystemMutex_init::FileSystemMutex_init once Initialization failed");
    exit(1);
  }
}



void FileSystemMutex_init::Initialization()
{
#ifndef OT_MUTEXINIT_NOCHECK
  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_NORMAL );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
  int rc = pthread_mutex_init( &FileSystemMutex, &attr );
  if (rc != 0)
  {
    perror("FileSystemMutex_init::Initialization mutex initialization failed");
    exit(1);
  }
#else
  pthread_mutex_init( &FileSystemMutex, NULL );
#endif
}
#endif // SLOW_FILESYSTEM


using OT::ResourceMap;
using OT::NotYetImplementedException;
using OT::Os;
using OT::Log;
using namespace OT;



template <typename T>
static inline
T max( T a, T b )
{
  return (a < b) ? b : a;
}



/* Build a newly allocated string in the style of printf */
char * newFormattedString(const char * format,
                          ...)
{
  va_list args;
  va_start( args, format );
  char * buf = vnewFormattedString( format, args );
  va_end( args );
  return buf;
}



/* Build a newly allocated string in the style of printf */
char * vnewFormattedString(const char * format,
                           va_list args)
{
  size_t len = max( strlen(format) * 2, BUFFER_LENGTH );
  char * newbuf = NULL;

  char * buf = (char *) calloc(len, sizeof(char));
  if (buf == NULL) return NULL;

  int bytes = 0;
  va_list args_copy;

  while (1)
  {
    va_copy( args_copy, args );
    bytes = vsnprintf(buf, len, format, args_copy);
    va_end( args_copy );

    if (bytes > -1 && bytes < static_cast<int>(len)) break; /* OK */

    if (bytes > -1) len = bytes + 1;
    else len *= 2;

    newbuf = (char *) realloc(buf, len * sizeof(char));
    if (newbuf == NULL)
    {
      free( buf );
      return NULL;
    }
    else buf = newbuf;
  }
  buf[bytes] = 0;

  return buf;
}



/* Write a message in the OpenTURNS log */
static void printToLog(void (*func)(const OT::String & msg),
                       const char * format,
                       va_list args)
{
  OT::OSS newfmt;
  newfmt << "[tid=" << pthread_self() << "] " << format;
  char * msg = vnewFormattedString( newfmt.str().c_str(), args );
  (*func)( msg );
  free( msg );
}



void printToLogWrapper(const char * format,
                       ...)
{
  va_list args;
  va_start( args, format );
  printToLog( Log::Wrapper, format, args );
  va_end( args );
}




/* Write a message in the OpenTURNS log */
void printToLogDebug(const char * format,
                     ...)
{
  va_list args;
  va_start( args, format );
  printToLog( Log::Debug, format, args );
  va_end( args );
}




/* Write an user message in the OpenTURNS log */
void printToLogUser(const char * format,
                    ...)
{
  va_list args;
  va_start( args, format );
  printToLog( Log::User, format, args );
  va_end( args );
}




/* Write an info message in the OpenTURNS log */
void printToLogInfo(const char * format,
                    ...)
{
  va_list args;
  va_start( args, format );
  printToLog( Log::Info, format, args );
  va_end( args );
}



/* Write an warning in the OpenTURNS log */
void printToLogWarn(const char * format,
                    ...)
{
  va_list args;
  va_start( args, format );
  printToLog( Log::Warn, format, args );
  va_end( args );
}




/* Write an error message in the OpenTURNS log */
void printToLogError(const char * format,
                     ...)
{
  va_list args;
  va_start( args, format );
  printToLog( Log::Error, format, args );
  va_end( args );
}




/* Write an trace message in the OpenTURNS log */
void printToLogTrace(const char * format,
                     ...)
{
  va_list args;
  va_start( args, format );
  printToLog( Log::Trace, format, args );
  va_end( args );
}



void setWrapperError(void * p_error,
                     const OT::String & msg)
{
  if (p_error)
  {
    struct WrapperError * p_wrapperError = static_cast<struct WrapperError *>(p_error);
    pthread_mutex_lock( &(p_wrapperError->mutex) );
    if (p_wrapperError->length > 0) free( p_wrapperError->message );
    p_wrapperError->length  = msg.size();
    p_wrapperError->message = strndup( msg.c_str(), p_wrapperError->length + 1 );
    pthread_mutex_unlock( &(p_wrapperError->mutex) );
  }
}



void clearWrapperError(void * p_error)
{
  if (p_error)
  {
    struct WrapperError * p_wrapperError = static_cast<struct WrapperError *>(p_error);
    pthread_mutex_lock( &(p_wrapperError->mutex) );
    free( p_wrapperError->message );
    p_wrapperError->message = 0;
    p_wrapperError->length  = 0;
    pthread_mutex_unlock( &(p_wrapperError->mutex) );
  }
}



OT::String getWrapperError(void * p_error)
{
  OT::String msg;
  if (p_error)
  {
    struct WrapperError * p_wrapperError = static_cast<struct WrapperError *>(p_error);
    pthread_mutex_lock( &(p_wrapperError->mutex) );
    if (p_wrapperError->length > 0)
    {
      msg = p_wrapperError->message;
      //       free( p_wrapperError->message );
      //       p_wrapperError->message = 0;
      //       p_wrapperError->length  = 0;
    }
    pthread_mutex_unlock( &(p_wrapperError->mutex) );
  }
  return msg;
}




/* Some functions that help debugging memory allocation */
/* Allocate SIZE bytes of memory.  */
void* dbg_malloc __MALLOC_P ((int line,
                              size_t __size))
{
  void* ptr = malloc(__size);
  if (ptr) memset(ptr, 0, __size);
#ifdef DEBUG
  printToLogDebug( "(malloc) line %d : ptr=%d size=%d", line, ptr, __size );
#endif
  return ptr;
}




/* Allocate NMEMB elements of SIZE bytes each, all initialized to 0.  */
void* dbg_calloc __MALLOC_P ((int line,
                              size_t __nmemb,
                              size_t __size))
{
  void* ptr = calloc(__nmemb, __size);
  if (ptr) memset(ptr, 0, __nmemb * __size);
  if (Log::HasDebug()) printToLogDebug( "(calloc) line %d : ptr=%p nmemb=%d size=%d", line, ptr, __nmemb, __size );
  return ptr;
}




/* Re-allocate the previously allocated block in __ptr, making the new
   block SIZE bytes long.  */
void* dbg_realloc __MALLOC_P ((int line,
                               void* __ptr,
                               size_t __size))
{
  void* ptr = realloc(__ptr, __size);
  if (Log::HasDebug()) printToLogDebug( "(realloc) line %d : ptr=%p __ptr=%p size=%d", line, ptr, __ptr, __size );
  return ptr;
}




/* Free a block allocated by `malloc', `realloc' or `calloc'.  */
void dbg_free __MALLOC_P ((int line,
                           void* __ptr))
{
  if (Log::HasDebug()) printToLogDebug( "(free) line %d : ptr=%p", line, __ptr );
  free(__ptr);
}




/* Protect access to the wrapper exchanged data structure */
void lock(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  assert(p_exchangedData->p_lock_);
  pthread_mutex_lock( const_cast<pthread_mutex_t *>(p_exchangedData->p_lock_) );
}




void unlock(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  assert(p_exchangedData->p_lock_);
  pthread_mutex_unlock( const_cast<pthread_mutex_t *>(p_exchangedData->p_lock_) );
}





void initMutex(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  struct WrapperExchangedData * ptr = const_cast<struct WrapperExchangedData *>(p_exchangedData);
  ptr->p_lock_ = new pthread_mutex_t;
  assert(ptr->p_lock_);

#ifndef OT_MUTEXINIT_NOCHECK
  pthread_mutexattr_t attr;
  pthread_mutexattr_init( &attr );
  //pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_NORMAL );
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
  pthread_mutex_init( const_cast<pthread_mutex_t *>(ptr->p_lock_), &attr );
  pthread_mutexattr_destroy( &attr );
#else
  pthread_mutex_init( const_cast<pthread_mutex_t *>(ptr->p_lock_), NULL );
#endif
}



void destroyMutex(const struct WrapperExchangedData * p_exchangedData)
{
  assert(p_exchangedData);
  struct WrapperExchangedData * ptr = const_cast<struct WrapperExchangedData *>(p_exchangedData);
  assert(ptr->p_lock_);
  pthread_mutex_destroy( const_cast<pthread_mutex_t *>(ptr->p_lock_) );
  delete ptr->p_lock_;
  ptr->p_lock_ = NULL;
}




/* Get the number of variables of some type (in or out) in the data read from the description file */
size_t _getNumberOfVariables(const struct WrapperExchangedData * p_exchangedData,
                             unsigned long type)
{
  size_t number = 0;

  const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
  while (currentVariableElement)
  {
    if (static_cast<UnsignedInteger>(currentVariableElement->variable_->type_) == type) ++number;
    currentVariableElement = currentVariableElement->next_;
  }

  return number;
}





/* Get the number of files of some type (in or out) in the data read from the description file */
size_t _getNumberOfFiles(const struct WrapperExchangedData * p_exchangedData,
                         unsigned long type)
{
  size_t number = 0;

  const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
  while (currentFileElement)
  {
    if (static_cast<UnsignedInteger>(currentFileElement->file_->type_) == type) ++number;
    currentFileElement = currentFileElement->next_;
  }

  return number;
}




/* Get a pointer to a variable structure. NULL if not found */
const struct WrapperVariableListElement * _getVariableById(const struct WrapperExchangedData * p_exchangedData,
    const char * id)
{
  const struct WrapperVariableListElement * v = NULL;
  const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
  while (currentVariableElement)
  {
    if (!strcmp(currentVariableElement->variable_->id_, id))
    {
      v = currentVariableElement->variable_;
      break;
    }
    currentVariableElement = currentVariableElement->next_;
  }
  return v;
}




/* Get a pointer to a file structure. NULL if not found */
const struct WrapperFileListElement * _getFileById(const struct WrapperExchangedData * p_exchangedData,
    const char * id)
{
  const struct WrapperFileListElement * f = NULL;
  const struct WrapperFileList * currentFileElement = p_exchangedData->fileList_;
  while (currentFileElement)
  {
    if (!strcmp(currentFileElement->file_->id_, id))
    {
      f = currentFileElement->file_;
      break;
    }
    currentFileElement = currentFileElement->next_;
  }
  return f;
}




/* Create a directory and all its parents if needed. Return non-null value on error */
int createDirectory(const char * directory,
                    void * p_error)
{
  if (! directory)
  {
    setWrapperError( p_error, "(createDirectory) No directory" );
    return 1;
  }

  const String path(directory);
  if (Os::MakeDirectory(path))
  {
    setWrapperError( p_error, "(createDirectory) Unable to create directory" );
    return 1;
  }
  return 0;
}



/* Delete a directory and all its content. Return non-null value on error */
int deleteDirectory(const char * directory,
                    void * p_error)
{
  if (! directory)
  {
    setWrapperError( p_error, "(deleteDirectory) No directory" );
    return 1;
  }

  const String path(directory);
  if (Os::DeleteDirectory(path))
  {
    setWrapperError( p_error, "(deleteDirectory) Unable to delete directory" );
    return 1;
  }
  return 0;
}





/* Read file pointed by path and allocate a buffer that contains all its data.
 * Put the stat of file in p_file_stat;
 */
char * readFile(const char * path,
                struct stat * p_file_stat,
                void * p_error,
                int timeout)
{
  char * buf = NULL;
  size_t acceptable_errors = MAX_ERRORS;
  size_t position;
  int fd;
  int rc = 0;
  size_t remaining_bytes;
  struct stat file_stat;
  int myerrno;
  FSLOCK( rc = stat(path, &file_stat); myerrno = errno; );
  //rc = stat(path, &file_stat); myerrno = errno;
  while (rc == -1)
  {
    if (timeout <= 0)
    {
      char msg[BUFFER_LENGTH];
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
      char err[BUFFER_LENGTH];
      strerror_r( myerrno, err, BUFFER_LENGTH );
      snprintf( msg, BUFFER_LENGTH, "(readFile) file path=%s does NOT exists. Reason: %s", path, err );
#else
      snprintf( msg, BUFFER_LENGTH, "(readFile) file path=%s does NOT exists", path );
#endif
      setWrapperError( p_error, msg );
      return NULL;
    }
    else
    {
      --timeout;

      if (Log::HasDebug()) printToLogDebug( "(readFile) file %s not available. Wait the file still %d times.", path, timeout );
#ifndef WIN32
      sleep(1);
#else
      Sleep(1000);
#endif
    }
    FSLOCK( rc = stat(path, &file_stat); myerrno = errno; );
  }
  buf = (char *) calloc(file_stat.st_size + 1, sizeof(char));
  if (buf == NULL)
  {
    char msg[BUFFER_LENGTH];
    snprintf( msg, BUFFER_LENGTH, "(readFile) Can NOT allocate enough memory (%lu bytes)", file_stat.st_size );
    setWrapperError( p_error, msg );
    return NULL;
  }

  /* We open and read the file into the buffer */
  FSLOCK( fd = open(path, O_RDONLY); myerrno = errno; ) ;
  //fd = open(path, O_RDONLY); myerrno = errno;
  if ( fd == -1)
  {
    char msg[BUFFER_LENGTH];
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
    char buf[ERRORBUF_LENGTH];
    strerror_r( myerrno, buf, ERRORBUF_LENGTH );
    snprintf( msg, BUFFER_LENGTH, "(readFile) Can NOT open file %s for reading. Reason: %s",
              path, buf );
#else
    snprintf( msg, BUFFER_LENGTH, "(readFile) Can NOT open file %s for reading",
              path );
#endif
    setWrapperError( p_error, msg );
    return NULL;
  }

  //IDM
  // LOGTRACE( OT::String("IDM - ??? file : ") + path );


  /* We read the file */
  if (Log::HasWrapper()) printToLogWrapper( "(readFile) Reading file %s (size=%d)", path, file_stat.st_size );
  remaining_bytes = file_stat.st_size;
  position = 0;
  while (remaining_bytes)
  {
    ssize_t got_bytes = 0;
    FSLOCK( got_bytes = read(fd, buf + position, remaining_bytes); myerrno = errno; );
    //got_bytes = read(fd, buf+position, remaining_bytes); myerrno = errno;
    if (Log::HasDebug()) printToLogDebug(  "(readFile) Read %d bytes - position=%d, remaining_bytes=%d", got_bytes, position, remaining_bytes );
    if (got_bytes == -1)
    {
      if (--acceptable_errors && ( (myerrno == EAGAIN) || (myerrno == EINTR) || (myerrno == EIO) ) ) continue;
      else
      {
        char msg[BUFFER_LENGTH];
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
        char buf[ERRORBUF_LENGTH];
        strerror_r( myerrno, buf, ERRORBUF_LENGTH );
        snprintf( msg, BUFFER_LENGTH, "(readFile) Error in reading file %s. Reason: %s",
                  path, buf );
#else
        snprintf( msg, BUFFER_LENGTH, "(readFile) Error in reading file %s",
                  path );
#endif
        setWrapperError( p_error, msg );
        return NULL;
      }
    }
    acceptable_errors = MAX_ERRORS;
    position += got_bytes;
    remaining_bytes -= got_bytes;
#ifdef WIN32
    if (got_bytes == 0) // remaining_bytes = nb bytes + 1 of the file!
      break;
#endif
  }
  buf[position] = 0;

  /* We close the file */
  FSLOCK( rc = close(fd); myerrno = errno; );
  //rc = close(fd); myerrno = errno;
  while ( rc == -1)
  {
    if (myerrno == EINTR)
    {
      FSLOCK( rc = close(fd); myerrno = errno; );
      continue;
    }
    else
    {
      char msg[BUFFER_LENGTH];
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
      char buf[ERRORBUF_LENGTH];
      strerror_r( myerrno, buf, ERRORBUF_LENGTH );
      snprintf( msg, BUFFER_LENGTH, "(readFile) Can NOT close file %s. Reason: %s",
                path, buf );
#else
      snprintf( msg, BUFFER_LENGTH, "(readFile) Can NOT close file %s",
                path );
#endif
      setWrapperError( p_error, msg );
      return NULL;
    }
  }

  if (Log::HasDebug())
  {
    printToLogDebug( "(readFile) Buffer from file %s (size=%d):", path, file_stat.st_size );
    char * pos = buf;
    char * newline = 0;
    OT::String prefix("(readFile) ^");
    while ( ( newline = strchr( pos, '\n' ) ) != NULL )
    {
      OT::String msg(prefix);
      msg += OT::String( pos, newline - pos );
      msg += "$";
      printToLogDebug( msg.c_str() );
      pos = newline + 1;
    }
  }
  *p_file_stat = file_stat;
  return buf;
}





/* Write the content of buf into file pointed by path. Use file_stat to set the mode */
int writeFile(const char * path,
              const char * buf,
              struct stat file_stat,
              void * p_error)
{
  size_t acceptable_errors = MAX_ERRORS;
  size_t position;
  int fd;
  int rc;
  ssize_t remaining_bytes;
  int myerrno;

  if (buf == NULL)
  {
    setWrapperError( p_error, "(writeFile) Can NOT write a null buffer" );
    return 1;
  }

  /* Create all upper directories */
  char * copypath = strdup( path );
#ifdef _MSC_VER
  char * parent = strdup( copypath );
  char * cp = parent + strlen(parent);
  while (*cp != '\\' && *cp != '/' && cp != parent) --cp;
  if (cp != parent) *cp = '\0';
  rc = createDirectory( parent, p_error );
  free( parent );
#else
  char * parent = dirname( copypath );
  rc = createDirectory( parent, p_error );
#endif
  free( copypath );
  if (rc)
  {
    char msg[BUFFER_LENGTH];
    snprintf( msg, BUFFER_LENGTH, "(writeFile) Can NOT open file %s for writing. Reason: %s",
              path, getWrapperError( p_error ).c_str() );
    setWrapperError( p_error, msg );
    return 1;
  }

  /* We open and read the file into the buffer */
  FSLOCK( fd = creat(path, file_stat.st_mode); myerrno = errno; );
  //fd = creat(path, file_stat.st_mode); myerrno = errno;
  if ( fd == -1)
  {
    char msg[BUFFER_LENGTH];
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
    char buf[ERRORBUF_LENGTH];
    strerror_r( myerrno, buf, ERRORBUF_LENGTH );
    snprintf( msg, BUFFER_LENGTH, "(writeFile) Can NOT open file %s. Reason: %s",
              path, buf );
#else
    snprintf( msg, BUFFER_LENGTH, "(writeFile) Can NOT open file %s",
              path );
#endif
    setWrapperError( p_error, msg );
    return 1;
  }


  /* We write the file */
  if (Log::HasWrapper()) printToLogWrapper( "(writeFile) Writing file %s (size=%d)", path, strlen(buf) );
  remaining_bytes = strlen(buf);
  position = 0;
  while (remaining_bytes)
  {
    int sent_bytes = 0;
    FSLOCK( sent_bytes = write(fd, buf + position, remaining_bytes); myerrno = errno; );
    //sent_bytes = write(fd, buf+position, remaining_bytes); myerrno = errno;
    if (sent_bytes == -1)
    {
      if (--acceptable_errors && ( (myerrno == EAGAIN) || (myerrno == EINTR) || (myerrno == EIO) ) ) continue;
      else
      {
        char msg[BUFFER_LENGTH];
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
        char buf[ERRORBUF_LENGTH];
        strerror_r( myerrno, buf, ERRORBUF_LENGTH );
        snprintf( msg, BUFFER_LENGTH, "(writeFile) Error in writing file %s. Reason: %s",
                  path, buf );
#else
        snprintf( msg, BUFFER_LENGTH, "(writeFile) Error in writing file %s",
                  path );
#endif
        setWrapperError( p_error, msg );
        return 1;
      }
    }
    acceptable_errors = MAX_ERRORS;
    position += sent_bytes;
    remaining_bytes -= sent_bytes;
  }

  /* We close the file */
  FSLOCK( rc = close(fd); myerrno = errno; );
  //rc = close(fd); myerrno = errno;
  while ( rc == -1 )
  {
    if (myerrno == EINTR)
    {
      FSLOCK( rc = close(fd); myerrno = errno; );
      continue;
    }
    else
    {
      char msg[BUFFER_LENGTH];
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
      char buf[ERRORBUF_LENGTH];
      strerror_r( myerrno, buf, ERRORBUF_LENGTH );
      snprintf( msg, BUFFER_LENGTH, "(writeFile) Can NOT close file %s. Reason: %s",
                path, buf );
#else
      snprintf( msg, BUFFER_LENGTH, "(writeFile) Can NOT close file %s",
                path );
#endif
      setWrapperError( p_error, msg );
      return 1;
    }
  }

  // IDM
  // LOGTRACE( OT::String("IDM - Add file : ") + path );

  return 0;
}






/* Print a regexp_match array */
void printRegexpMatchArray(const struct regexp_match regmatch)
{
  if (Log::HasDebug())
  {
    size_t i;
    for(i = 0; i < regmatch.n; ++i)
    {
      printToLogDebug( "%sregmatch[%d/%d] = { "
                       "reg_init_paren_pos = %d, "
                       "repl_init_paren_ref = %d, "
                       "reg_tmp_paren_pos = %d, "
                       "repl_tmp_paren_ref = %d, "
                       "reg_new_paren_pos = %d, "
                       "repl_new_paren_ref = %d }",
                       ((i == 0) ? "" : "      "), i, regmatch.n,
                       regmatch.d[i].reg_init_paren_pos,
                       regmatch.d[i].repl_init_paren_ref,
                       regmatch.d[i].reg_tmp_paren_pos,
                       regmatch.d[i].repl_tmp_paren_ref,
                       regmatch.d[i].reg_new_paren_pos,
                       regmatch.d[i].repl_new_paren_ref );
    }
  }
}




/* PARENTHESIS_FOUND is true when ptr points to a '(' character that is not backslashed */
#define PARENTHESIS_FOUND(ptr, string) ( (*(ptr) == '(') && ( ((ptr) == &(string)[0]) || (*((ptr)-1) != '\\') ) )




/* Return the number of parenthesis that corresponds to subexpressions in pattern matching */
inline size_t getNumberOfParenthesis(const std::string & s)
{
  size_t number = 0;
  const char * ptr = 0;

  if (s.size() == 0) return 0;

  ptr = s.c_str();
  while (*ptr)
  {
    if ( PARENTHESIS_FOUND( ptr, s.c_str() ) ) ++number;
    ++ptr;
  }

  return number;
}




/* Allocate and set the regexp_match array with the corresponding reference of sub-expressions
 * Argument myregexp contains the original regular expression with the parenthesis that must
 * be memorized.
 * Example: <regexp>^(\\S*)I1(\\S*)=(\\S*)(\\R)\\S*$</regexp>
 *
 * The function creates a structure regexp_match whose values are:
 *  regmatch.n = the number of parenthesis in the original regexp (ie, 4 in the example)
 *  regmatch.d = an array of regmatch.n elements (ie, 4 elements in the example) of type
 *               struct regexp_match_data
 *
 * For the i-th parenthesis found in the original regexp, the regmatch.d[i] element
 * contains:
 *  reg_init_paren_pos  = the position of the parenthesis in the original regexp (offset)
 *  reg_new_paren_pos   = the position of the parenthesis in the new regexp string (offset)
 *  repl_init_paren_ref = i + 1
 *  repl_new_paren_ref  = the index of the parenthesis in the new regexp string + 1
 *  reg_tmp_paren_pos   = the temporary position of the parenthesis in the original regexp (offset)
 *  repl_tmp_paren_ref  = the position of the parenthesis in the original regexp (offset)
 */

struct regexp_match createRegexpMatchArray(const std::string & myregexp)
{
  size_t number = 0;
  struct regexp_match regmatch;
  const char * ptr = 0;

  regmatch.n = 0;
  regmatch.d = 0;
  if (myregexp.size() == 0) goto ERR;

  regmatch.n = getNumberOfParenthesis(myregexp);
  if (regmatch.n > 0)
  {

    regmatch.d = (struct regexp_match_data *) calloc(regmatch.n, sizeof(struct regexp_match_data));

    ptr = myregexp.c_str();
    while (*ptr)
    {
      if ( PARENTHESIS_FOUND( ptr, myregexp.c_str() ) )
      {
        regmatch.d[number].reg_init_paren_pos  = ptr - myregexp.c_str();
        regmatch.d[number].reg_new_paren_pos   = regmatch.d[number].reg_init_paren_pos;
        regmatch.d[number].repl_init_paren_ref = number + 1;
        regmatch.d[number].repl_new_paren_ref  = number + 1;
        regmatch.d[number].reg_tmp_paren_pos   = regmatch.d[number].reg_init_paren_pos;
        regmatch.d[number].repl_tmp_paren_ref  = regmatch.d[number].repl_init_paren_ref;
        ++number;
      }
      ++ptr;
    }

  } /* end if (nb_paren > 0) */

  if (Log::HasWrapper())
    printToLogWrapper( "(createRegexpMatchArray) %sRegexp is '%s'%s",
                       TTY::GetColor(TTY::BOLD),
                       myregexp.c_str(),
                       TTY::GetColor(TTY::DEFAULT) );
  printRegexpMatchArray( regmatch );


ERR:
  return regmatch;
}




/* Free the storage allocated for a regexp_match structure */
void freeRegexpMatchArray(const struct regexp_match regmatch)
{
  free( regmatch.d );
}




void printChunk(struct chunk * elt)
{
  if (! Log::HasDebug()) return;
  printToLogDebug( "(printChunk) elt (%p) = {", elt );
  if (elt)
  {
    printToLogDebug( "(printChunk) ... next  = %p", elt->next );
    if (elt->repl)
      printToLogDebug( "(printChunk) ... repl  = %s", elt->repl );
    else
    {
      printToLogDebug( "(printChunk) ... start = %d", elt->start );
      printToLogDebug( "(printChunk) ... end   = %d", elt->end );
      OT::String firstPart  ( elt->ref, elt->ref + elt->start );
      OT::String middlePart ( elt->ref + elt->start, elt->ref + elt->end );
      OT::String lastPart   ( elt->ref + elt->end, elt->ref + strlen(elt->ref) );
      printToLogDebug( "(printChunk) ... ref   = %s%s->%s%s%s%s%s<-%s%s",
                       firstPart.c_str(),
                       TTY::GetColor(TTY::REDFG), TTY::GetColor(TTY::DEFAULT),
                       TTY::GetColor(TTY::BOLD), middlePart.c_str(), TTY::GetColor(TTY::DEFAULT),
                       TTY::GetColor(TTY::REDFG), TTY::GetColor(TTY::DEFAULT),
                       lastPart.c_str() );
    }
  }
  printToLogDebug( "(printChunk) }" );
}




/* Add an element (chunk) at the end of the linked list */
struct chunk * addChunkToList(struct chunk * list,
                              struct chunk * elt)
{
  printChunk(elt);
  if (list == 0) return elt;
  struct chunk * current = list;
  while (current->next) current = current->next;
  current->next = elt;
  return list;
}




/* Print out the list as a string */
void printList(struct chunk * list)
{
  if (! Log::HasDebug()) return;
  if (list == 0) return;
  if (list->repl) printToLogDebug( "(printList) ->%s<-", list->repl );
  else printToLogDebug( "(printList) ->%*s<-", list->end - list->start, list->ref + list->start  );
  printList(list->next);
}





/* Copy the content of the list as a string into buf. Buf MUST be allocated to a size that can
 * contains the whole list.
 */
void copyList(char * buf,
              struct chunk * list)
{
  char * ptr;

  if (list == 0) return;
  if (list->repl)
  {
    strcpy(buf, list->repl);
    ptr = buf + strlen(list->repl);
  }
  else
  {
    strncpy(buf, &(list->ref[list->start]), list->end - list->start);
    ptr = buf + list->end - list->start;
  }
  copyList(ptr, list->next);
}




/* Return the length of the string that corresponds to the list content */
size_t lengthList(struct chunk * list)
{
  size_t len = 0;

  if (list == 0) return 0;
  len = lengthList(list->next);
  if (list->repl) len += strlen(list->repl);
  else len += list->end - list->start;
  if (Log::HasDebug()) printToLogDebug( "(lengthList) len = %d", len );
  return len;
}





/* Deallocate the elements (chunks) of the list */
void freeList(struct chunk * list)
{
  if (list == 0) return;
  freeList(list->next);
  if (list->repl) free(list->repl);
  free(list);
  if (Log::HasDebug()) printToLogDebug( "(freeList)" );
}




/* Substitute the content of mystring according to myregexp with myreplace.
 * Return a new buffer allocated (malloc) with the content of the replaced string.
 */
char * substitute(const char * mystring,
                  const std::string & origRegexp,
                  const std::string & origReplace,
                  const std::string & newRegexp,
                  const std::string & newReplace,
                  const struct regexp_match regmatch,
                  void * p_error)
{
#ifdef OPENTURNS_HAVE_REGEX
  if (newRegexp.size())   /* Only substitute non null regex */
  {
    regex_t * compiled;
    int cflags;
    size_t nmatch;
    regmatch_t * pmatch;
    struct chunk * list;
    struct chunk * last_elt;
    size_t pos;
    int  rc;
    size_t len;
    size_t len_mystring;
    size_t i, j;
    char * buf;
    int somematch;

    /* build regular expression */
    compiled = (regex_t *) malloc(sizeof(regex_t));
    cflags = REG_EXTENDED | REG_NEWLINE;
    rc = 0;
    if (( rc = regcomp(compiled, newRegexp.c_str(), cflags) ))
    {
      char * msg;
      size_t msg_len;

      msg_len = regerror(rc, compiled, 0, 0);
      msg = (char *) calloc(msg_len, sizeof(char));
      regerror(rc, compiled, msg, msg_len);
      OT::String err;
      err += "Error in compiling regular expression '";
      err += newRegexp;
      err += "' (from '";
      err += origRegexp;
      err += "'). Message is: ";
      err += msg;
      setWrapperError( p_error, err.c_str() );
      free(msg);
      return 0;
    }

    /* scan buffer for matching patterns */
    nmatch = getNumberOfParenthesis(newRegexp) + 1;
    pmatch = (regmatch_t *) calloc(nmatch, sizeof(regmatch_t));
    len_mystring = strlen(mystring);
    pos = 0;
    rc = 0;
    somematch = 0;
    list = 0;
    while ( (pos <= len_mystring) &&
            !(rc = regexec(compiled, mystring + pos, nmatch, pmatch, 0)))
    {
      somematch = 1;

      /* Did something match the regexp ? */
      if ( (pmatch[0].rm_so != -1) && (pmatch[0].rm_eo != -1) )
      {
        struct chunk * elt;
        elt = (struct chunk *) malloc(sizeof(struct chunk));
        elt->next  = 0;
        elt->start = pos;
        elt->end   = pos + pmatch[0].rm_so;
        elt->ref   = mystring;
        elt->repl  = 0;
        list = addChunkToList(list, elt);
        if (Log::HasWrapper())
        {
          OT::String msg(mystring + pos + pmatch[0].rm_so, mystring + pos + pmatch[0].rm_eo);
          printToLogWrapper( "(substitute) %sMatched     '%s'%s", TTY::GetColor(TTY::BOLD), msg.c_str(), TTY::GetColor(TTY::DEFAULT) );
          printToLogWrapper( "(substitute) %sReplaced by '%s'%s", TTY::GetColor(TTY::BOLD), newReplace.c_str(), TTY::GetColor(TTY::DEFAULT) );
        }
      }

      /* Char by char parsing of the replace string to find shorcuts */
      for (i = 0; i < newReplace.size(); /**/)
      {
        j = i;
        while ((newReplace[j] != 0) && (newReplace[j] != '\\')) ++j; /* skipping non shortcuts */

        /* save the string chunk between the last shortcut and the current one */
        if (i != j)
        {
          if (Log::HasDebug()) printToLogDebug( "(substitute) Reading chunk from %d to %d in replace string %s", i, j, newReplace.c_str() );
          struct chunk * elt;
          elt = (struct chunk *) malloc(sizeof(struct chunk));
          elt->next  = 0;
          elt->start = i;
          elt->end   = j;
          elt->ref   = newReplace.c_str();
          elt->repl  = 0;
          list = addChunkToList(list, elt);
        }

        /* Something seems to start with a double backslash. Is it a true shortcut ? */
        if (newReplace[j] == '\\')
        {
          if (Log::HasDebug()) printToLogDebug( "(substitute) Reading \\ in replace string %s", newReplace.c_str() );
          ++j;

          /* not a shortcut but a special character */
          if ( islower(newReplace[j]) )
          {
            if (Log::HasDebug()) printToLogDebug( "(substitute) Reading lower case character '%c' at place %d in replace string %s", newReplace[j], j, newReplace.c_str() );
            struct chunk * elt;
            elt = (struct chunk *) malloc(sizeof(struct chunk));
            elt->next  = 0;
            elt->start = 0;
            elt->end   = 1;
            elt->ref   = 0;
            elt->repl  = (char *) calloc(2, sizeof(char));
            elt->repl[0] = newReplace[j] - 'a';
            elt->repl[1] = 0;
            list = addChunkToList(list, elt);
          }

          /* Dealing with a shortcut. Replace it by its expression (string chunk) in the original parsed string */
          if ( isdigit(newReplace[j]) )
          {
            /* Get the number of the shortcut (\nnn) */
            size_t width = ResourceMap::GetAsUnsignedInteger( "regexp-shortcut-width" );
            char NNN[width + 1];
            strncpy( NNN, newReplace.c_str() + j, width );
            NNN[width] = 0;
            size_t nnn = strtoul( NNN, NULL, 10 );

            if ((nnn > 0) && (nnn <= regmatch.n))
            {
              size_t nnn_in_new_regexp = regmatch.d[nnn - 1].repl_new_paren_ref;

              if (Log::HasDebug()) printToLogDebug( "(substitute) Reading back reference '%s' (value=%d) at place %d in replace string %s",
                                                      NNN, nnn, j, newReplace.c_str() );
              j += width;
              if ((nnn_in_new_regexp > 0) && (nnn_in_new_regexp <= nmatch))
              {
                struct chunk * elt;
                elt = (struct chunk *) malloc(sizeof(struct chunk));
                elt->next  = 0;
                elt->start = pos + pmatch[nnn_in_new_regexp].rm_so;
                elt->end   = pos + pmatch[nnn_in_new_regexp].rm_eo;
                elt->ref   = mystring;
                elt->repl  = 0;
                list = addChunkToList(list, elt);
              }
            }
          }
        }
        i = j;
      } /* end for */



      if ( (pmatch[0].rm_so != -1) && (pmatch[0].rm_eo != -1) )
      {
        pos += pmatch[0].rm_eo;
      }
    } /* end while */

    if ( (rc != REG_NOMATCH) || !somematch)
    {
      char * msg;
      size_t msg_len;

      msg_len = regerror(rc, compiled, 0, 0);
      msg = (char *) calloc(msg_len, sizeof(char));
      regerror(rc, compiled, msg, msg_len);
      if (Log::HasWrapper()) printToLogWrapper( "(substitute) Error in matching regular expression '%s' (from '%s') in string '%s'. Message is: %s%s%s",
            newRegexp.c_str(), origRegexp.c_str(), mystring,
            TTY::GetColor(TTY::BOLD),
            msg,
            TTY::GetColor(TTY::DEFAULT) );
      free(msg);
      freeList(list);
      regfree(compiled);
      free(pmatch);
      free(compiled);
      return 0;
    }

    last_elt = (struct chunk *) malloc(sizeof(struct chunk));
    last_elt->next  = 0;
    last_elt->start = pos;
    last_elt->end   = strlen(mystring);
    last_elt->ref   = mystring;
    last_elt->repl  = 0;
    list = addChunkToList(list, last_elt);

    len = lengthList(list);
    if (Log::HasDebug()) printToLogDebug( "(substitute) length of list = %d", len );
    buf = (char *) calloc(len + 1, sizeof(char));
    memset(buf, 0, len + 1);
    printList(list);
    copyList(buf, list);

    freeList(list);
    regfree(compiled);
    free(pmatch);
    free(compiled);

    return buf;

  }
  else
  {

    return strdup(mystring);

  } /* endif (newRegexp.size()) */


#else /* OPENTURNS_HAVE_REGEX */
  throw NotYetImplementedException(HERE) << "WrapperCommonFunctions need regex";
#endif /* OPENTURNS_HAVE_REGEX */
}




typedef std::map<std::string, std::string> ShortcutMap;




/* Initialize a map of shortcuts */
ShortcutMap getShortcutMap(const struct WrapperExchangedData * p_exchangedData)
{
  ShortcutMap shMap;

  const struct PlatformConfiguration * p_pf = p_exchangedData->platform_;
  shMap[ p_pf->realRegexpShortcut_      ] = ResourceMap::Get("real-regexp");
  shMap[ p_pf->integerRegexpShortcut_   ] = ResourceMap::Get("integer-regexp");
  shMap[ p_pf->separatorRegexpShortcut_ ] = ResourceMap::Get("separator-regexp");

  return shMap;
}





/* Substitute the possibly found shortcuts in the regular expression
 * and set accordingly the regexp_match array to reflect the
 * changes in the regexp.
 */
void substituteShortcuts(const ShortcutMap & shMap,
                         const std::string & origRegexp,
                         const std::string & origReplace,
                         std::string & newRegexp,
                         std::string & newReplace,
                         struct regexp_match regmatch,
                         void * p_error)
{
#ifdef OPENTURNS_HAVE_REGEX
  newRegexp.erase();
  newReplace.erase();

  // we copy the original strings to operate on them
  std::string regexpBeforeSubstitution  = origRegexp;
  std::string replaceBeforeSubstitution = origReplace;

  if (Log::HasDebug()) printToLogDebug( "(substituteShortcuts) working on regexp '%s' and replace string '%s'", origRegexp.c_str(), origReplace.c_str() );

  /* We try to substitute as many times as there are substitutions to do.
   * This is the case when shortcuts contain shortcuts.
   */
  size_t pos1 = 0;
  size_t pos2 = 0;
  size_t pass = 0;

  std::string regexpAtPreviousRound;
  std::string replaceAtPreviousRound;

  do
  {

    /* we do intermediate savings because shortcut substitutions can be nested */
    regexpAtPreviousRound  = regexpBeforeSubstitution;
    replaceAtPreviousRound = replaceBeforeSubstitution;

    pass += 1;
    if (Log::HasDebug()) printToLogDebug( "(substituteShortcuts) %spass #%d%s", TTY::GetColor(TTY::BOLD), pass, TTY::GetColor(TTY::DEFAULT) );
    ShortcutMap::const_iterator it;
    for(it = shMap.begin(); it != shMap.end(); ++it)
    {
      const std::string regexpShortcut  = it->first;
      const std::string shortcutReplace = it->second;

      if (Log::HasDebug())
      {
        printToLogDebug( "(substituteShortcuts) Considering regexpShortcut='%s' to be replaced by shortcutReplace='%s'",
                         regexpShortcut.c_str(), shortcutReplace.c_str() );
        printToLogDebug( "(substituteShortcuts) regexpBeforeSubstitution ='%s'", regexpBeforeSubstitution.c_str() );
        printToLogDebug( "(substituteShortcuts) replaceBeforeSubstitution='%s'", replaceBeforeSubstitution.c_str() );
        printRegexpMatchArray( regmatch );
      }
      newRegexp.erase();
      newReplace.erase();

      /*************************************/
      /* We operate on regular expressions */
      /*************************************/
      pos1 = 0;
      pos2 = 0;
      while ( 1 )
      {
        /* searching shortcut in the regular expression (possibly modified in a previous pass) */
        pos2 = regexpBeforeSubstitution.find( regexpShortcut, pos1 );
        if ( (pos2 == String::npos) || (pos2 > static_cast<size_t>( regexpBeforeSubstitution.size() )) ) break;

        newRegexp.append( regexpBeforeSubstitution, pos1, pos2 - pos1 ); /* We copy the part between two substituted substrings */
        newRegexp.append( shortcutReplace );                           /* We do the substitution */

        /* We translate the values in the regmatch structure */
        /* We have to update the offsets because the shortcut substitution may have modified the string */
        for(size_t i = 0; i < regmatch.n; ++i)
        {
          if (pos2 < regmatch.d[i].reg_tmp_paren_pos)
          {
            /* reg_new_paren_pos = the position of the parenthesis in the new regexp string */
            regmatch.d[i].reg_new_paren_pos  += shortcutReplace.size() - regexpShortcut.size();
            /* repl_new_paren_ref = the index of the parenthesis in the new regexp string */
            regmatch.d[i].repl_new_paren_ref += getNumberOfParenthesis( shortcutReplace );
          } // end if
        } // end for

        pos1 = pos2 + regexpShortcut.size();

        if (Log::HasDebug()) printRegexpMatchArray( regmatch );
      } /* end while ( 1 ) */

      newRegexp.append( regexpBeforeSubstitution, pos1, regexpBeforeSubstitution.size() - pos1 ); /* We copy the tail */


      /*********************************/
      /* We operate on replace strings */
      /*********************************/
      pos1 = 0;
      pos2 = 0;

      /* build regular expression */
      regex_t * compiled = (regex_t *) malloc(sizeof(regex_t));
      int cflags = REG_EXTENDED | REG_NEWLINE;
      int rc = 0;
      //char re[] = "\\\\[0-9]+";
      size_t width = ResourceMap::GetAsUnsignedInteger( "regexp-shortcut-width" );
      char * re = newFormattedString( "^\\\\[0-9]{%u}$", width );
      if (( rc = regcomp(compiled, re, cflags) )) /* We look for the \nnn back references */
      {
        char * msg;
        size_t msg_len;

        msg_len = regerror(rc, compiled, 0, 0);
        msg = (char *) calloc(msg_len, sizeof(char));
        regerror(rc, compiled, msg, msg_len);
        OT::String err;
        err += "Error in compiling regular expression '";
        err += re;
        err += "'. Message is: ";
        err += msg;
        setWrapperError( p_error, err.c_str() );
        free(msg);
        return;
      } // end if
      free( re );
      re = 0;

      /* scan buffer for matching patterns */
      regmatch_t pmatch;
      size_t len = replaceBeforeSubstitution.size();
      while (pos1 <= len)
      {
        if (( rc = regexec(compiled, replaceBeforeSubstitution.c_str() + pos1, 1, &pmatch, 0) ))
        {
#ifdef DEBUG
          //        char * msg;
          //        size_t msg_len;

          //        msg_len = regerror(rc, compiled, 0, 0);
          //        msg = (char *) calloc(msg_len, sizeof(char));
          //        regerror(rc, compiled, msg, msg_len);
          //        fprintf(stderr,
          //                "Error in matching regular expression '" <<  << "' in string '" <<  << "'. Message is: ",
          //                re,
          //                replaceBeforeSubstitution.c_str()+pos1,
          //                msg);
          //        free(msg);
#endif
          break;
        }

        if ( (pmatch.rm_so != -1) && (pmatch.rm_eo != -1) )
        {
          /* We match \nnn in the replace string */
          /* We read the value matched and look in the regmatch struct for the corresponding value */
          pos2 = pos1 + pmatch.rm_so + 1; /* one is for the slash */
          size_t nnn = strtoul(replaceBeforeSubstitution.c_str() + pos2, 0, 0);
          size_t new_nnn = 0;
          for(size_t i = 0; i < regmatch.n; ++i)
          {
            if (regmatch.d[i].repl_init_paren_ref == nnn)
            {
              new_nnn = regmatch.d[i].repl_new_paren_ref;
              break;
            }
          } /* end for */

          /* We do the substitutions */
          width = ResourceMap::GetAsUnsignedInteger( "regexp-shortcut-width" );
          char NNN[width + 2];
          snprintf( NNN, width + 2, "%0*u", static_cast<int>(width), static_cast<unsigned int>(new_nnn) );
          newReplace.append( replaceBeforeSubstitution, pos1, pos2 - pos1 ); /* We copy the part between two substituted substrings */
          newReplace.append( NNN );

          pos1 = pos1 + pmatch.rm_eo;
        } /* end if */
      } /* end while */

      newReplace.append( replaceBeforeSubstitution, pos1, replaceBeforeSubstitution.size() - pos1 ); /* We copy the tail */

      regfree( compiled );
      free( compiled );

      if (Log::HasDebug())
      {
        printToLogDebug( "(substituteShortcuts) newRegexp                ='%s'", newRegexp.c_str() );
        printToLogDebug( "(substituteShortcuts) newReplace               ='%s'", newReplace.c_str() );
        //printRegexpMatchArray( regmatch );
      }

      regexpBeforeSubstitution  = newRegexp;
      //replaceBeforeSubstitution = newReplace;
      /* We translate the values in p_regmatch structure (update of the temporary values) */
      for(size_t i = 0; i < regmatch.n; ++i)
      {
        regmatch.d[i].reg_tmp_paren_pos  = regmatch.d[i].reg_new_paren_pos;
        regmatch.d[i].repl_tmp_paren_ref = regmatch.d[i].repl_new_paren_ref;
      } // end for

    } /* end for */

    /* We leave the loop when no more substitutions were done */
  }
  while (regexpAtPreviousRound != newRegexp);   /* end do while */

  if (Log::HasDebug()) printToLogDebug( "(substituteShortcuts) build new regexp '%s' and replace string '%s'", newRegexp.c_str(), newReplace.c_str() );

#else /* OPENTURNS_HAVE_REGEX */
  throw NotYetImplementedException(HERE) << "WrapperCommonFunctions need regex";
#endif /* OPENTURNS_HAVE_REGEX */
}






/* Substitute all the variables in p_variableList in buf.
 * Buf may be realloc-ated to hold the new string (if bigger).
 */
char * substituteVariables(char * buf,
                           const struct WrapperExchangedData * p_exchangedData,
                           const char * subst,
                           const struct point * p_point,
                           long & sizeDiff,
                           void * p_error)
{
  char * new_buf = 0;
  size_t coord = 0;
  size_t initialSize = strlen( buf );

  std::set<OT::String> substSet = getSetFromSubst( subst );

  const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
  while (currentVariableElement)
  {
    //printToLogTrace( "(substituteVariables) Variable %s ?", currentVariableElement->variable_->id_ );

    if ( ( currentVariableElement->variable_->type_ == WRAPPER_IN ) ||
         ( currentVariableElement->variable_->type_ == WRAPPER_INTERNAL ) )
    {

      if ( !subst || inSubst( substSet, currentVariableElement->variable_->id_ ) )
      {

        if (Log::HasWrapper())
        {
          printToLogWrapper( "(substituteVariables) %sConsidering variable %s%s",
                             TTY::GetColor(TTY::BOLD), currentVariableElement->variable_->id_, TTY::GetColor(TTY::DEFAULT) );
        }

        // The string orig_replace will contain the substituted value of p_point. Its size is computed dynamically.
        size_t len = strlen(currentVariableElement->variable_->format_);
        char * orig_replace = newFormattedString( currentVariableElement->variable_->format_, p_point->data_[coord] );
        size_t bytes = strlen( orig_replace );

        if (Log::HasDebug())
        {
          printToLogDebug( "(substituteVariables) format_     ='%s' (%d bytes) will be replaced by", currentVariableElement->variable_->format_, len );
          printToLogDebug( "(substituteVariables) orig_replace='%s' (%d bytes)", orig_replace, bytes );
        }

        std::string origRegexp  = currentVariableElement->variable_->regexp_;
        std::string origReplace = orig_replace;
        std::string newRegexp;
        std::string newReplace;

        free(orig_replace);

        // We substitute the shortcuts in the newly created origReplace string (= orig_replace)
        ShortcutMap shMap = getShortcutMap( p_exchangedData );
        struct regexp_match regmatch = createRegexpMatchArray( origRegexp );
        substituteShortcuts( shMap, origRegexp, origReplace, newRegexp, newReplace, regmatch, p_error );

        new_buf = substitute(buf, origRegexp, origReplace, newRegexp, newReplace, regmatch, p_error );
        freeRegexpMatchArray( regmatch );
        if (new_buf)
        {
          free(buf);
          buf = new_buf;
        }
      } // if ( !subst || inSubst( substSet, currentVariableElement->variable_->id_ ) )

      if ( currentVariableElement->variable_->type_ == WRAPPER_IN ) ++coord;
    }

    currentVariableElement = currentVariableElement->next_;
  }

  size_t finalSize = strlen( buf );
  sizeDiff = finalSize - initialSize;
  return buf;
}






/* Substitute all the variables in p_variableList in buf.
 * Buf may be realloc-ated to hold the new string (if bigger).
 */
char * substituteVariablesOnCommandLine(char * buf,
                                        const struct WrapperExchangedData * p_exchangedData,
                                        const char * subst,
                                        const struct point * p_point,
                                        long & sizeDiff,
                                        void * p_error)
{
  char * new_buf = 0;
  size_t coord = 0;
  size_t initialSize = strlen( buf );

  std::set<OT::String> substSet = getSetFromSubst( subst );

  const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
  while (currentVariableElement)
  {
    //printToLogTrace( "(substituteVariablesOnCommandLine) Variable %s ?", currentVariableElement->variable_->id_ );

    if ( ( currentVariableElement->variable_->type_ == WRAPPER_IN ) ||
         ( currentVariableElement->variable_->type_ == WRAPPER_INTERNAL ) )
    {

      if ( !subst || inSubst( substSet, currentVariableElement->variable_->id_ ) )
      {

        if (Log::HasWrapper())
        {
          printToLogWrapper( "(substituteVariablesOnCommandLine) %sConsidering variable %s%s",
                             TTY::GetColor(TTY::BOLD), currentVariableElement->variable_->id_, TTY::GetColor(TTY::DEFAULT) );
        }

        OT::OSS orig_replace;
        orig_replace << "$OPENTURNS_WRAPPERVARIABLE_" << currentVariableElement->variable_->id_;

        std::string origRegexp  = currentVariableElement->variable_->regexp_;
        std::string origReplace = orig_replace.str();
        std::string newRegexp;
        std::string newReplace;

        // We substitute the shortcuts in the newly created origReplace string (= orig_replace)
        ShortcutMap shMap = getShortcutMap( p_exchangedData );
        struct regexp_match regmatch = createRegexpMatchArray( origRegexp );
        substituteShortcuts( shMap, origRegexp, origReplace, newRegexp, newReplace, regmatch, p_error );

        new_buf = substitute(buf, origRegexp, origReplace, newRegexp, newReplace, regmatch, p_error );
        freeRegexpMatchArray( regmatch );
        if (new_buf)
        {
          free(buf);
          buf = new_buf;
        }
      } // if ( !subst || inSubst( substSet, currentVariableElement->variable_->id_ ) )

      if ( currentVariableElement->variable_->type_ == WRAPPER_IN ) ++coord;
    }

    currentVariableElement = currentVariableElement->next_;
  }

  size_t finalSize = strlen( buf );
  sizeDiff = finalSize - initialSize;
  return buf;
}






/* Look for origRegexp in mystring and parse it as if it was a double. Returns non zero if not found or failure */
int retrieve(const std::string & mystring,
             const std::string & origRegexp,
             const std::string & origFormat,
             const std::string & newRegexp,
             const std::string & newFormat,
             const struct regexp_match regmatch,
             double & value,
             void * p_error)
{
#ifdef OPENTURNS_HAVE_REGEX
  regex_t * compiled;
  int cflags;
  size_t nmatch;
  regmatch_t * pmatch;
  int rc;
  size_t parenthesis;
  size_t nbParenthesis;

  /* build regular expression */
  compiled = (regex_t *) malloc(sizeof(regex_t));
  cflags = REG_EXTENDED | REG_NEWLINE;
  rc = 0;
  if (( rc = regcomp(compiled, newRegexp.c_str(), cflags) ))
  {
    char * msg;
    size_t msg_len;

    msg_len = regerror(rc, compiled, 0, 0);
    msg = (char *) calloc(msg_len, sizeof(char));
    regerror(rc, compiled, msg, msg_len);
    OT::String err;
    err += "Error in compiling regular expression '";
    err += newRegexp;
    err += "' (from '";
    err += origRegexp;
    err += "'). Message is: ";
    err += msg;
    setWrapperError( p_error, err.c_str() );
    free(msg);
    return 1;
  }

  /* read parenthesis to be extracted */
  /* We expect a format like '\nnn' where nnn is the number of the parenthesis we have to extract */
  /* The size of nnn is determined by the value of "regexp-shortcut-width" */
  regex_t * expr = (regex_t *) malloc(sizeof(regex_t));
  size_t width = ResourceMap::GetAsUnsignedInteger( "regexp-shortcut-width" );
  char * re = newFormattedString( "^\\\\[0-9]{%u}$", width );
  char NNN[width + 1];
  memset( NNN, 'n', width );
  regcomp(expr, re, REG_EXTENDED);
  if (( rc = regexec(expr, newFormat.c_str(), 0, 0, 0) ))
  {
    OT::String err;
    err += "Error in matching format expression '";
    err += newFormat;
    err += "' (from '";
    err += origFormat;
    err += "'). Should be of the form '\\";
    err += NNN;
    err += "' where ";
    err += NNN;
    err += " is the number of the parenthesis you want to extract (re = '";
    err += re;
    err += "')";
    setWrapperError( p_error, err.c_str() );
    regfree(expr);
    free(expr);
    regfree(compiled);
    free(compiled);
    return 1;
  }

  parenthesis = strtoul(newFormat.c_str() + 1, 0, 0);
  regfree(expr);
  free(expr);


  /* scan buffer for matching patterns */
  nbParenthesis = getNumberOfParenthesis(newRegexp);
  if (parenthesis > nbParenthesis)
  {
    OT::String err;
    err += "Error between regular expression '";
    err += newRegexp;
    err += "' and format '";
    err += newFormat;
    err += "' (from '";
    err += origRegexp;
    err += "'and '";
    err += origFormat;
    err += "'). The number of parenthesis seen in expression does not match the value read in format";
    setWrapperError( p_error, err.c_str() );
    regfree(compiled);
    free(compiled);
    return 1;
  }
  nmatch = nbParenthesis + 1;
  pmatch = (regmatch_t *) calloc(nmatch, sizeof(regmatch_t));
  rc = 0;
  if (!(rc = regexec(compiled, mystring.c_str(), nmatch, pmatch, 0)))
  {
    if ( (pmatch[parenthesis].rm_so != -1) && (pmatch[parenthesis].rm_eo != -1) )
    {
      char * wholeMatched = strndup( mystring.c_str() + pmatch[0].rm_so,
                                     pmatch[0].rm_eo - pmatch[0].rm_so );
      char * matched = strndup( mystring.c_str() + pmatch[parenthesis].rm_so,
                                pmatch[parenthesis].rm_eo - pmatch[parenthesis].rm_so );
      value = strtod(matched, 0);

      if (Log::HasWrapper()) printToLogWrapper( "(retrieve) %sMatched '%s' -> %s=%s -> value=%.16g )%s",
            TTY::GetColor(TTY::BOLD),
            wholeMatched, origFormat.c_str(), matched, value,
            TTY::GetColor(TTY::DEFAULT) );
      free( matched      );
      free( wholeMatched );
    }
  }

  if ( rc )
  {
    char * msg;
    size_t msg_len;

    msg_len = regerror(rc, compiled, 0, 0);
    msg = (char *) calloc(msg_len, sizeof(char));
    regerror(rc, compiled, msg, msg_len);

    if (Log::HasDebug())
    {
      OT::String err;
      err += "Error in matching regular expression '";
      err += newRegexp;
      err += "' (from '";
      err += origRegexp;
      err += "') in string '";
      err += mystring;
      err += "'. Message is: ";
      err += msg;
      setWrapperError( p_error, err.c_str() );
    }

    free(msg);
    regfree(compiled);
    free(pmatch);
    free(compiled);
    return 0;
  }

  regfree(compiled);
  free(pmatch);
  free(compiled);

  return 0;

#else /* OPENTURNS_HAVE_REGEX */
  throw NotYetImplementedException(HERE) << "WrapperCommonFunctions need regex";
#endif /* OPENTURNS_HAVE_REGEX */
}






/* Make a set from file subst tag */
std::set<OT::String> getSetFromSubst( const char * subst )
{
  std::set<OT::String> aSet;
  if ( !subst || !strcmp(subst, "") ) return aSet;

  const char * comma = subst;
  const char * next  = comma;
  while (true)
  {
    next = strchr(comma, ',');
    if (next == NULL)
    {
      aSet.insert( comma );
      break;
    }
    aSet.insert( OT::String( comma, next - comma ) );
    comma = next + 1;
  }

  return aSet;
}




/* Tell if variable is mentionned in subst string */
bool inSubst( const std::set<OT::String> & substSet,
              const char * name )
{
  return !substSet.empty() && strcmp(name, "") && ( substSet.find( name ) != substSet.end() );
}




/* Pick up data from the file */
int retrieveVariables(char * buf,
                      const struct WrapperExchangedData * p_exchangedData,
                      const char * subst,
                      struct point * p_point,
                      void * p_error)
{
  int coord = 0;

  std::set<OT::String> substSet = getSetFromSubst( subst );

  const struct WrapperVariableList * currentVariableElement = p_exchangedData->variableList_;
  while (currentVariableElement)
  {
    if ( currentVariableElement->variable_->type_ == WRAPPER_OUT )
    {

      if ( !subst || inSubst( substSet, currentVariableElement->variable_->id_ ) )
      {

        if (Log::HasWrapper())
        {
          printToLogWrapper( "(retrieveVariables) %sConsidering variable %s%s",
                             TTY::GetColor(TTY::BOLD), currentVariableElement->variable_->id_, TTY::GetColor(TTY::DEFAULT) );
        }

        std::string origRegexp = currentVariableElement->variable_->regexp_;
        std::string origFormat = currentVariableElement->variable_->format_;
        std::string newRegexp;
        std::string newFormat;

        if ( origFormat.size() == 0 )
        {
          size_t width = ResourceMap::GetAsUnsignedInteger( "regexp-shortcut-width" );
          char * NNN = new char[width + 2];
          snprintf( NNN, width + 2, "\\%0*u", static_cast<int>(width), 1 );
          origFormat = NNN;
          delete [] NNN;
          //LOGTRACE(OSS() << "origFormat='" << origFormat << "'");
        }

        ShortcutMap shMap = getShortcutMap( p_exchangedData );
        struct regexp_match regmatch = createRegexpMatchArray( origRegexp );
        substituteShortcuts( shMap, origRegexp, origFormat, newRegexp, newFormat, regmatch, p_error );

        double value = 0.;
        int rc = retrieve(buf, origRegexp, origFormat, newRegexp, newFormat, regmatch, value, p_error );
        if (rc && Log::HasWarn())
        {
          printToLogWarn( "(retrieveVariables) %sVariable %s could NOT be retrieved from file. Reason: %s%s",
                          TTY::GetColor(TTY::BOLD),
                          currentVariableElement->variable_->id_,
                          getWrapperError( p_error ).c_str(),
                          TTY::GetColor(TTY::DEFAULT) );
        }
        p_point->data_[coord] = value;
        freeRegexpMatchArray( regmatch );

        if (Log::HasWrapper())
          printToLogWrapper( "(retrieveVariables) %sFound value for variable %s = %g%s",
                             TTY::GetColor(TTY::BOLD),
                             currentVariableElement->variable_->id_, value,
                             TTY::GetColor(TTY::DEFAULT) );
      }
      ++coord;
    }
    currentVariableElement = currentVariableElement->next_;
  }

  /* OK */
  return 0;
}





#ifndef WIN32
#define IS_READABLE_FOR_ME(s, uid)       ( ( (s).st_mode & S_IRUSR ) && ( (s).st_uid == (uid) ) )
#define IS_READABLE_FOR_MY_GROUP(s, gid) ( ( (s).st_mode & S_IRGRP ) && ( (s).st_gid == (gid) ) )
#define IS_READABLE_FOR_ANYBODY(s)         ( (s).st_mode & S_IROTH )
#define IS_READABLE(s,uid,gid) (IS_READABLE_FOR_ME( (s) , (uid) ) || IS_READABLE_FOR_MY_GROUP( (s) , (gid) ) || IS_READABLE_FOR_ANYBODY( (s) ))
#define IS_WRITABLE_FOR_ME(s, uid)       ( ( (s).st_mode & S_IWUSR ) && ( (s).st_uid == (uid) ) )
#define IS_WRITABLE_FOR_MY_GROUP(s, gid) ( ( (s).st_mode & S_IWGRP ) && ( (s).st_gid == (gid) ) )
#define IS_WRITABLE_FOR_ANYBODY(s)         ( (s).st_mode & S_IWOTH )
#define IS_WRITABLE(s,uid,gid) (IS_WRITABLE_FOR_ME( (s) , (uid) ) || IS_WRITABLE_FOR_MY_GROUP( (s) , (gid) ) || IS_WRITABLE_FOR_ANYBODY( (s) ))
#define IS_READWRITABLE(s,uid,gid) ( IS_READABLE( (s) , (uid) , (gid) ) && IS_WRITABLE( (s) , (uid) , (gid) ) )
#endif





/* Check that the directory is usable (existence and read/write access) */
int checkDirectory(const char * directory,
                   void * p_error)
{
  struct stat dir_stat;
  int rc = 0;

  //IDM
  // LOGTRACE( OT::String("IDM - ??? dir  : ") + directory );

  /* Check if the directory exists and if we can read/write to it. Exits otherwise */
  int myerrno UNUSED;
  FSLOCK( rc = stat(directory, &dir_stat); myerrno = errno; );
  if (rc < 0)
  {
    char * msg;
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE == 600)
    char err[BUFFER_LENGTH];
    strerror_r( myerrno, err, BUFFER_LENGTH );
    msg = newFormattedString( "(checkDirectory) Directory or file %s does NOT exists. Reason: %s", directory, err );
#else
    msg = newFormattedString( "(checkDirectory) Directory or file %s does NOT exists", directory );
#endif
    setWrapperError( p_error, msg );
    free( msg );
    return 1;
  }
  if (!S_ISDIR(dir_stat.st_mode))
  {
    char * msg = newFormattedString( "(checkDirectory) %s is NOT a directory", directory );
    setWrapperError( p_error, msg );
    free( msg );
    return 1;
  }
#ifndef WIN32
  uid_t my_euid = geteuid();
  gid_t my_egid = getegid();
  if (!IS_READWRITABLE(dir_stat, my_euid, my_egid))
  {
    char * msg = newFormattedString( "(checkDirectory) %s is NOT readable or writable for uid=%d gid=%d", directory, my_euid, my_egid );
    setWrapperError( p_error, msg );
    free( msg );
    return 1;
  }
#endif

  /* OK */
  return 0;
}






#ifndef WIN32
/* Run the command passed as argument in the specified directory. Set its environment approprietaly */
int runCommand(const char * command,
               const char * directory,
               const struct WrapperExchangedData * p_exchangedData,
               const struct point * p_point,
               void * p_error)
{
  int childrc = 0;

  // Prepare everything for the fork (to avoid race between fork and pthreads)
  const char * program = command;
  const char * args[2];
  args[0] = command;
  args[1] = NULL;

  // compute the size of the environ variable
  int envSize = 0;
  while (environ[envSize++] != 0) /* loop */;

  envSize += _getNumberOfVariables( p_exchangedData, WRAPPER_IN  );
  envSize += _getNumberOfFiles( p_exchangedData, WRAPPER_IN  );
  envSize += _getNumberOfFiles( p_exchangedData, WRAPPER_OUT );
  envSize += 1; // OPENTURNS_EXECUTION_DIR

  char * env[envSize];

  int envCur = 0;
  while (environ[envCur] != 0)
  {
    env[envCur] = strdup(environ[envCur]);
    ++envCur;
  }

  int idx = 0;
  struct WrapperVariableList * curV = p_exchangedData->variableList_;
  while (curV && curV->variable_->type_ == WRAPPER_IN)
  {
    OT::OSS var;
    var << "OPENTURNS_WRAPPERVARIABLE_";
    var << curV->variable_->id_;
    var << "=";
    var << p_point->data_[idx++];
    env[envCur++] = strdup( var.str().c_str() );
    curV = curV->next_;
  }

  struct WrapperFileList * curF = p_exchangedData->fileList_;
  while (curF)
  {
    OT::String var = "OPENTURNS_WRAPPERFILE_";
    var += curF->file_->id_;
    var += "=";
    var += curF->file_->path_;
    env[envCur++] = strdup( var.c_str() );
    curF = curF->next_;
  }

  OT::String execDir = "OPENTURNS_EXECUTION_DIR=";
  execDir += directory;
  env[envCur++] = strdup( execDir.c_str() );

  env[envCur++] = NULL;

  // Stream redirection
  OT::String newStdin;
  newStdin  = directory;
  newStdin += Os::GetDirectorySeparator();
  newStdin += "stdin";
  FSLOCK( int fdin = open( newStdin.c_str(),
                           O_WRONLY | O_CREAT | O_TRUNC
#if defined(O_DIRECT) // not available on osx
                           | O_DIRECT
#endif
                           | O_SYNC,
                           S_IRUSR | S_IWUSR );
          fsync( fdin );
          close( fdin ); );


  OT::String newStdout;
  newStdout  = directory;
  newStdout += Os::GetDirectorySeparator();
  newStdout += "stdout";


  OT::String newStderr;
  newStderr  = directory;
  newStderr += Os::GetDirectorySeparator();
  newStderr += "stderr";


  // Dump environment to log file
  OT::String logData;
  logData += "# This file is automatically generated by OpenTURNS\n"
             "# It contains the envvar used to launch the batch script\n"
             "\n";
  for(int v = 0; v < envSize - 1; ++v)
  {
    int lg = strlen(env[v]);
    char * E = static_cast<char *>( calloc( lg, sizeof(char) + 3 ) ); // for quotes and nul char
    char * equal = strchr( env[v], '=' );
    int sz = equal - env[v];
    strncpy( E, env[v], sz + 1 ); // we copy until the equal sign
    E[sz + 1] = '"';
    strncpy( E + sz + 2, env[v] + sz + 1, lg - sz ); // we copy until the end
    E[lg + 1] = '"';

    logData += E;
    logData += "\n";
    free(E);
  }
  OT::String logFile = command;
  logFile += ".log";
  FSLOCK( FILE * fdlog = fopen( logFile.c_str(), "w" );
          fprintf( fdlog, "%s", logData.c_str() );
          fflush( fdlog );
          fclose( fdlog ); );


  // fork the process, pid should contain child's pid
  pid_t pid = fork();

  // Switch according to process
  if (pid == static_cast<pid_t>(0))   // the child side
  {
    // Log restarts automaticaly after closing
    Log::Reset();

    // Stream redirection
    stdin  = freopen( newStdin.c_str(),  "r", stdin  );
    stdout = freopen( newStdout.c_str(), "w", stdout );
    stderr = freopen( newStderr.c_str(), "w", stderr );

#ifdef SLOW_FILESYSTEM
    // Filesystem lock destroyed to avoid future deadlocks in case fork() happened during critical section and
    // mutex remains locked
    pthread_mutex_destroy( &FileSystemMutex );
#endif

    // Wait for 1ms to ensure that the filesystem has been updated.
    //     struct timeval tv;
    //     tv.tv_sec  = 0;
    //     tv.tv_usec = 10000;
    //     select(0, NULL, NULL, NULL, &tv);

    int rc = execve( program, (char * const *) args, (char * const *) env ); // should never return

    char * msg = newFormattedString( "(runCommand) execve error for file %s with rc=%d", command, rc );
    perror( msg );
    free( msg );
    fflush( NULL );

    exit(33);

  }
  else if (pid < static_cast<pid_t>(0))     // fork failure
  {
    while( envSize ) free( const_cast<char *>(env[--envSize]) );
    setWrapperError( p_error, "Fork failed" );
    return -1;

  }
  else     // the father side
  {
    while( envSize ) free( const_cast<char *>(env[--envSize]) );
    int status = 0;
    waitpid( pid, &status, 0 ); // wait for this child to terminate
    if (WIFEXITED(status)) childrc = WEXITSTATUS(status);
    else
    {
      OT::String err;
      char PID[64];
      sprintf( PID, "%d", pid );
      err += "Child (pid=";
      err += PID;
      err += ") exited abnormally. Executed command was: ";
      err += command;
      setWrapperError( p_error, err.c_str() );
    }
    return childrc;
  }
}
#endif /* WIN32 */


