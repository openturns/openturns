//                                               -*- C++ -*-
/**
 *  @brief The class LibraryLoader loads and manages all dynamic libraries
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
#ifndef OPENTURNS_LIBRARYLOADER_HXX
#define OPENTURNS_LIBRARYLOADER_HXX

#include <iostream>              // for std::ostream
#include <map>                   // for std::map
#include "OStream.hxx"
#include "Library.hxx"
#include "Lockable.hxx"


BEGIN_NAMESPACE_OPENTURNS



/**
 * @class LibraryLoader
 * @brief Loads and manages all dynamic libraries
 *
 * The LibraryLoader class acts as a general factory to load and unload
 * dynamic libraries (Unix shared objects or Windows DLL) on demand.
 * The class is based on the 'singleton' design pattern.
 * Libraries are loaded only once and handles are stored in a map.
 * @see Library
 * @todo Library reference counting
 * @toto Release method definition and implementation
 */

class OT_API LibraryLoader
  : public Lockable
{
  CLASSNAME;
public:


  /** The accessor to the Singleton element */
  static LibraryLoader & GetInstance();

  /* Destructor */
  ~LibraryLoader();

  /* String converter */
  String __repr__() const;
  String __str__(const String & offset = "") const;

  /** @brief Loads the library at path
   * This method takes the path, either relative or absolute, to a dynamic loadable object
   * and tries to lod it. If the object was already loaded, the previous reference is returned.
   * It not the object is searched for on the filesystem. If it can't be found a
   * FileNotFoundException is raised. If it is found but can't be loaded a DynamicLibraryException
   * is raised.
   * @throw FileNotFoundException
   * @throw DynamicLibraryException
   * @param path A path to the object to be loaded
   * @result An handle to the Library usable afterwars by other OpenTURNS objects
   */
  Library load(const FileName & path);

protected:

private:

  /**
   * The map that stores all the handles on loaded dynamic libraries
   * Key is the path of the library, value is the handle onto the library.
   * This map is protected against concurrent accesses
   */
  typedef std::map<FileName, LibraryHandle> CatalogType;
  CatalogType libraryCatalog_;

  /** The unique instance of the Singleton model */
  static LibraryLoader * p_instance_;

  /** Default constructor */
  LibraryLoader();

  /** Copy constructor */
  LibraryLoader(const LibraryLoader & other);

  /** Assignment */
  LibraryLoader & operator =(const LibraryLoader & other);

}

; /* class LibraryLoader */

/**
 * @fn std::ostream & operator <<(std::ostream & os, const LibraryLoader & obj)
 * @brief Output stream converter
 * @param os A STL output stream object
 * @param obj The object read by \em os
 * @return A reference to \em os
 */
OT_API std::ostream & operator <<(std::ostream & os, const LibraryLoader & obj);
OT_API OStream & operator <<(OStream & OS, const LibraryLoader & obj);

END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LIBRARYLOADER_HXX */
