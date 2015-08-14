//                                               -*- C++ -*-
/**
 *  @brief The class LibraryLoader loads ans manages all dynamic libraries
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
#include "OTconfig.hxx"
#include "OTdebug.h"
#ifdef OPENTURNS_HAVE_LIBDL
BEGIN_C_DECLS
#include <dlfcn.h>               // for dlopen, dlsym, dlclose, dlerror
END_C_DECLS
#endif
#include <vector>                // for std::vector
#include "OSS.hxx"
#include "LibraryLoader.hxx"
#include "Path.hxx"
#include "Exception.hxx"
#include "Log.hxx"

BEGIN_NAMESPACE_OPENTURNS



CLASSNAMEINIT(LibraryLoader);


/* The unique instance of the Singleton model */
LibraryLoader * LibraryLoader::p_instance_ = 0;

/* Default constructor */
LibraryLoader::LibraryLoader()
  : libraryCatalog_()
{
  // Nothing to do
}

/* Copy constructor */
LibraryLoader::LibraryLoader(const LibraryLoader & other) ///< The copied LibraryLoader
  : Lockable(other)
  , libraryCatalog_()
{
  // Nothing to do
}

/* Destructor */
LibraryLoader::~LibraryLoader()
{
#if defined(OPENTURNS_HAVE_LIBDL) || defined(_MSC_VER)
  Lock lock(this);

  // We close all libraries one after another
  CatalogType::const_iterator iterator;
  for(iterator =  libraryCatalog_.begin();
      iterator != libraryCatalog_.end();
      ++iterator)
  {
#ifdef _MSC_VER
    if (FreeLibrary((HMODULE) (*iterator).second))
    {
      char * buffer = NULL;
      DWORD error_code = GetLastError();
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &buffer, 0, NULL);
      String msg(buffer);
      LocalFree(buffer);
      throw DynamicLibraryException(HERE) << "Cannot FreeLibrary library located at '"
                                          << (*iterator).first << "'. Reason : "
                                          << msg;
    }
#else
    if (dlclose((*iterator).second))
      throw DynamicLibraryException(HERE) << "Cannot dlclose library located at '"
                                          << (*iterator).first << "'. Reason : "
                                          << dlerror();
#endif
  } /* end for */
#else /* OPENTURNS_HAVE_LIBDL */
  throw NotYetImplementedException(HERE) << "In LibraryLoader::~LibraryLoader(): need libdl";
#endif /* OPENTURNS_HAVE_LIBDL */
}


/* Assignment */
LibraryLoader & LibraryLoader::operator =(const LibraryLoader & other)
{
  return *this;
}


/* String converter */
String LibraryLoader::__repr__() const
{
  OSS oss(true);
  oss << "class=LibraryLoader"
      << " catalog=[";

  {
    /* Lock block */
    Lock lock(this);

    const char * separator = "";
    CatalogType::const_iterator iterator;
    for(iterator =  libraryCatalog_.begin();
        iterator != libraryCatalog_.end();
        ++iterator, separator = ", ")
    {
      oss << separator
          << "('" << (*iterator).first << "'," << (*iterator).second << ")";
    } /* end for */

  } /* end Lock block */

  oss << "]";
  return oss;
}

String LibraryLoader::__str__(const String & offset) const
{
  return __repr__();
}


/* The accessor to the Singleton element */
LibraryLoader & LibraryLoader::GetInstance()
{
  if (!p_instance_)
  {
    // this is the first call, we have to create the instance
    p_instance_ = new LibraryLoader;
  } /* end if */
  return *p_instance_;
}


/*
 * Loads the library located at path
 */
Library LibraryLoader::load(const FileName & path)
{
#if defined(OPENTURNS_HAVE_LIBDL) || defined(_MSC_VER)
  Lock lock(this);

  CatalogType::const_iterator foundLibraryIterator = libraryCatalog_.find(path);

  // Load the library if not already done
  if (foundLibraryIterator == libraryCatalog_.end())
  {

    // Get the directory list...
    std::vector<FileName> directoryList = Path::GetWrapperDirectoryList();

    // ... and search it for the file
    FileName libraryFullPath;
    libraryFullPath = Path::FindFileByNameInDirectoryList(path, directoryList);


#ifdef _MSC_VER
    void * theLibraryHandle = LoadLibrary(libraryFullPath.c_str());
    if (theLibraryHandle == NULL)
    {
      char * buffer = NULL;
      DWORD error_code = GetLastError();
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR) &buffer, 0, NULL);
      String msg(buffer);
      LocalFree(buffer);
      throw DynamicLibraryException(HERE) << "Cannot LoadLibrary library located at '"
                                          << libraryFullPath << "'. Reason : "
                                          << msg;
    }
#else
    void * theLibraryHandle = dlopen(libraryFullPath.c_str(), RTLD_NOW);
    if (theLibraryHandle == NULL)
      throw DynamicLibraryException(HERE) << "Cannot dlopen library located at '"
                                          << libraryFullPath << "'. Reason : "
                                          << dlerror();
#endif

    LOGINFO(OSS() << "Successful load of dynamic library located at " << libraryFullPath );

    // Add the library to the catalog
    libraryCatalog_[path] = theLibraryHandle;

    return Library(theLibraryHandle, path);


  }
  else     // The library is already loaded
  {
    return Library((*foundLibraryIterator).second, (*foundLibraryIterator).first);
  } /* end if */
#else /* OPENTURNS_HAVE_LIBDL */
  throw NotYetImplementedException(HERE) << "In LibraryLoader::load(): need libdl";
#endif /* OPENTURNS_HAVE_LIBDL */
}



/*
 * @fn std::ostream & operator <<(std::ostream & os, const LibraryLoader & obj)
 * @brief Output stream converter
 * @param os A STL output stream object
 * @param obj The object read by \em os
 * @return A reference to \em os
 */
std::ostream & operator <<(std::ostream & os, const LibraryLoader & obj)
{
  return os << obj.__repr__();
}

OStream & operator <<(OStream & OS, const LibraryLoader & obj)
{
  return OS << obj.__str__();
}


END_NAMESPACE_OPENTURNS
