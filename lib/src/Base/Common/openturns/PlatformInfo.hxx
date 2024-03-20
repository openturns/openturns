//                                               -*- C++ -*-
/**
 *  @brief The class PlatformInfo gives information about the library
 *
 *  Copyright 2005-2024 Airbus-EDF-IMACS-ONERA-Phimeca
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
#ifndef OPENTURNS_PLATFORMINFO_HXX
#define OPENTURNS_PLATFORMINFO_HXX

#include <map>
#include "openturns/OTprivate.hxx"
#include "openturns/Description.hxx"

BEGIN_NAMESPACE_OPENTURNS



/**
 * @class PlatformInfo
 * @brief Defines static methods to query the library about various information (name, version, etc.)
 */

class OT_API PlatformInfo
{
private:
  /**
   * The numerical precision used to write out scientific numbers
   */
  static UnsignedInteger Precision_;

public:

  /** Return the version number of OpenTURNS */
  static String GetVersion();

  /** Return the revision of the versioning system this version corresponds to (broken for the moment) */
  static String GetRevision();

  /** Return the date OpenTURNS was built */
  static String GetDate();

  /** Return the path OpenTURNS was installed to */
  static String GetInstallationDirectory();

  /** Return the precision used for floating %point output */
  static UnsignedInteger GetNumericalPrecision();

  /** Define the precision used for floating %point output
   * @param precision A positive integer counting the number of digits shown
   */
  static void SetNumericalPrecision(SignedInteger precision);

  /** Features accessor */
  static Description GetFeatures();
  static Bool HasFeature(const String & feature);

  /** Compiler id GNU/MSVC... */
  static String GetCompilerId();

  /** Compiler version */
  static String GetCompilerVersion();
private:

  PlatformInfo();

  static std::map<String, Bool> Features_;

}; /* class PlatformInfo */



END_NAMESPACE_OPENTURNS

#endif /* OPENTURNS_PLATFORMINFO_HXX */
