//                                               -*- C++ -*-
/**
 * @brief This file provides function tools for the wrapper library
 *
 * Copyright 2005-2015 Airbus-EDF-IMACS-Phimeca
 *
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 *
 *
 */

#ifndef OPENTURNS_WRAPPERCOMMONFUNCTIONS_HXX
#define OPENTURNS_WRAPPERCOMMONFUNCTIONS_HXX

#include "OTconfig.hxx"
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>   // for va_list
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#ifdef OPENTURNS_HAVE_REGEX_H
# include <regex.h>
#endif
#ifdef OPENTURNS_HAVE_SIGNAL_H
# include <signal.h>
#endif

#include <string>
#include <sstream>
#include <map>
#include <set>
#include "OTdebug.h"
#include "OTprivate.hxx"
#include "OTtypes.hxx"
#include "WrapperInterface.h"
#include "OTthread.hxx"

/* Execute 'code' under filesystem lock. Warning: 'code' MUST NOT THROW any exception */
#ifdef SLOW_FILESYSTEM
/* This mutex is used to lock all filesystem access to avoid races */
extern pthread_mutex_t FileSystemMutex;

class OT_API FileSystemMutex_init
{
  static void Initialization();
public:
  FileSystemMutex_init();
};

static FileSystemMutex_init __FileSystemMutex_initializer;

#define FSLOCK(code) do { pthread_mutex_lock(&FileSystemMutex); code; pthread_mutex_unlock(&FileSystemMutex); } while (0)
#else
#define FSLOCK(code) code
#endif

#if !defined(__MALLOC_P)
#define __MALLOC_P(args) args
#endif

/* Build a newly allocated string in the style of printf */
extern OT_API char * newFormattedString(const char * format, ...);
extern OT_API char * vnewFormattedString(const char * format, va_list args);

/* Write a message in the OpenTURNS log */
extern OT_API void printToLogWrapper(const char * format, ...);

/* Write a user message in the OpenTURNS log */
extern OT_API void printToLogDebug(const char * format, ...);

/* Write an user message in the OpenTURNS log */
extern OT_API void printToLogUser(const char * format, ...);

/* Write an info message in the OpenTURNS log */
extern OT_API void printToLogInfo(const char * format, ...);

/* Write an warning in the OpenTURNS log */
extern OT_API void printToLogWarn(const char * format, ...);

/* Write an error message in the OpenTURNS log */
extern OT_API void printToLogError(const char * format, ...);

/* Write an trace message in the OpenTURNS log */
extern OT_API void printToLogTrace(const char * format, ...);

/* Some functions that help debugging memory allocation */
/* Allocate SIZE bytes of memory.  */
extern OT_API void* dbg_malloc __MALLOC_P ((int line, size_t __size)) UNUSED;

/* Allocate NMEMB elements of SIZE bytes each, all initialized to 0.  */
extern OT_API void* dbg_calloc __MALLOC_P ((int line, size_t __nmemb, size_t __size)) UNUSED;

/* Re-allocate the previously allocated block in __ptr, making the new
   block SIZE bytes long.  */
extern OT_API void* dbg_realloc __MALLOC_P ((int line, void* __ptr, size_t __size)) UNUSED;

/* Free a block allocated by `malloc', `realloc' or `calloc'.  */
extern OT_API void dbg_free __MALLOC_P ((int line, void* __ptr)) UNUSED;


#ifdef DEBUG_MEMORY
#define malloc(sz)      dbg_malloc( __LINE__, (sz) )
#define calloc(nb,sz)   dbg_calloc( __LINE__, (nb), (sz) )
#define realloc(ptr,sz) dbg_realloc( __LINE__, (ptr), (sz) )
#define free(ptr)       dbg_free( __LINE__, (ptr) )
#endif


#ifndef MAX_ERRORS
#define MAX_ERRORS 1000
#endif

static const size_t BUFFER_LENGTH = 2048;

/* Get the number of variables of some type (in or out) in the data read from the description file */
extern OT_API size_t _getNumberOfVariables(const struct WrapperExchangedData * p_exchangedData, unsigned long type);

/* Get the number of files of some type (in or out) in the data read from the description file */
extern OT_API size_t _getNumberOfFiles(const struct WrapperExchangedData * p_exchangedData, unsigned long type);

/* Get a pointer to a variable structure. NULL if not found */
extern OT_API const struct WrapperVariableListElement * _getVariableById(const struct WrapperExchangedData * p_exchangedData, const char * id);

/* Get a pointer to a file structure. NULL if not found */
extern OT_API const struct WrapperFileListElement * _getFileById(const struct WrapperExchangedData * p_exchangedData, const char * id);

/* Create a directory and all its parents if needed. Return non-null value on error */
extern OT_API int createDirectory(const char * directory, void * p_error);

/* Delete a directory and all its content. Return non-null value on error */
extern OT_API int deleteDirectory(const char * directory, void * p_error);

/* Make a set from file subst tag */
extern OT_API std::set<OT::String> getSetFromSubst(const char * subst);

/* Tell if variable is mentionned in subst string */
extern OT_API bool inSubst( const std::set<OT::String> & substSet, const char * name );

/* Read file pointed by path and allocate a buffer that contains all its data.
 * Put the stat of file in p_file_stat;
 */
extern OT_API char * readFile(const char * path, struct stat * p_file_stat, void * p_error, int timeout);

/* Write the content of buf into file pointed by path. Use file_stat to set the mode */
extern OT_API int writeFile(const char * path, const char * buf, struct stat file_stat, void * p_error);


/* Management of errors emited by the wrapper */
struct OT_API WrapperError
{
  pthread_mutex_t mutex;
  int length;
  char * message;
};

extern OT_API void setWrapperError(void * p_error, const OT::String & msg);
extern OT_API void clearWrapperError(void * p_error);
extern OT_API OT::String getWrapperError(void * p_error);



/* This structure saves the position of the shortcuts
 * in the regexp after they have been substituted.
 * A reference is the number of the parenthesis either
 * in the regular expression (the number of the parenthesis)
 * or in the replace string (seen as a backward reference \nnn).
 */
struct OT_API regexp_match_data
{
  size_t reg_init_paren_pos;
  size_t repl_init_paren_ref;
  size_t reg_tmp_paren_pos;
  size_t repl_tmp_paren_ref;
  size_t reg_new_paren_pos;
  size_t repl_new_paren_ref;
};
struct OT_API regexp_match
{
  size_t n;
  struct regexp_match_data * d;
};



/* Print a regexp_match array */
extern OT_API void printRegexpMatchArray(const struct regexp_match regmatch);

/* Allocate and set the regexp_match array with the corresponding reference of sub-expressions */
extern OT_API struct regexp_match createRegexpMatchArray(const std::string & myregexp);

/* Free the storage allocated for a regexp_match structure */
extern OT_API void freeRegexpMatchArray(const struct regexp_match regmatch);




/* A structure that represents a piece of string either in the source string (ref) or the replace
 * string (repl);
 */
struct OT_API chunk
{
  struct chunk * next;
  size_t start;
  size_t end;
  const char * ref;
  char * repl;
};

extern OT_API void printChunk(struct chunk * elt);

/* Add an element (chunk) at the end of the linked list */
extern OT_API struct chunk * addChunkToList(struct chunk * list, struct chunk * elt);

/* Print out the list as a string */
extern OT_API void printList(struct chunk * list);

/* Copy the content of the list as a string into buf. Buf MUST be allocated to a size that can
 * contains the whole list.
 */
extern OT_API void copyList(char * buf, struct chunk * list);

/* Return the length of the string that corresponds to the list content */
extern OT_API size_t lengthList(struct chunk * list);

/* Deallocate the elements (chunks) of the list */
extern OT_API void freeList(struct chunk * list);



/* Substitute the content of mystring according to myregexp with myreplace.
 * Return a new buffer allocated (malloc) with the content of the replaced string.
 */
extern OT_API char * substitute(const char * mystring,
                                const std::string & origRegexp,
                                const std::string & origReplace,
                                const std::string & newRegexp,
                                const std::string & newReplace,
                                const struct regexp_match regmatch,
                                void * p_error);

typedef std::map<std::string, std::string> ShortcutMap;


/* Protect access to the wrapper exchanged data structure */
extern OT_API void lock(const struct WrapperExchangedData * p_exchangedData);
extern OT_API void unlock(const struct WrapperExchangedData * p_exchangedData);
extern OT_API void initMutex(const struct WrapperExchangedData * p_exchangedData);
extern OT_API void destroyMutex(const struct WrapperExchangedData * p_exchangedData);

#define LOCK            do { lock(p_exchangedData);
#define UNLOCK          unlock(p_exchangedData); } while (0);
#define UNLOCK_ON_ERROR unlock(p_exchangedData);

/* Initialize a map of shortcuts */
extern OT_API ShortcutMap getShortcutMap(const struct WrapperExchangedData * p_exchangedData);

/* Substitute the possibly found shortcuts in the regular expression
 * and set accordingly the regexp_match array to reflect the
 * changes in the regexp.
 */
extern OT_API void substituteShortcuts(const ShortcutMap & shMap,
                                       const std::string & origRegexp,
                                       const std::string & origReplace,
                                       std::string & newRegexp,
                                       std::string & newReplace,
                                       struct regexp_match regmatch,
                                       void * p_error);


/* Substitute all the variables in p_variableList in buf.
 * Buf may be realloc-ated to hold the new string (if bigger).
 */
extern OT_API char * substituteVariables(char * buf,
    const struct WrapperExchangedData * p_exchangedData,
    const char * subst,
    const struct point * p_point,
    long & sizeDiff,
    void * p_error);
extern OT_API char * substituteVariablesOnCommandLine(char * buf,
    const struct WrapperExchangedData * p_exchangedData,
    const char * subst,
    const struct point * p_point,
    long & sizeDiff,
    void * p_error);


/* Look for origRegexp in mystring and parse it as if it was a double. Returns non zero if not found or failure */
extern OT_API int retrieve(const std::string & mystring,
                           const std::string & origRegexp,
                           const std::string & origFormat,
                           const std::string & newRegexp,
                           const std::string & newFormat,
                           const struct regexp_match regmatch,
                           double & value,
                           void * p_error);


/* Pick up data from the file */
extern OT_API int retrieveVariables(char * buf,
                                    const struct WrapperExchangedData * p_exchangedData,
                                    const char * subst,
                                    struct point * p_point,
                                    void * p_error);




#define IS_READABLE_FOR_ME(s, uid)       ( ( (s).st_mode & S_IRUSR ) && ( (s).st_uid == (uid) ) )
#define IS_READABLE_FOR_MY_GROUP(s, gid) ( ( (s).st_mode & S_IRGRP ) && ( (s).st_gid == (gid) ) )
#define IS_READABLE_FOR_ANYBODY(s)         ( (s).st_mode & S_IROTH )
#define IS_READABLE(s,uid,gid) (IS_READABLE_FOR_ME( (s) , (uid) ) || IS_READABLE_FOR_MY_GROUP( (s) , (gid) ) || IS_READABLE_FOR_ANYBODY( (s) ))
#define IS_WRITABLE_FOR_ME(s, uid)       ( ( (s).st_mode & S_IWUSR ) && ( (s).st_uid == (uid) ) )
#define IS_WRITABLE_FOR_MY_GROUP(s, gid) ( ( (s).st_mode & S_IWGRP ) && ( (s).st_gid == (gid) ) )
#define IS_WRITABLE_FOR_ANYBODY(s)         ( (s).st_mode & S_IWOTH )
#define IS_WRITABLE(s,uid,gid) (IS_WRITABLE_FOR_ME( (s) , (uid) ) || IS_WRITABLE_FOR_MY_GROUP( (s) , (gid) ) || IS_WRITABLE_FOR_ANYBODY( (s) ))
#define IS_READWRITABLE(s,uid,gid) ( IS_READABLE( (s) , (uid) , (gid) ) && IS_WRITABLE( (s) , (uid) , (gid) ) )



/* Check that the directory is usable (existence and read/write access) */
extern OT_API int checkDirectory(const char * directory, void * p_error);

#ifndef WIN32
/* Run the command passed as argument in the specified directory. Set its environment approprietaly */
extern OT_API int runCommand(const char * command, const char * directory, const struct WrapperExchangedData * p_exchangedData, const struct point * p_point, void * p_error);
#endif


#endif /* OPENTURNS_WRAPPERCOMMONFUNCTIONS_HXX */
