//                                               -*- C++ -*-
/**
 *  @brief A Library object points to an actual shared library
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
#ifndef OPENTURNS_LIBRARY_HXX
#define OPENTURNS_LIBRARY_HXX

#include <iostream>              // for std::ostream
#include "OTprivate.hxx"
#include "PersistentObject.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class Library
 * @brief Points to an actual shared library
 *
 * Library defines an abstraction of a shared library.
 * It stores a handle onto the actual opened library and
 * gives access to its symbols.
 */

class OT_API Library
  : public PersistentObject
{
  CLASSNAME;
public:


  friend class LibraryLoader;

  /* Virtual constructor */
  virtual Library * clone() const;

  /* Comparison operator */
  Bool operator ==(const Library & other) const;

  /* String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** @brief Library symbol accessor
   * This method retrieves the symbol stored in the dynamic library and casts it to make it usable afterwards.
   * If the symbol wasn't found a DynamicLibraryException is raised indicating the failure.
   * @throw DynamicLibraryException
   * @param symbolName The name of the symbol to look for
   * @result A symbol usable by other OpenTURNS objects
   */
  LibrarySymbol getSymbol(String symbolName) const;

protected:

private:

  /** The handle to the actual shared library */
  const LibraryHandle handle_;

  /** The path of the library */
  String path_;

  /**
   * Default constructor
   */
  Library(LibraryHandle handle, const String & path);

}

; /* class Library */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_LIBRARY_HXX */
