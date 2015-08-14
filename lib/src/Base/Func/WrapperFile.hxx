//                                               -*- C++ -*-
/**
 *  @brief This class provides all the treatments for wrapper file manipulation
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
#ifndef OPENTURNS_WRAPPERFILE_HXX
#define OPENTURNS_WRAPPERFILE_HXX

#include <iostream>              // for std::ostream
#include "OTprivate.hxx"
#include "PersistentObject.hxx"
#include "WrapperData.hxx"

BEGIN_NAMESPACE_OPENTURNS

/**
 * @class WrapperFile
 * @brief Provides all the treatments for wrapper file manipulation
 *
 * All the methods needed for wrapper description file reading or writing
 * are located in this class. The class handles various file formats (plain text,
 * XML, etc.).
 */

class OT_API WrapperFile
  : public PersistentObject
{
  CLASSNAME;
public:

  typedef int Version;

  /** @brief Find the path of a wrapper file from its name.
   * @param name The name of the wrapper expurged of its extension
   * @return The absolute path of the corresponding wrapper description file
   * @throw NoWrapperFileFoundException
   * The method uses Path class to implements its search algorithm.
   * @see Path
   */
  static FileName FindWrapperPathByName(const String & name);

  /** @brief Build a wrapper from a name
   * @param name The name of the wrapper expurged of its extension
   * @return A structure describing the content of the wrapper description file
   * @throw NoWrapperFileFoundException
   */
  static WrapperFile FindWrapperByName(const String & name);

  /** @brief Build a wrapper from a stream
   * @param xmlStream A stream (in XML format) that is the content of a wrapper description file
   * @return A structure describing the content of the wrapper description file
   * @see toString()
   */
  static WrapperFile BuildWrapperFromStream(const String & xmlStream);

protected:

  /** The file name extension */
  static const String extension_;


public:

  /** Default constructor */
  WrapperFile();

  /** Constructor from file */
  explicit WrapperFile(const FileName & pathToFile);

  /* Virtual constructor */
  virtual WrapperFile * clone() const;

  /* String converter */
  virtual String __repr__() const;
  virtual String __str__(const String & offset = "") const;

  /** Description file path accessor
   * @param path The path of the wrapper description file
   */
  void setDescriptionFilePath(const FileName & path);
  /** Description file path accessor
   * @return The path of the wrapper description file
   */
  FileName getDescriptionFilePath() const;

  /** Wrapper data accessor
   * @param data A structure describing the content of the wrapper description file
   */
  void setWrapperData(const WrapperData & data);
  /** Wrapper data accessor
   * @return A structure describing the content of the wrapper description file
   */
  const WrapperData & getWrapperData() const;

  /** Write the internal data to a wrapper file
   * @param pathToFile The path where the wrapper description file must be written
   */
  void writeFile(const FileName & pathToFile);

  /** Stream out the internal data
   * @return A string that can be later used by BuildWrapperFromStream()
   */
  String toString() const;

  /** Accessor to version number */
  Version getVersion() const;
  void setVersion(Version v);

protected:

  /** Where the description file of the wrapper is located */
  FileName descriptionFilePath_;

  /** The data that the wrapper may exchange with the platform */
  WrapperData data_;

  /** The version of DTD used to write the description file */
  Version version_;

private:

  /** Initialization */
  void init() const;

  /** Finalization */
  void done() const;

  /** File parsing function */
  void parseFile(const FileName & pathToFile);

  /** Stream parsing function */
  void parseStream(const String & stream);

} ; /* class WrapperFile */


END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_WRAPPERFILE_HXX */
